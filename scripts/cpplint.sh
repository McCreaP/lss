#!/bin/bash

if [ `whoami` != 'vagrant' ]
then
    echo -e "\e[31mPlease run this script from inside of the Docker container\e[0m"
    exit 1
fi

echo -e "\e[32mRunning cpplint ...\e[0m"

pushd `pwd`
cd /home/vagrant/lss

./../cpplint.py --project=lss --root=src --filter=-legal/copyright `find src -name *.cc -o -name *.cpp -o -name *.h`
EXIT_CODE=$?

popd > /dev/null

if [ ${EXIT_CODE} -eq 0 ]
then
    echo -e "\e[32mRunning cpplint ... DONE\e[0m"
else
    echo -e "\e[31mRunning cpplint ... DONE\e[0m"
fi

exit ${EXIT_CODE}