# Build instructions for macOS

## Build Qt

Release:
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
        -prefix "/usr/local/qt-6.5.0-flowdrop-release" \
        -platform macx-clang -- -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"
```

Debug:
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
        -prefix "/usr/local/qt-6.5.0-flowdrop-debug" \
        -platform macx-clang -- -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"
```

Then:

`cmake --build . --parallel`

`cmake --install .`

## Clone source code

`git clone --recursive https://github.com/noseam-env/flowdrop-qt`

Set option `-DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"`

"-DCMAKE_PREFIX_PATH=/usr/local/qt-6.5.0-flowdrop-release"

"-DCMAKE_PREFIX_PATH=/usr/local/qt-6.5.0-flowdrop-debug"
