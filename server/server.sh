#!/data/data/com.termux/files/usr/bin/bash

PID_DIR="$HOME/.server-home"
DEFAULT_DIR="."
DEFAULT_PORT=3000

mkdir -p "$PID_DIR"

ensure_browser_sync() {
    if ! command -v browser-sync >/dev/null 2>&1; then
        echo "[*] browser-sync not found. Installing globally..."
        npm install -g browser-sync || { echo "[!] Installation failed."; exit 1; }
    fi
}

parse_args() {
    DIR="$DEFAULT_DIR"
    PORT="$DEFAULT_PORT"

    while [[ $# -gt 0 ]]; do
        case "$1" in
            -p|--port)
                PORT="$2"
                shift 2
                ;;
            *)
                DIR="$1"
                shift
                ;;
        esac
    done

    PID_FILE="$PID_DIR/$PORT.pid"
}

start_server() {
    parse_args "$@"
    cd "$DIR" || { echo "[!] Directory not found: $DIR"; exit 1; }

    ensure_browser_sync

    if [ -f "$PID_FILE" ] && kill -0 "$(cat "$PID_FILE")" 2>/dev/null; then
        echo "[!] Server already running on port $PORT (PID $(cat "$PID_FILE"))"
        return
    fi

    echo "[*] Starting server in $DIR on port $PORT"
    browser-sync start --server "$DIR" --files "$DIR/**/*.html,$DIR/**/*.css,$DIR/**/*.js" --port "$PORT" --no-open --no-ui 2>&1 || { echo "Couldn't start server, exiting..."; exit 1; }
    echo $! > "$PID_FILE"
    echo "[+] Server started with PID $(cat "$PID_FILE")"
}

stop_server() {
    parse_args "$@"

    if [ ! -f "$PID_FILE" ]; then
        echo "[!] No server running on port $PORT."
        return
    fi

    PID=$(cat "$PID_FILE")
    if kill -0 "$PID" 2>/dev/null; then
        kill "$PID"
        echo "[+] Server on port $PORT stopped (PID $PID)"
    else
        echo "[!] Server process for port $PORT is already dead."
    fi
    rm -f "$PID_FILE"
}

status_server() {
    shopt -s nullglob
    pidfiles=("$PID_DIR"/*.pid)
    if [ ${#pidfiles[@]} -eq 0 ]; then
        echo "[*] No running servers."
        return
    fi

    for pidfile in "${pidfiles[@]}"; do
        PORT=$(basename "$pidfile" .pid)
        PID=$(cat "$pidfile")
        if kill -0 "$PID" 2>/dev/null; then
            echo "[*] Port $PORT: Running (PID $PID)"
        else
            echo "[!] Port $PORT: Dead process. Cleaning up."
            rm -f "$pidfile"
        fi
    done
}

restart_server() {
    stop_server "$@"
    sleep 1
    start_server "$@"
}

show_help() {
    echo "Usage: server [start|stop|restart|status|help] [directory] [-p PORT]"
    echo ""
    echo "  start [dir] [-p PORT]    Start server in directory (default: .) on port (default: 3000)"
    echo "  stop [-p PORT]           Stop server on given port"
    echo "  restart [dir] [-p PORT]  Restart server"
    echo "  status                   Show running servers"
    echo "  help                     Show this help message"
}

# Dispatch
CMD="$1"
shift || true

case "$CMD" in
    start)   start_server "$@" ;;
    stop)    stop_server "$@" ;;
    restart) restart_server "$@" ;;
    status)  status_server ;;
    help|"") show_help ;;
    *)       echo "[!] Unknown command: $CMD"; show_help ;;
esac
