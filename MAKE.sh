#!/bin/bash

SOURCE_DIRECTORY="source"
BUILD_DIRECTORY='build'

function log(){
    echo "[${1}] [$(date +"%Y-%m-%dT%H:%M:%SZ")] ${2}"
}

function logInfo (){
    log '.' "${1}"
}

function logError (){
    log '-' "${1}"
}

function logSuccess (){
    log '+' "${1}"
}

function printHelp (){
    logInfo 'Usage: ./MAKE [OPTION]'
    logInfo 'Options:'
    logInfo "       dev   - build a version for devopment purposes"
    logInfo "       build - build a optimaised version"
}

function commandDev (){
    logInfo 'Executing:'
    logInfo "g++ ${SOURCE_DIRECTORY}/**/*.cpp -g --std=c++17 -lrt -lpthread -lcurl -ljsoncpp -W -Wall -Wextra -pedantic"
    
    startTime=$(date +%s)
    g++ $(find ${SOURCE_DIRECTORY}  -type f -name '*.cpp' | tr '\n' ' ') -g --std=c++17 -lrt -lpthread -lcurl -ljsoncpp -W -Wall -Wextra -pedantic
    
    [ $? -eq 0 ] && logSuccess 'build was successful' || logError 'build failed'
    
    endTime=$(date +%s)
    logInfo "took $(expr $endTime - $startTime) secs"
}

function commandBuild (){
    
    #if [ ! -d ${BUILD_DIRECTORY} ]; then
    #    logInfo "Making build directory (${BUILD_DIRECTORY})"
    #    mkdir ${BUILD_DIRECTORY}
    #fi
    
    logInfo 'Executing:'
    logInfo "g++ ${SOURCE_DIRECTORY}/**/*.cpp  -o LogExporter.exe --std=c++17 -O3 -lrt -lpthread -lcurl -ljsoncpp -W -Wall -Wextra -pedantic"
    
    startTime=$(date +%s)
    g++ $(find ${SOURCE_DIRECTORY}  -type f -name '*.cpp' | tr '\n' ' ') -o 'LogExporter.exe' --std=c++17 -O3 -lrt -lpthread -lcurl -ljsoncpp -W -Wall -Wextra -pedantic
    
    [ $? -eq 0 ] && logSuccess 'build was successful' || logError 'build failed'
    
    endTime=$(date +%s)
    logInfo "took $(expr $endTime - $startTime) secs"
}


# ~~~ Main ~~~

if [ $# -lt 1 ]; then
    printHelp
    exit 1
fi

case $1 in
    "dev" | "DEV")
        commandDev
    ;;
    "build" | "BUILD")
        commandBuild
    ;;
    *)
        printHelp
esac
