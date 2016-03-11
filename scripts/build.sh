#!/bin/bash

source "$(dirname "$BASH_SOURCE")/shared/header.sh"

BUILD_TYPE="Debug"
MAKE_VERBOSE_FLAG=""

while [[ $# > 0 ]]
do
    case "$1" in
        -h|--help)
            echo "Usage: $0 [options]"
            echo
            echo "options:"
            echo "-h, --help        print this message and exit"
            echo "-c, --clean       clean all build files"
            echo "-r, --release     build in the release mode (default: debug)"
            echo "-v, --verbose     show more logs"
            exit 0
            ;;
        -c|--clean)
            echo_cmd rm -rf "/home/vagrant/lss/"{build,bin}
            exit 0
            ;;
        -r|--release)
            BUILD_TYPE="Release"
            ;;
        -v|--verbose)
            MAKE_VERBOSE_FLAG="VERBOSE=1"
            ;;
        *)
            echo "Unrecognized flag: $1"
            exit 1
            ;;
    esac
    shift
done

log_info "Building project ..."

pushd .
cd /home/vagrant/lss

mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..
make $MAKE_VERBOSE_FLAG -j `nproc`
make install

popd > /dev/null

log_info "Building project ... DONE"
