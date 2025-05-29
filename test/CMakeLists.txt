project(ShardineTests LANGUAGES CXX)

find_package(Qt6 COMPONENTS
        Test
        REQUIRED
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

enable_testing(true)

add_executable(RunTests
        FieldTest.cpp
)

add_test(NAME ShardineTests COMMAND RunTests)

target_link_libraries(RunTests PRIVATE Qt::Test)
target_include_directories(RunTests PRIVATE ../src)
