#!/bin/bash

echo "---APT Wrapper---"

# Constants
LOG_F="$HOME/.config/package_logs/installed_packages.log"
APT="/data/data/com.termux/files/usr/bin/apt"

# Functions
log_package() {
  grep -qxF "$1" "$LOG_F" || (echo "$1" >> "$LOG_F" && echo "$1 added to logs.")
}

rm_package() {
  grep -qxF "$1" "$LOG_F" && sed -i "s/$1//g" "$LOG_F" && echo "$1 removed from logs."
}


# Make sure log file exists
[ -d "$(dirname $LOG_F)" ] || mkdir -p "$(dirname $LOG_F)"
[ -f "$LOG_F" ] || echo "" > "$LOG_F"

# Main logic
if [ "$1" == "install" ]; then
  # Extract valid package names
  packages=()
  for arg in "${@:2}"; do
    if [[ "$arg" != -* ]]; then
      packages+=("$arg")
    fi
  done

  # Run the apt command and append packages to list
  $APT $@ && for pkg in "${packages[@]}"; do
    log_package "$pkg"
  done

elif [[ "$1" == "remove" || "$1" == "purge" ]]; then
  # Extract valid package names
  packages=()
  for arg in "${@:2}"; do
    if [[ "$arg" != -* ]]; then
      packages+=("$arg")
    fi
  done

  # Run the apt command and remove packages from list
  $APT $@ && for pkg in "${packages[@]}"; do
    rm_package "$pkg"
  done
  # Get rid of empty lines
  sed -i '/^$/d' "$LOG_F"

else
  # Forward other commands directly to apt
  $APT $@
  exit $?
fi
