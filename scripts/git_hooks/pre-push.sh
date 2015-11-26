#!/bin/bash

echo -e "\e[32mRunning pre push script ... \e[0m"

git stash -q --keep-index
cd development_env

EXIT_CODE=0
vagrant ssh --command "./lss/scripts/build.sh"
[ $? -ne 0 ] && EXIT_CODE=1
vagrant ssh --command "./lss/scripts/cpplint.sh"
[ $? -ne 0 ] && EXIT_CODE=1
vagrant ssh --command "./lss/scripts/coverage.sh"
[ $? -ne 0 ] && EXIT_CODE=1

git stash pop -q

if [ $EXIT_CODE -eq 0 ]
then
    echo -e "\e[32mRunning pre push script ... DONE\e[0m"
else
    echo -e "\e[31mRunning pre push script ... FAILED\e[0m"
fi

exit ${EXIT_CODE}