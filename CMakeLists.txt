cmake_minimum_required(VERSION 3.16)

project(ExpenseTracker VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

add_executable(expense
    src/CLI_Interface.cpp
    src/DataHandler.cpp
    src/ExpenseManager.cpp
    src/main.cpp
)

target_include_directories(expense PRIVATE include)

message(STATUS "ExpenseTracker project scaffold configured")
