project(adapt-exact-system-sin-L2)

add_executable(${PROJECT_NAME} main.cpp)
include (../../CMake.common)

set(BIN ${PROJECT_BINARY_DIR}/${PROJECT_NAME})
add_test(adapt-exact-system-sin-L2 ${BIN})
set_tests_properties(adapt-exact-system-sin-L2 PROPERTIES COST -5.8)

target_link_libraries(${PROJECT_NAME} ${HERMES_BIN})
