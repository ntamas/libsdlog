add_custom_target(build_tests)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS build_tests)

function(add_unity_test NAME)
    add_executable(test_${NAME} test_${NAME}.c)
    target_link_libraries(test_${NAME} PUBLIC sdlog unity)
    add_test(${NAME} test_${NAME})
    add_dependencies(build_tests test_${NAME})
endfunction()

add_unity_test(io)
add_unity_test(message_format)
add_unity_test(writer)
