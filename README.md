# Large Scale Scheduler

### Organizational matters
* We're having a video conference every Monday at 19:00.
* We meet at the Proseminar every Thursday at 8:30

### Technical matters
* The code is written in __C++14__.
* We adhere to [__Google C++ Style Guide__](https://google-styleguide.googlecode.com/svn/trunk/cppguide.html).
* We use __GoogleTest__. Code without proper tests cannot be
  pushed to master.
* The project is built with __CMake__.
* We use __Bitbucket issue tracker__.
* We write a self-documented source code. The code is also documented by __README.md__ in project directories. No additional documentation is planned to be prepared.
* Code is being developed in separate __feature branches__ and regularly merged
  into master.
* To merge a branch into master the code __must be reviewed__ by another person,
  it must have sufficent test coverage and all the tests have to pass.
  
### Building, running and testing
* This repo contains git submodules, so you need to clone it with `--recursive` option or just run `git submodule update --init --recursive` on existing repo to pull them.
* Create and enter build directory, which will contain all build files and cmake cache `mkdir -p directory && cd build`
* Generate makefile `cmake ..`. There is no need to repeat it every time, cause generated makefile automatically detects changes in CMakeLists.txt
* Run `make lss` for building scheduler or `make unit_tests`. You can use `-j [Number of jobs]` make option to run in parallel and speed up the build.
* Scheduler will be placed in `build/src/lss` and tests in `build/test/unit_tests`