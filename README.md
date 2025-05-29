# Shardine

Shardine is a simple-yet-powerful filesystem viewer and editor.

## Building from source

To build from source, you need to install CMake &geq; 3.15 and [vcpkg](https://vcpkg.io).
Then, simply run

    cmake . -DCMAKE_BUILD_TYPE=Release `-DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg`

from the command line, substituting `/path/to/vcpkg` with the path to your vcpkg installation directory.

