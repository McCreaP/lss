#!/bin/bash

source "$(dirname "$BASH_SOURCE")/shared/header.sh"

TARGET_COVERAGE=80

REPORTS_DIR="coverage"

# Default quiet
VERBOSE_FLAG="--quiet"

while [[ $# > 0 ]]
do
    case "$1" in
        -h|--help)
            echo "Usage: $0 [options]"
            echo
            echo "options:"
            echo "-h, --help        print this message and exit"
            echo "-v, --verbose     show more logs"
            exit 0
            ;;
        -v|--verbose)
            VERBOSE_FLAG=""
            ;;
        *)
            echo "Unrecognized flag: $1"
            exit 1
            ;;
    esac
    shift
done

log_info "Measuring unit tests coverage ..."

pushd . > /dev/null

cd /home/vagrant/lss

rm -rf ${REPORTS_DIR}
mkdir -p ${REPORTS_DIR}

lcov --base-directory . --directory . --no-external --zerocounters --quiet
lcov --base-directory . --directory . --no-external --capture --initial --quiet --output-file ${REPORTS_DIR}/base.info
./build/src/unit_tests --gtest_print_time=1 --gtest_output=xml > /dev/null
lcov --base-directory . --directory . --no-external --capture ${VERBOSE_FLAG} --output-file ${REPORTS_DIR}/test.info

cd ${REPORTS_DIR}
lcov --quiet -a base.info -a test.info -o total.info
rm base.info test.info
genhtml --quiet -o html -t "Unit tests coverage" total.info

COVERAGE=$(lcov --summary total.info  2>&1 | grep lines | awk '{print $2}' | tr -d %)

lcov --summary total.info
echo "Current lines coverage: $COVERAGE%"
echo "Target lines coverage: $TARGET_COVERAGE%"
echo "More detailed information can be found in file:///home/vagrant/lss/${REPORTS_DIR}/html/index.html"

EXIT_CODE=0
# COVERAGE cannot be simply compared with TARGET_COVERAGE
# It can be a floating point number
if [ $(echo ${COVERAGE}'>'${TARGET_COVERAGE} | bc -l) -eq 1 ]
then
    log_info "Test coverage meets target"
    log_info "Measuring unit tests coverage ... DONE"
else
    log_error "Test coverage is less than target coverage"
    log_error "Measuring unit tests coverage ... DONE"
    EXIT_CODE=1
fi

popd > /dev/null

exit ${EXIT_CODE}
