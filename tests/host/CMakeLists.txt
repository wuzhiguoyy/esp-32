cmake_minimum_required(VERSION 3.16)
project(daoxuan_host_tests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

set(FEATURES ${CMAKE_CURRENT_SOURCE_DIR}/../../main/features)
set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../../main)

add_executable(test_nmea test_nmea_parser.cpp ${FEATURES}/nmea_parser.cc)
target_include_directories(test_nmea PRIVATE ${INCLUDE})
add_test(NAME nmea_parser COMMAND test_nmea)

add_executable(test_weight test_weight_math.cpp ${FEATURES}/weight_math.cc)
target_include_directories(test_weight PRIVATE ${INCLUDE})
add_test(NAME weight_math COMMAND test_weight)

add_executable(test_door test_door_lock_logic.cpp ${FEATURES}/door_lock_logic.cc)
target_include_directories(test_door PRIVATE ${INCLUDE})
add_test(NAME door_lock_logic COMMAND test_door)

add_executable(test_location test_location_payload.cpp ${FEATURES}/location_payload.cc)
target_include_directories(test_location PRIVATE ${INCLUDE})
add_test(NAME location_payload COMMAND test_location)

add_executable(test_rc522 test_rc522.cpp ${FEATURES}/rc522.cc)
target_include_directories(test_rc522 PRIVATE ${INCLUDE})
add_test(NAME rc522 COMMAND test_rc522)
