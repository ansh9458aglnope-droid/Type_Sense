cmake_minimum_required(VERSION 3.10)
project(TypeSense LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 Widgets and SQL
find_package(Qt6 COMPONENTS Widgets Sql REQUIRED)
add_executable(TypeSense
    src/main.cpp
    src/MainWindow.cpp
    src/TypingEngine.cpp
)

target_include_directories(TypeSense PRIVATE src)

# Link Qt6 Widgets
target_link_libraries(TypeSense Qt6::Widgets Qt6::Sql)
