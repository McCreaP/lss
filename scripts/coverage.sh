#!/bin/bash

set -o nounset
set -o errexit

if [ `whoami` != 'vagrant' ]
then
    echo -e "\e[31mPlease run this script from inside of the Docker container\e[0m"
    exit 1
fi

TARGET_COVERAGE=80

REPORTS_DIR="coverage"

# Default quiet
VERBOSE_FLAG="--quiet"

if [ $# -gt 0 ]
then
    case "$1" in
        -h|--help)
            echo "Usage: $0 [options]"
            echo
            echo "options:"
            echo "-h, --help        print this usage and exit"
            echo "-v, --verbose     show more logs"
            exit 0
            ;;
        -v|--verbose)
            VERBOSE_FLAG=""
            ;;
        *)
            break
            ;;
    esac
fi

echo -e "\e[32mMeasuring unit tests coverage ... \e[0m"

pushd `pwd`

cd /home/vagrant/lss

rm -rf ${REPORTS_DIR}
mkdir -p ${REPORTS_DIR}

lcov --base-directory . --directory . --no-external --zerocounters --quiet
lcov --base-directory . --directory . --no-external --capture --initial --quiet --output-file ${REPORTS_DIR}/base.info
./build/src/unit_tests
lcov --base-directory . --directory . --no-external --capture ${VERBOSE_FLAG} --output-file ${REPORTS_DIR}/test.info

cd ${REPORTS_DIR}
lcov --quiet -a base.info -a test.info -o total.info
rm base.info test.info
genhtml --quiet -o html -t "Unit tests coverage" total.info

COVERAGE=$(lcov --summary total.info  2>&1 | grep lines | awk '{print $2}' | tr -d %)

lcov --summary total.info
echo "Target lines coverage: $TARGET_COVERAGE%"
echo "More detailed information can be found in file:///home/vagrant/lss/${REPORTS_DIR}/html/index.html"

EXIT_CODE=0
if [ $(echo ${COVERAGE}'>'${TARGET_COVERAGE} | bc -l) -ne 0 ]
then
    echo -e "\e[32mTest coverage is greater than target coverage\e[0m"
else
    echo -e "\e[31mTest coverage is less than target coverage\e[0m"
    EXIT_CODE=1
fi

popd > /dev/null

echo -e "\e[32mMeasuring unit tests coverage ... DONE\e[0m"
exit ${EXIT_CODE}