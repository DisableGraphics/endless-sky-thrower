export PLATFORM="$1"
if [ $PLATFORM == "linux" ]; then
    export BUILD_DIR=builddir
    export INSTALL_DIR=bin
    export PACK_COMMAND="tar -czf"
    export PACK_NAME="esthrower-linux-x86_64.tar.gz"
    export EXE_NAME="esthrower"
    export LIBS=""
elif [ $PLATFORM == "windows" ]; then
    export BUILD_DIR=build-mingw64
    export INSTALL_DIR=bin
    export PACK_COMMAND="zip -r"
    export PACK_NAME="esthrower-windows-x86_64.zip"
    export EXE_NAME="esthrower.exe"
    export LIBS="*.dll"
else
    echo "Unknown platform"
    echo "Available platforms: linux, windows"
    exit 1
fi

mkdir $INSTALL_DIR
cp $BUILD_DIR/$EXE_NAME $BUILD_DIR/$LIBS $INSTALL_DIR
$PACK_COMMAND $PACK_NAME $INSTALL_DIR
