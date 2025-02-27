#!/bin/bash

help() {
    echo \
'
    Usage: '$0' <output> <params_to_echo>
'
}

if [ $# -lt 1 ]; 
then
       help
       exit 1
fi

echo "$2" "$1"