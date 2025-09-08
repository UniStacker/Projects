#!/bin/bash


if [ $# -lt 1 ]; then
  echo "Usage: $0 [file_path]"
fi

if [[ ! -f "$1" ]]; then
  echo "ERROR: The provided is not a file \"$1\""
fi

apt update && apt upgrade -y
apt install $(cat "$1")

exit 0
