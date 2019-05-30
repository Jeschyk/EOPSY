#!/bin/sh

    if [ -e lastExample ];
    then
    rm -r lastExample
    echo "The file, lastExample has been deleted"
    fi

createExamples()
echo "The goal of the modify.sh is to change the names of the files, according to the given sed expression/regex.\n"
{
    mkdir example
    cd example
    mkdir Folder1 FOLdeR2 folder3
    cd Folder1
    touch xyz.txt
    cd ..
    cd FOLdeR2
    touch abc.doc kpp.txt
    cd ..
    cd folder3
    mkdir FOLDER4
    cd FOLDER4
    touch arc.txt
    cd ..
    touch pkp.txt
    cd ..
    cd ..
}

clear
createExamples
echo "Default structure:\n"
find example
echo "\n-----------------------\nUppercase (without recursion):"
./modify.sh -u example
echo "Changed structure:\n"

find EXAMPLE
echo "\n-----------------------\nLowercase (with recursion):"
./modify.sh -r -l EXAMPLE
echo "Changed structure:\n"

find example
echo "\n-----------------------\nUppercase (with recursion):"
./modify.sh -r -u example
echo "Changed structure:\n"

find EXAMPLE
echo "\n-----------------------\nLowercase (without recursion):"
./modify.sh -l EXAMPLE
echo "Changed structure:\n"

find example
echo "\n-----------------------\nSeed example/lastExample"  
./modify.sh -r 's/example/lastExample/' example
echo "Changed structure:\n"

find lastExample
echo "\n-----------------------\nSeed lastExample/lastExample-rename with the same name "
./modify.sh -r 's/lastExample/lastExample/' lastExample
echo "Changed structure:\n"

find lastExample
echo "\n-----------------------\nWrong command, help will be displayied"
./modify.sh -l

echo "\n-----------------------\nWrong command, help will be displayied one more time with another information"
./modify.sh -r
