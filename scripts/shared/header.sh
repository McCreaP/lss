#!/bin/bash

set -o nounset
set -o errexit

log_info() { echo -e "\e[32m$@\e[0m"; }

log_error() { echo -e "\e[31m$@\e[0m"; }

echo_cmd() {
  echo "$@"
  eval "$@"
}

if [ `whoami` != 'vagrant' ]
then
    log_error "Please run this script from inside of the Docker container"
    exit 1
fi
