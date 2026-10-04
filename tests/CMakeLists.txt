add_executable(gtanet_tests protocol_tests.cpp)
target_include_directories(gtanet_tests PRIVATE ${CMAKE_SOURCE_DIR})
add_test(NAME gtanet_tests COMMAND gtanet_tests)
