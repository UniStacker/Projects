#!/bin/bash

# Changes directory and list first 5 items
cd() {
    # Run the actual cd command
    builtin cd "$@" || return

    # Print the first 5 items with correct colors
    ls --color=always | head -n 5

    # Check if there are more than 5 items
    total_items=$(ls | wc -l)
    if [ "$total_items" -gt 5 ]; then
        echo "..."
    fi
}
