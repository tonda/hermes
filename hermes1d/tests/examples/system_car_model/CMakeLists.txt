project(example-system_car_model)

add_executable(${PROJECT_NAME} main.cpp)
include (../../CMake.common)

set(BIN ${PROJECT_BINARY_DIR}/${PROJECT_NAME})
add_test(example-system_car_model ${BIN})
set_tests_properties(example-system_car_model PROPERTIES WILL_FAIL TRUE)

target_link_libraries(${PROJECT_NAME} ${HERMES_BIN})
