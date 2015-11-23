#!/bin/bash

set -o nounset
set -o errexit

if [ `whoami` != 'vagrant' ]
then
    echo -e "\e[31mPlease run this script from inside of the Docker container\e[0m"
    exit 1
fi

if [ $# -gt 0 ]
then
    case "$1" in
        -h|--help)
            echo "Usage: $0 [options]"
            echo
            echo "options:"
            echo "-h, --help        print this usage and exit"
            echo "-c, --clean       clean all build files"
            exit 0
            ;;
        -c|--clean)
            rm -rf /home/vagrant/lss/build
            exit 0
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
cmake -v ..
make -j 4
make install

echo -e "\e[32mBuilding project ... DONE\e[0m"

popd > /dev/null