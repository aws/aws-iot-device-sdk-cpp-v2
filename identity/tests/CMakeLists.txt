include(AwsTestHarness)
enable_testing()
include(CTest)

file(GLOB TEST_SRC "*.cpp")
file(GLOB TEST_HDRS "*.h")
file(GLOB TESTS ${TEST_HDRS} ${TEST_SRC})

set(TEST_BINARY_NAME ${PROJECT_NAME}-tests)

if(NOT BYO_CRYPTO)
    add_net_test_case(IdentityV2ClientCreateDestroy5)
    add_net_test_case(IdentityV2ClientCreateDestroy311)
    add_net_test_case(IdentityV2ClientProvisionWithCertAndKey5)
    add_net_test_case(IdentityV2ClientProvisionWithCertAndKey311)
    add_net_test_case(IdentityV2ClientProvisionWithCSR5)
    add_net_test_case(IdentityV2ClientProvisionWithCSR311)
endif()

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

