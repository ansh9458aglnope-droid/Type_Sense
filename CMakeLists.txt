cmake_minimum_required(VERSION 3.10)
project(TypeSense LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 Widgets, SQL, and SQLite3
find_package(Qt6 COMPONENTS Widgets Sql REQUIRED)

add_executable(TypeSense
    src/main.cpp
    src/MainWindow.cpp
    src/TypingEngine.cpp
    src/WebcamHandler.cpp
    src/database_manager.cpp
    src/c_ds/exercise_queue.c
    src/c_ds/correction_stack.c
)

target_include_directories(TypeSense PRIVATE src include)

# Link Qt6 Widgets, SQL and SQLite3
target_link_libraries(TypeSense Qt6::Widgets Qt6::Sql)

# ---------- DB Viewer ----------
add_executable(db_viewer src/db_viewer.cpp)
target_link_libraries(db_viewer Qt6::Core Qt6::Sql)