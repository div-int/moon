# CMakeList.txt : CMake project for moon, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project (moon)
# Add source to this project's executable.
add_executable ("${PROJECT_NAME}" "src/olcPixelGameEngine.h" "src/bus.h" "src/bus.cpp" "src/w65c816s.h" "src/w65c816s.cpp"  "src/ram.h" "src/ram.cpp" "src/rom.h" "src/rom.cpp" "src/moon.cpp" "src/moon.h")

# TODO: Add tests and install targets if needed.
