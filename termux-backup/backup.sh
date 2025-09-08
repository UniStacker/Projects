#!/bin/bash

set -e  # Exit on error

# Constants
SRC_DIR="$(dirname "$(readlink -f "$0")")"
BAK_DIR="$Sdcard/Download/Termux_Backups"

# Ensure backup directory exists
mkdir -p "$BAK_DIR"

# Handle backup option
if [[ "$1" == "-b" || -z "$1" ]]; then
  echo "Making a backup..."
  echo "The following directories/files will be backed:"

  # Generate exclude pattern for find
  EXCLUDE_PATTERN=$(awk '{printf "! -name \"%s\" ", $0}' "$SRC_DIR/exclude.txt")

  # List files to be backed up
  eval find ~ -maxdepth 1 $EXCLUDE_PATTERN

  tar --exclude-from="$SRC_DIR/exclude.txt" -czf "$BAK_DIR/$(date +%d-%m-%Y_%H-%M).tar.bak" $(ls -A)
  echo "Backup created at '$BAK_DIR'."
  exit 0
fi

# Handle restore option
if [[ "$1" == "-r" ]]; then
  if [[ -n "$2" ]]; then
    if [[ ! -f "$2" ]]; then
      echo "Error: The file '$2' does not exist."
      exit 1
    fi
    echo "Restoring backup from '$2'..."
    tar -xf "$2" -C "$HOME"
  else
    if [[ -z "$(ls -A "$BAK_DIR" 2>/dev/null)" ]]; then
      echo "No backups found in '$BAK_DIR'."
      exit 1
    fi
    LATEST_BACKUP=$(ls -t "$BAK_DIR" | head -n 1)
    echo "Restoring latest backup: '$LATEST_BACKUP'..."
    tar -xf "$BAK_DIR/$LATEST_BACKUP" -C "$HOME"
  fi

  echo "Restore completed successfully."
  source ~/.bashrc
  exit 0
fi

# Handle help option
if [[ "$1" == "-h" ]]; then
  echo "Usage:"
  echo "  1. Make a backup:        $0 -b"
  echo "  2. Restore latest backup: $0 -r"
  echo "  3. Restore a specific backup: $0 -r [/path/to/backup]"
  echo "  4. Show this help message: $0 -h"
  exit 0
fi

# Handle invalid options
echo "Invalid option: $1"
echo "Run '$0 -h' for help."
exit 1
