# Geom++ 

[![Build / Test (Windows)](https://github.com/amastrobera/geompp/actions/workflows/build_test_windows.yml/badge.svg)](https://github.com/amastrobera/geompp/actions/workflows/build_test_windows.yml)

[![Build / Test (Linux)](https://github.com/amastrobera/geompp/actions/workflows/build_test_linux.yml/badge.svg)](https://github.com/amastrobera/geompp/actions/workflows/build_test_linux.yml)


## Geometry library for 2D and 3D calculations

The library offers a set of primitive classes to store geometrical data (Point, Ray, LineSegment, Polygon, ... etc). It also contains functionalities for those primities to interact with each other (+/-/*, Intersection, Overlap, ... etc). 

The aim of the library is to be fast, mathematically-correct, well-tested, easy to use and helpful in a variety of languages other than C++, such as Python or C#. 

It is based on C++20 standard.

This geometry library was born to improve the previous [GeomSharp](https://github.com/amastrobera/geom_sharp) libraray, by creating 
(1) better algorithsm, 
(2) faster execution, 
(3) not being limited to C# and .Net Framework. 


## Build it 

### Docker Dev Environemnt

A Docker image is buildable for development and testing environment, both on linux and windows

To build and work with it 

```
cd docker
.\build.bat -image Linux    # if you are on windows and want to build a linux image
                            # or .\build.bat -image Windows
                            # or ./build.sh -image Windows
                            # or ./build.sh -image Linux

.\run.bat -image Linux      # same possibilities 
```

### Download and Install

To download and use the library

##### Linux
On linux, I have used the `g++13`, you can get it with 
```
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt install -y g++-13

# remove and recreate symbolic links
sudo rm -f /usr/bin/g++
sudo rm -f /usr/bin/c++
sudo ln -s /usr/bin/g++-13 /usr/bin/g++
sudo ln -s /usr/bin/c++-13 /usr/bin/c++
```

Then simply build it with cmake
```
mkdir buid
cd build
cmake ..
make -j6

# try run test
./geompp_tests/geompp_tests

# or a specific class / test
./geompp_tests/geompp_tests --gtest_filter="Point2D*"
./geompp_tests/geompp_tests --gtest_filter="Point2D.ToFile"
```

You should see something like this 
![unit test linux](etc/unit_tests_linux.png)

##### Windows
I have Windows 11, and use Visual Studio 2022. 

Once you install [VS 2022](https://visualstudio.microsoft.com/downloads/), launch the Visual Studio Installer, click on "Modify" and install the _Desktop Development with C++_. 

Open Visual Studio, then _Open Folder_ and select the `geompp`. 

Use Ctrl+Shift+B to build the whole solution, and click on the "Play button" or F5 to run all tests.

You should see something like this 
![unit test linux](etc/unit_tests_win_vs.png)


## WIP schedule

follow what's being developed in [/WIP.md](work in progress todo list)