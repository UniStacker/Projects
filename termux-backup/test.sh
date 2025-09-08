#!/bin/bash

PATTERN=$(awk '{printf "! -name \"%s\" ", $0}' "./exclude.txt")

echo "$PATTERN"
