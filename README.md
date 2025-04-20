# CMake-SDL2-OpenGL-Template

Just an empty project template for cmake, using sdl2, glad and a bunch of other useful things.
Make sure to read to the end.

## Getting Started

This project has preconfigured configurations for MSVC, GCC and Clang and Debug and Release.

When you open the folder in `clion`, you get a dialog where you have to setup all the configurations.
Delete the default `Debug` one, and enable the ones you want:

`Debug/Release-MSVC`
`Debug/Release-Gcc`
`Debug/Release-Clang`

I assume you have the `CMake Tools` addon (the one from microsoft) installed.
When you open it in VSCode you should get a similar prompt, simply pick your config.

See `./CMakeLists.txt` and massage this part to match your setup. CMake's `project` maps to Visual Studio's solution

```cmake
project(Solution
    VERSION 1.0.0
    DESCRIPTION "Your Project"
    HOMEPAGE_URL https://github.com/YourUser/YourProject
)
```

In `./src/CMakeLists.txt` you can find a target i called `Project` you can keep it or massage it to whatever you like.
CMake targets map to projects in Visual Studio. This template only sets up one target. The name of it will also be the
default program binary name. In this case it would be `Project.exe` on windows and `Project` on non windows platforms.

I took the liberty to provide various cmake configurations in `libs/CMakeLists.txt`. Most of the things one might need
in engine/game dev are commented out for now. This template only cares about SDL2, glad and glm so that you can
start right away.

When you uncomment other dependencies, make sure you adjust the includes
in `./src/CMakeLists.txt`'s `target_link_libraries` section.

## What now

Goto `./src/Main.cpp:173` and start adding your stuff.

## Oh, no

When you load this template for the first time and CMake is configuring it for the first time, you might
see this error. That's because glad needs to generate its files first. That happens when you build.
So, just build the project and the error should disappear.

```bash
    .../CMake-SDL2-OpenGL-Template/build/Debug-Gcc/src/CMakeFiles/Project.dir/cmake_pch.hxx:10:10: fatal error: glad/gl.h: No such file or directory
       10 | #include <glad/gl.h>
          |          ^~~~~~~~~~~
```
