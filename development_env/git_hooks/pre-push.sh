#!/bin/bash

echo -e "\e[32mRunning pre push script ... \e[0"
echo `pwd`
git stash -q --keep-index

vagrant ssh --command "./lss/build.sh"
[ $? -ne 0 ] && ABORT="TRUE"
vagrant ssh --command "cpplint --project=lss --root=src --filter=-legal/copyright \`find lss/src -type f\`"
[ $? -ne 0 ] && ABORT="TRUE"

git stash pop -q

if [ $ABORT -eq "TRUE" ]
then
    echo "\e[31mRunning pre push script ... FAILED\e[0"
    exit 1
else
    echo "\e[32mRunning pre push script ... DONE\e[0"
    exit 0
fi
