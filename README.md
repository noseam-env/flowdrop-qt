# FlowDrop: Desktop (Qt-based)

[FlowDrop](https://github.com/noseam-env/flowdrop) is a cross-platform tool that allows sharing files between devices directly over Wi-Fi.

The source code is published under GPL-3.0 license with anti-commercial clause, the license is available [here](https://github.com/noseam-env/flowdrop-qt/blob/master/LICENSE).


## Requirements

Local network that supports mDNS

- Windows: download and install Apple's [Bonjour](https://github.com/noseam-env/flowdrop-cli/raw/master/redist/Bonjour64.msi)
- GNU/Linux: install avahi `sudo apt install avahi-daemon`


## TODO

I will be glad if you help improve this project

- Improve [FlowDrop specification](https://github.com/noseam-env/flowdrop)
- Build [Bonjour](https://github.com/apple-oss-distributions/mDNSResponder) for Windows ARM.


## Build instructions

* [GNU/Linux](docs/building-linux.md)
* [macOS](docs/building-mac.md)
* [Windows](docs/building-win.md)


## Authors

- **Michael Neonov** ([email](mailto:two.nelonn@gmail.com), [github](https://github.com/Nelonn))


## Third-party

* Qt 6.5.0 ([LGPL](http://doc.qt.io/qt-6/lgpl.html))
* Roboto font ([Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0.html))
* Guideline Support Library ([MIT License](https://github.com/Microsoft/GSL/blob/master/LICENSE))
* libknotdeviceinfo ([GNU GPL v3](https://github.com/noseam-env/libknotdeviceinfo/blob/master/LICENSE))
* libflowdrop ([GNU GPL v3 with anti-commercial clause](https://github.com/noseam-env/libflowdrop/blob/master/LICENSE))
* libknotdnssd ([GNU GPL v3](https://github.com/noseam-env/libknotdnssd/blob/master/LICENSE))
* libvirtualtfa ([MIT License](https://github.com/noseam-env/libvirtualtfa/blob/master/LICENSE))
* curl 8.2.1 ([curl License](https://curl.se/docs/copyright.html))
* libhv 1.3.1 ([BSD 3-Clause License](https://github.com/ithewei/libhv/blob/v1.3.1/LICENSE))
* nlohmann_json 3.11.2 ([MIT License](https://github.com/nlohmann/json/blob/v3.11.2/LICENSE.MIT))
* Ninja ([Apache License 2.0](https://github.com/ninja-build/ninja/blob/master/COPYING))
* CMake ([New BSD License](https://github.com/Kitware/CMake/blob/master/Copyright.txt))
