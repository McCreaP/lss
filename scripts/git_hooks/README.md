# Git hooks

This directory contains git hook - annoying script running before every `git push`. 
To successfully push to the remote repository the code has to compile and must be verified by **cpplint**.
To achieve this git hook runs scripts: **build.sh** and **cpplint.sh**. 
You can check your code with these scripts independently.

Additionally, before pushing to the master branch the code should have sufficient test coverage and all tests have to pass.
There is no yet git hook which verify it but you can manually run **coverage.sh** to obtain this information.

## Installation

Please, enable the hook by typing in the **repository root directory**:

`ln -s ../../scripts/git_hooks/pre-push.sh .git/hooks/pre-push`