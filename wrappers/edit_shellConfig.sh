#!/bin/zsh

# Open bashrc in nano to edit
nvim ~/.zshrc

sleep 3

# Source edited bashrc
source ~/.zshrc || exit 1
echo "zshrc has been edited, and sourced successfully."

exit 0
