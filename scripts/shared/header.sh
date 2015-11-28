#!/bin/bash

set -o nounset
set -o errexit

log_info() { echo -e "\e[32m${1}\e[0m"; }

log_error() { echo -e "\e[31m${1}\e[0m"; }

if [ `whoami` != 'vagrant' ]
then
    log_error "Please run this script from inside of the Docker container"
    exit 1
fi