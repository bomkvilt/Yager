#!/bin/bash

# ----------------------| 
function gen_project() {
    if [ ! -d "build/linux" ]; then
        mkdir -p "build/linux"
    fi
    pushd build/linux
    cmake "$1" ../..
    popd
}

function build_project() {
    pushd build/linux
    make all -j8
    popd
}

function run_project() {
    ./build/linux/cmd/cmd
}

# ----------------------| 
echo "linux project generator..."
if [ "$#" == "0" ]; then
    echo "Default args..."
    gen_project
    build_project
    exit 0
fi

for arg in $@; do
    if [ "$arg" == "-p" ]; then
        echo "Production mode..."
        ARGS="-DGN_bProduction=on"
    elif [ "$arg" == "-g" ]; then
        echo "Project generation..."
        gen_project $ARGS
    elif [ "$arg" == "-b" ]; then
        echo "Project building..."
        build_project
    elif [ "$arg" == "-r" ]; then
        echo "Project starting..."
        run_project
    fi
done
echo "...Done"
