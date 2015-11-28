#!/bin/bash

source "$(dirname "$BASH_SOURCE")/shared/header.sh"

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
