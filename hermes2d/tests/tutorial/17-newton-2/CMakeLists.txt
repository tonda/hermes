project(tutorial-17-newton-elliptic-2)

add_executable(${PROJECT_NAME} main.cpp)
include (../../../examples/CMake.common)

set(BIN ${PROJECT_BINARY_DIR}/${PROJECT_NAME})
add_test(tutorial-17-newton-elliptic-2 ${BIN})
