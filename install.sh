#!/bin/bash

set -o nounset
set -o errexit

if [ `whoami` != 'vagrant' ]
then
    echo -e "\e[31mPlease run this script from inside of the Docker container\e[0m"
    exit 1
fi

BUILD_TYPE="Debug"
MAKE_VERBOSE_FLAG=""

if [ $# -gt 0 ]
then
    case "$1" in
        -h|--help)
            echo "Usage: $0 [options]"
            echo
            echo "options:"
            echo "-h, --help        print this usage and exit"
            echo "-c, --clean       clean all build files"
            echo "-r, --release     build in the release mode (default: debug)"
            echo "-v, --verbose     show more logs"
            exit 0
            ;;
        -c|--clean)
            echo "rm -rf /home/vagrant/lss/build"
            rm -rf /home/vagrant/lss/build
            exit 0
            ;;
        -r|--release)
            BUILD_TYPE="Release"
            ;;
        -v|--verbose)
            MAKE_VERBOSE_FLAG="VERBOSE=1"
            ;;
        *)
            break
            ;;
    esac
fi

echo -e "\e[32mBuilding project ...\e[0m"

pushd `pwd`
cd /home/vagrant/lss

mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..
make $MAKE_VERBOSE_FLAG -j 4
make install

popd > /dev/null

echo -e "\e[32mBuilding project ... DONE\e[0m"