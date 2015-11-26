if [ `whoami` != 'vagrant' ]
    then
    echo "Please run from inside of Docker container"
    exit
fi

echo "Checking git submodules"
git submodule update --init --recursive

echo "Compiling sources"
mkdir -p build && cd build
cmake ..
make -j 4

echo "Installing sources"
make install

echo "Success"
