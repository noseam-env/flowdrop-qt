# Build instructions for Windows

All commands (if not stated otherwise) will be launched from **x64 Native Tools Command Prompt for VS 2022.bat** (should be in **Start Menu > Visual Studio 2022** menu folder). Pay attention not to use any other Command Prompt.

## Install third party software

- Install **Bonjour SDK** from `redist/bonjoursdksetup.exe`

## Build Qt

Release: 
```
..\qt-everywhere-src-6.5.0\configure.bat `
        -debug-and-release `
        -force-debug-info `
        -opensource `
        -confirm-license `
        -static `
        -static-runtime `
        -skip qt3d `
        -skip qtcharts `
        -skip qtdatavis3d `
        -skip qtscript `
        -skip qtwebengine `
        -nomake examples `
        -nomake tests `
        -platform win32-arm64-msvc `
        -prefix "C:\Qt-6.5.0-amd64-FlowDrop-Release"
```

Debug: 
```
..\qt-everywhere-src-6.5.0\configure.bat `
        -debug `
        -force-debug-info `
        -opensource `
        -confirm-license `
        -static `
        -static-runtime `
        -skip qt3d `
        -skip qtcharts `
        -skip qtdatavis3d `
        -skip qtscript `
        -skip qtwebengine `
        -nomake examples `
        -nomake tests `
        -platform win32-arm64-msvc `
        -prefix "C:\Qt-6.5.0-amd64-FlowDrop-Debug"
```

Release arm64:
```
..\qt-everywhere-src-6.5.0\configure.bat `
        -release `
        -force-debug-info `
        -opensource `
        -confirm-license `
        -static `
        -static-runtime `
        -skip qt3d `
        -skip qtcharts `
        -skip qtdatavis3d `
        -skip qtscript `
        -skip qtwebengine `
        -nomake examples `
        -nomake tests `
        -prefix "C:\Qt-6.5.0-arm64-FlowDrop-Debug" `
        -platform win32-arm64-msvc -- -DQT_HOST_PATH="C:\Qt-6.5.0-amd64-FlowDrop-Release"
```

Then:

`cmake --build . --parallel`

`cmake --install .`

## Clone source code

`git clone --recursive https://github.com/noseam-env/flowdrop-qt`

"-DCMAKE_PREFIX_PATH=C:\Qt-6.5.0-amd64-FlowDrop-Release"

"-DCMAKE_PREFIX_PATH=C:\Qt-6.5.0-amd64-FlowDrop-Debug"

