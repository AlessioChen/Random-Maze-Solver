cmake_minimum_required(VERSION 3.26)
project(maze_solver)

set(CMAKE_CXX_STANDARD 11)

# try also to compile and execute without: -fopenmp
# in Linux just use -fopenmp, without -lomp and -Xpreprocessor
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp  -lomp")

# the following 2 lines are needed on macOS. Check the position of headers in libraries (e.g. Macports vs. Homebrew)
include_directories(/opt/homebrew/opt/libomp/include)
link_directories(/opt/homebrew/opt/libomp/lib)

add_executable(maze_solver main.cpp)








