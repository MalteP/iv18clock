#!/bin/bash

#
# convert lines of the form
#
# 23c23
# < e066
# ---
# > e06c
#
# (as generated by a 2-byte hexdump of the flash image) into hes from:
#
# (0x17) 0x2c 
# < e066
# > e06c
#

while read line; do
    [[ "$line" =~ '---' ]] && continue
    if [[ "$line" =~ ([0-9]+)c([0-9]+) ]]; then
        old=$((${BASH_REMATCH[2]} - 1))
        new=$((${BASH_REMATCH[1]} - 1))
        printf "\n%s\n" "$line"
        if [ "$old" = "$new" ]; then
            printf "(%#04x) %#04x:\n" \
                "$old" "$((old * 2))"
        else
            printf "(%#04x) %#04x...(%#04x) %#04x:\n" \
                "$old" "$((old * 2))" \
                "$new" "$((new * 2))" 
        fi
    else
        printf "%s\n" "$line"
    fi
done
