cmake_minimum_required(VERSION 3.22) # Qt 6.9 requires 3.22 or greater
project(
        Shardine
        VERSION 0.1
        DESCRIPTION "A simple filesystem viewer and editor"
        LANGUAGES CXX
)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        LinguistTools
        Svg
        REQUIRED
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(Shardine)

set(HEXEDIT_QT_VERSION 6)
target_include_directories(Shardine PUBLIC external)
target_include_directories(Shardine PRIVATE src)
configure_file(src/utils/project.h.in autogen/project.h @ONLY)
target_include_directories(Shardine PRIVATE ${CMAKE_BINARY_DIR}/autogen)
add_library(hexedit INTERFACE)
target_include_directories(hexedit INTERFACE ${PROJECT_SOURCE_DIR}/external/hexedit/include)

target_sources(Shardine
        PUBLIC
        external/hexedit/include/hexedit/hexedit.hpp
        external/hexedit/include/hexedit/chunks.hpp
        external/hexedit/include/hexedit/commands.hpp
        external/hexedit/src/library/hexedit.cpp
        external/hexedit/src/library/chunks.cpp
        external/hexedit/src/library/commands.cpp
        PRIVATE
        tr.qrc
        src/main.cpp
        src/ui/MainWindow.cpp
        src/ui/MainWindow.h
        src/ui/MainWindow.ui
        src/ui/StructureModel.cpp
        src/ui/StructureModel.h
        src/ui/SegmentModel.h
        src/ui/CreateFS.cpp
        src/ui/CreateFS.h
        src/ui/CreateFS.ui
        src/ui/images.qrc
        src/minix/MinixFS.cpp
        src/minix/MinixFS.h
        src/minix/SuperBlock.h
        src/minix/INode.h
        src/filesystem/MinixAdapter.cpp
        src/filesystem/MinixAdapter.h
        src/filesystem/Segment.h
        src/filesystem/FileSystem.h
        src/filesystem/Factory.cpp
        src/filesystem/Factory.h
        src/filesystem/Field.h
        src/filesystem/Structure.h
        src/utils/BitMap.cpp
        src/utils/BitMap.h
        src/utils/utils.h
        src/utils/strmode.h
        src/qt-utils/qtutils.h
        src/qt-utils/Operation.h
        src/qt-utils/MountableDevice.h
        src/contr/OperationStack.cpp
        src/contr/OperationStack.h
        src/contr/Controller.cpp
        src/contr/Controller.h
        src/contr/SegmentInfo.h
)

find_package(libassert CONFIG REQUIRED)

target_link_libraries(Shardine PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Svg
        hexedit
        libassert::assert
)

qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES es)
qt_add_translations(Shardine)

add_subdirectory(test)

target_compile_options(Shardine PRIVATE -Wall -Wextra -pedantic)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto=auto")
