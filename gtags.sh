#!/bin/bash

while IFS= read -r line
do    
    if [[ $line == "/"* ]]
    then
        mkdir -p "gtags${line}"
        ln -s $line "gtags${line}"
    fi
done < $1
