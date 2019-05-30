#!/bin/bash

RECURSION=0 #0-no recursion/1-recursion
MODE=0 #0-default/1-lowercase/2-uppercase

help ()
{
    if [ -z != "$1" ]
    then
        echo $1
    fi
    echo "Usage:"
    echo "modify [-r] [-l|-u] <dir/file names...>"
    echo "modify [-r] <sed pattern> <dir/file names...>"
    echo "modify [-h]"
    exit
}

rename ()
{
    if [ "$MODE" == 1 ]         #lowercase mode
    then
        echo "${name,,}"
    elif [ "$MODE" == 2 ]       #uppercase mode
    then
        echo "${name^^}"
    elif [ "$pattern" != "" ]   #sed/pattern mode
    then
        echo "`echo $name | sed $pattern`"  #passing files through given sed pattern 
    fi
}

#main
RECURSION=0         
if [ "$1" == "-h" ] #help display
then
    help
fi                  
if [ "$1" == "-r" ] #here checking if it is a recursive mode
then
    RECURSION=1     #recursive mode set
    shift
else                #otherwise, no-recurvise mode set
    RECURSION=0
fi
                    #after checking if it is recursive mode(first criterion), checking second criterion between: -u, -l, empty and pattern options
if [ "$1" == "-l" ] 
then
    MODE=1          #lowercase mode
elif [ "$1" == "-u" ]
then
    MODE=2          #uppercase mode
elif [ -z "$1" ]
then
    help "Error, Not enough given arguments"
else
    pattern="$1"    #everything else than empty $1, -u or -l will be considered as a pattern
fi
shift

if [ -z "$1" ]
then
    help "Error, You have not provided any filename or directory"
else
    for path in "$@"    #passed as the list/table of parameters
    do
        name=$(basename "$path")    #strips directory informaton and suffixes from file names
        dir=$(dirname "$path")      #strips the last part of a given filename, in effect outputting just the directory components of the pathname
        if [ $RECURSION == 1 ]
        then
            fpath=(`find "$path"`)  #fpath-found path
            for ((i=${#fpath[@]} - 1; i >= 0; i--)) #going through the list from the end, because the first element is a directory(has to be changed as the last one)
            do
                name=$(basename "${fpath[$i]}")
                dir=$(dirname "${fpath[$i]}")
                if [ "${fpath[$i]}" != "$dir/$(rename)" ]   #checking if the new filename is not the same as an old filname for recursion mode
                then
                mv "${fpath[$i]}" "$dir/$(rename)" 2> /dev/null #redirect the error of moving same filenames (for sed pattern) to "the black hole" for the recursion mode
                fi
            done
        else
            if [ "$path" != "$dir/$(rename)" ]              #checking if the new filename is not the same as an old filname
                then
                mv "$path" "$dir/$(rename)" 2> /dev/null    #redirect the error of moving same filenames (for sed pattern) to "the black hole" without recursion
                fi
        fi
    done
fi
