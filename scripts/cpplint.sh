#!/bin/bash

source shared/logging.sh

if [ `whoami` != 'vagrant' ]
then
    log_error "Please run this script from inside of the Docker container"
    exit 1
fi

log_info "Running cpplint ..."

pushd .
cd /home/vagrant/lss

./../cpplint.py --project=lss --root=src --filter=-legal/copyright `find src -name *.cc -o -name *.cpp -o -name *.h`
EXIT_CODE=$?

popd > /dev/null

if (( EXIT_CODE == 0 ))
then
    log_info "Running cpplint ... DONE"
else
    log_error "Running cpplint ... FAILED"
fi

exit ${EXIT_CODE}