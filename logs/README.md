# Logging

We use [**GLog**](https://github.com/google/glog) library for application level logging.

## Quick start example:
```c++
    #include <glog/logging.h>
    int main(int argc, char* argv[]) {
      LOG(INFO) << "Logging example";
    }
```
## Severity levels
You can specify one of the following severity levels:

* `INFO`
Messages intended to show what's going on in the system, at a broad-brush level. `INFO` messages do not indicate any problems and the system should be able to run at full speed in production with `INFO` level logging.
* `WARNING`
Record information about an unexpected event but one that is not an error, i.e. does not prevent correct operation of the system. It is the appropriate level to log invalid data entered by the user/system.
* `ERROR`
Indicates a serious but recoverable error. It signifies a bug in the code that should be fixed - if it does not then it's not an error.
* `FATAL`
An unrecoverable error occurred or one that it is not safe to attempt to recover from. `FATAL` message also indicates a bug or misconfiguration.

Logging a `FATAL` message **terminates** the program.

You can't define your own severity levels, but you can make use of verbose logging.

## Verbose logging

Should be used to log detailed information meant to aid in diagnosing problems and debugging.

**GLog** provides the `VLOG` macro, which allows you
to define your own numeric logging levels.
The `--v` command line option controls which verbose messages are logged:
```c++
   VLOG(1) << "I'm printed when you run the program with --v=1 or higher";
   VLOG(2) << "I'm printed when you run the program with --v=2 or higher";
```

Verbose logging can be controlled from the command line on a per-module basis:

   `--vmodule=greedy=2,*test=3 --v=0`

will:

* Print `VLOG(2)` and lower messages from `greedy.{h,cc}`
* Print `VLOG(3)` and lower messages from files prefixed with `"test"`
* Print `VLOG(0)` and lower messages from elsewhere

## Advanced usage
The exaples of more advanced usage can be found in [GLog documentation](http://google-glog.googlecode.com/svn/trunk/doc/glog.html).
