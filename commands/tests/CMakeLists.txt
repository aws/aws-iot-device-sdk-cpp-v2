include(AwsTestHarness)
enable_testing()
include(CTest)

file(GLOB TEST_SRC "*.cpp")
file(GLOB TEST_HDRS "*.h")
file(GLOB TESTS ${TEST_HDRS} ${TEST_SRC})

set(TEST_BINARY_NAME ${PROJECT_NAME}-tests)

add_net_test_case(CommandsV2ClientCreateDestroy5)
add_net_test_case(CommandsV2ClientCreateDestroy311)

generate_cpp_test_driver(${TEST_BINARY_NAME})

aws_add_sanitizers(${TEST_BINARY_NAME})

# set extra warning flags
if(AWS_WARNINGS_ARE_ERRORS)
    if(MSVC)
        target_compile_options(${TEST_BINARY_NAME} PRIVATE /W4 /WX /wd4068)
    else()
        target_compile_options(${TEST_BINARY_NAME} PRIVATE -Wall -Wno-long-long -Werror)
    endif()
endif()
