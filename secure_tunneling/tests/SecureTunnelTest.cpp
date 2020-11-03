/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/http/http.h>
#include <aws/iotdevice/private/serializer.h>
#include <aws/iotsecuretunneling/SecureTunnel.h>
#include <aws/testing/aws_test_harness.h>

#define STREAM_ID 10

extern "C"
{
    struct aws_websocket_incoming_frame;
    extern bool on_websocket_incoming_frame_payload(
        struct aws_websocket *websocket,
        const struct aws_websocket_incoming_frame *frame,
        struct aws_byte_cursor data,
        void *user_data);
}

struct SecureTunnelingTestContext
{
    Aws::Iotsecuretunneling::DeviceApiHandle *deviceApiHandle;
    aws_secure_tunneling_local_proxy_mode localProxyMode;
    Aws::Iotsecuretunneling::SecureTunnel *secureTunnel;

    SecureTunnelingTestContext()
    {
        deviceApiHandle = nullptr;
        localProxyMode = AWS_SECURE_TUNNELING_DESTINATION_MODE;
        secureTunnel = nullptr;
    }
};
static SecureTunnelingTestContext s_testContext;

// Client callbacks implementation
static void s_OnConnectionComplete() {}

static void s_OnSendDataComplete(int errorCode) {}

static void s_OnDataReceive(const Aws::Crt::ByteBuf &data) {}

static bool s_OnStreamStartCalled = false;
static void s_OnStreamStart()
{
    s_OnStreamStartCalled = true;
}

static void s_OnStreamReset() {}

static void s_OnSessionReset() {}

static int before(struct aws_allocator *allocator, void *ctx)
{
    auto *testContext = static_cast<SecureTunnelingTestContext *>(ctx);

    aws_http_library_init(allocator);
    testContext->deviceApiHandle = new Aws::Iotsecuretunneling::DeviceApiHandle(allocator);

    testContext->secureTunnel = new Aws::Iotsecuretunneling::SecureTunnel(
        allocator,
        nullptr,
        nullptr,
        "access_token",
        testContext->localProxyMode,
        "endpoint",
        s_OnConnectionComplete,
        s_OnSendDataComplete,
        s_OnDataReceive,
        s_OnStreamStart,
        s_OnStreamReset,
        s_OnSessionReset);

    return AWS_ERROR_SUCCESS;
}

static int after(struct aws_allocator *allocator, int setup_result, void *ctx)
{
    auto *testContext = static_cast<SecureTunnelingTestContext *>(ctx);

    delete testContext->secureTunnel;
    testContext->secureTunnel = nullptr;

    delete testContext->deviceApiHandle;
    aws_http_library_clean_up();

    return AWS_ERROR_SUCCESS;
}

static void s_send_secure_tunneling_frame_to_websocket(
    const struct aws_iot_st_msg *st_msg,
    struct aws_allocator *allocator,
    struct aws_secure_tunnel *secure_tunnel)
{

    struct aws_byte_buf serialized_st_msg;
    aws_iot_st_msg_serialize_from_struct(&serialized_st_msg, allocator, *st_msg);

    /* Prepend 2 bytes length */
    struct aws_byte_buf websocket_frame;
    aws_byte_buf_init(&websocket_frame, allocator, serialized_st_msg.len + 2);
    aws_byte_buf_write_be16(&websocket_frame, (uint16_t)serialized_st_msg.len);
    struct aws_byte_cursor c = aws_byte_cursor_from_buf(&serialized_st_msg);
    aws_byte_buf_append(&websocket_frame, &c);
    c = aws_byte_cursor_from_buf(&websocket_frame);

    on_websocket_incoming_frame_payload(NULL, NULL, c, secure_tunnel);

    aws_byte_buf_clean_up(&serialized_st_msg);
    aws_byte_buf_clean_up(&websocket_frame);
}

AWS_TEST_CASE_FIXTURE(
    SecureTunnelingHandleStreamStartTest,
    before,
    s_SecureTunnelingHandleStreamStartTest,
    after,
    &s_testContext);
static int s_SecureTunnelingHandleStreamStartTest(Aws::Crt::Allocator *allocator, void *ctx)
{
    auto *testContext = static_cast<SecureTunnelingTestContext *>(ctx);

    struct aws_iot_st_msg st_msg;
    AWS_ZERO_STRUCT(st_msg);
    st_msg.type = STREAM_START;
    st_msg.stream_id = STREAM_ID;
    s_OnStreamStartCalled = false;
    s_send_secure_tunneling_frame_to_websocket(&st_msg, allocator, testContext->secureTunnel->GetUnderlyingHandle());

    ASSERT_TRUE(s_OnStreamStartCalled);
    ASSERT_INT_EQUALS(STREAM_ID, testContext->secureTunnel->GetUnderlyingHandle()->stream_id);
    ASSERT_UINT_EQUALS(0, testContext->secureTunnel->GetUnderlyingHandle()->received_data.len);

    return AWS_ERROR_SUCCESS;
}
