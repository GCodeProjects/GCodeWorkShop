Building GCodeWorkShop
======================

GCodeWorkShop is written in C++ using the Qt5 framework. In general,
the following steps are needed to build it:

- Install the C++11 compiler supported by [Qt SDK][1]. You should also
  install the make utility.
- Install Qt utilities such as qmake, moc, lrelease and the resource packer.
- Install the Qt5 framework with version 5.6 or higher.
- Obtain and unzip the GCodeWorkShop source code.
- Create a build folder and navigate to it.
- Run qmake specifying the path to GCodeWorkShop sources.
- Run make to build the application.


Building in Debian
------------------

Install the following packages.

```
apt update
apt install build-essential qtbase5-dev qttools5-dev-tools libqt5serialport5-dev
```

Switch to the folder with sources. Now you can either build the application
step by step or use the script to automatically build the application and
create an installation package.


### Step-by-step build

```sh
mkdir -p build
cd build
qmake -r ..
make lrelease -j $(nproc)
qmake -r ..
make -j $(nproc)
```


### Automatically build and create an package

Run script:

```sh
install/deb-build.sh
```

The script creates a .deb package in the current folder and does not require
root privileges.


Building in windows with MSYS2
------------------------------

Install the following packages.

```sh
pacman -Syu
pacman -S make mingw-w64-x86_64-gcc mingw-w64-x86_64-nsis \
    mingw-w64-x86_64-qt5-tools mingw-w64-x86_64-qt5-translations \
    mingw-w64-x86_64-qt5-base mingw-w64-x86_64-qt5-serialport
```

Switch to the folder with sources. Now you can either build the application
step by step or use the script to automatically build the application and
create an installer.


### Step-by-step build

```sh
mkdir -p build
cd build
qmake -r ..
make lrelease -j $(nproc)
qmake -r ..
make -j $(nproc)
```


### Automatically build and create an installer

Run script:

```sh
install/win-msys2-build.sh
```

The script creates a .exe installer in the current folder.


[1]: https://doc.qt.io/qt-5/supported-platforms.html "Qt5 doc: supported platforms"
