# openE57

![openE57](https://github.com/madduci/openE57/workflows/openE57/badge.svg)

openE57 is a forked version of the original LibE57 (http://www.libe57.org) project, with the intent to refine and optimize the source code in a modern C++ idiomatic way and remove unnecessary dependencies (e.g. Boost) in favour of the C++ Standard Library.

The library is compiled as C++17, since some of following language intrinsics and libraries are used:

* constexpr values
* enum classes
* random (replaces boost::uuid)
* filesystem (replaces boost::filesystem)
* thread (replaces boost::thread)
* memory (replaces boost::shared_ptr and std::auto_ptr)

At the current state, Boost is still required when building the tools as in the original source code, but it will be completely removed with the release 2.0.0.

## Requirements

You need the following tools to build this library:

* A C++17 compiler (MSVC 2017+, gcc 7+, clang 7+)
* A recent version of CMake (3.15+)
* A recent version of conan (1.28+)

## Build Instructions

There are two ways to build and use openE57: building it as a conan package or as a standard CMake project.
The dependencies are now managed with conan and integrated in CMake, without the need of compiling the required libraries by yourself.

### Building as a local conan package

The following instructions will guide you to build openE57 as a local conan package (actually it is being submitted as official conan recipe in the Conan Center), so it can be used it further in other projects.

Available conan options are the following ones:

* with_tools - disabled by default
* shared - disabled by default (not supported at the moment - no symbol is exported yet)
* fPIC - enabled by default, activates the `-fPIC` flag on gcc/clang compilers on Linux/Mac

Platform independent:

```shell
git clone https://github.com/madduci/openE57.git
cd open57
conan create . --build=missing
```

### Building as local project (e.g. for development)

The following instructions will guide you to build openE57 as a standard CMake Project, downloading the external dependencies with conan, but managing all the settings with CMake.

Available CMake Options are:

* BUILD_EXAMPLES
* BUILD_TOOLS
* BUILD_TESTS - actually unsupported (no tests yet available)
* BUILD_SHARED_LIBS - actually unsupported (missing exported symbols)
* BUILD_WITH_MT - activates/disables the `/MT[d]` flag when using Visual Studio
* BUILD_WITH_FPIC - activates/disables the `-fPIC` flag on gcc/clang compilers on Linux/Mac

On Linux:

```shell
git clone https://github.com/madduci/openE57.git
cd open57
mkdir build && cd build
conan install .. --build=missing
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --target install
```

On Windows:

```cmd
git clone https://github.com/madduci/openE57.git
cd open57
md build && cd build
conan create . --build=missing
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_WITH_MT=[TRUE|FALSE]
cmake --build . --config Release --target install
```
