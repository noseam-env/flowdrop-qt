# Build instructions for Linux

## Install third party software

`sudo apt-get install libavahi-compat-libdnssd-dev libnotify-dev`

## Build Qt

`sudo apt-get install libclang-dev libgl1-mesa-dev libgdk-pixbuf2.0-dev`

`sudo apt-get sudo apt install libfontconfig1-dev libfreetype6-dev libx11-dev libx11-xcb-dev libxext-dev libxfixes-dev libxi-dev libxrender-dev libxcb1-dev libxcb-cursor-dev libxcb-glx0-dev libxcb-keysyms1-dev libxcb-image0-dev libxcb-shm0-dev libxcb-icccm4-dev libxcb-sync-dev libxcb-xfixes0-dev libxcb-shape0-dev libxcb-randr0-dev libxcb-render-util0-dev libxcb-util-dev libxcb-xinerama0-dev libxcb-xkb-dev libxkbcommon-dev libxkbcommon-x11-dev`

`sudo apt-get install '^libxcb.*-dev' libx11-xcb-dev libglu1-mesa-dev libxrender-dev libxi-dev libxkbcommon-dev libxkbcommon-x11-dev`

`sudo apt-get purge libzstd-dev`

Release amd64:
```
../qt-everywhere-src-6.5.0/configure \
        -debug-and-release \
        -force-debug-info \
        -opensource \
        -confirm-license \
        -static \
        -skip qt3d \
        -skip qtcharts \
        -skip qtdatavis3d \
        -skip qtscript \
        -skip qtwebengine \
        -nomake examples \
        -nomake tests \
        -opengl es2 \
        -xcb \
		-no-feature-xcb-sm \
		-dbus-runtime \
        -platform /usr/bin/ninja \
        -xplatform linux-g++-64 \
        -prefix "/usr/local/qt-6.5.0-amd64-flowdrop-release"
```

Debug amd64:
```
../qt-everywhere-src-6.5.0/configure \
        -debug \
        -force-debug-info \
        -opensource \
        -confirm-license \
        -static \
        -skip qt3d \
        -skip qtcharts \
        -skip qtdatavis3d \
        -skip qtscript \
        -skip qtwebengine \
        -nomake examples \
        -nomake tests \
        -opengl es2 \
        -xcb \
		-no-feature-xcb-sm \
		-dbus-runtime \
        -platform /usr/bin/ninja \
        -xplatform linux-g++-64 \
        -prefix "/usr/local/qt-6.5.0-amd64-flowdrop-debug"
```

Release arm64:
```
../qt-everywhere-src-6.5.0/configure \
        -debug-and-release \
        -force-debug-info \
        -opensource \
        -confirm-license \
        -static \
        -skip qt3d \
        -skip qtcharts \
        -skip qtdatavis3d \
        -skip qtscript \
        -skip qtwebengine \
        -nomake examples \
        -nomake tests \
        -opengl es2 \
        -platform /usr/bin/ninja \
        -xplatform linux-aarch64-gnu-g++ \
        -sysroot ~/sysroot \
            -I ~/sysroot/usr/include \
            -L ~/sysroot/usr/lib \
            -L ~/sysroot/usr/lib/aarch64-linux-gnu \
            -L ~/sysroot/lib
        -prefix "/usr/local/qt-6.5.0-arm64-flowdrop-release"
```

Then:

`cmake --build . --parallel`

`sudo cmake --install .`

## Clone source code

`git clone --recursive https://github.com/noseam-env/flowdrop-qt`

"-DCMAKE_PREFIX_PATH=/usr/local/qt-6.5.0-amd64-flowdrop-release"

"-DCMAKE_PREFIX_PATH=/usr/local/qt-6.5.0-amd64-flowdrop-debug"

"-DCMAKE_PREFIX_PATH=/usr/local/qt-6.5.0-arm64-flowdrop-debug"
