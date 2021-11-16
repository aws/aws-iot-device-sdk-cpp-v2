/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <aws/crt/io/SocketOptions.h>
#include <aws/http/http.h>
#include <aws/iotdevice/private/secure_tunneling_impl.h>
#include <aws/iotdevice/private/serializer.h>
#include <aws/iotdevicecommon/IotDevice.h>
#include <aws/iotsecuretunneling/SecureTunnel.h>
#include <aws/testing/aws_test_harness.h>

using namespace std;
using namespace Aws::Crt::Io;
using namespace Aws::Iotdevicecommon;
using namespace Aws::Iotsecuretunneling;

#define INVALID_STREAM_ID 0
#define STREAM_ID 10
#define PAYLOAD "secure tunneling data payload"

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
    unique_ptr<DeviceApiHandle> deviceApiHandle;
    unique_ptr<EventLoopGroup> elGroup;
    unique_ptr<HostResolver> resolver;
    unique_ptr<ClientBootstrap> clientBootstrap;
    shared_ptr<SecureTunnel> secureTunnel;

    aws_secure_tunneling_local_proxy_mode localProxyMode;

    SecureTunnelingTestContext() { localProxyMode = AWS_SECURE_TUNNELING_DESTINATION_MODE; }
};
static SecureTunnelingTestContext s_testContext;

// Client callbacks implementation
static void s_OnConnectionComplete() {}
static void s_OnConnectionShutdown() {}

static void s_OnSendDataComplete(int errorCode) {}

static bool s_OnDataReceiveCorrectPayload = false;
static void s_OnDataReceive(const Aws::Crt::ByteBuf &data)
{
    s_OnDataReceiveCorrectPayload = aws_byte_buf_eq_c_str(&data, PAYLOAD);
}

static bool s_OnStreamStartCalled = false;
static void s_OnStreamStart()
{
    s_OnStreamStartCalled = true;
}

static bool s_OnStreamResetCalled = false;
static void s_OnStreamReset()
{
    s_OnStreamResetCalled = true;
}

static bool s_OnSessionResetCalled = false;
static void s_OnSessionReset()
{
    s_OnSessionResetCalled = true;
}

static int before(struct aws_allocator *allocator, void *ctx)
{
    auto *testContext = static_cast<SecureTunnelingTestContext *>(ctx);

    aws_http_library_init(allocator);

    testContext->deviceApiHandle = unique_ptr<DeviceApiHandle>(new DeviceApiHandle(allocator));
    testContext->elGroup = unique_ptr<EventLoopGroup>(new EventLoopGroup(1, allocator));
    testContext->resolver = unique_ptr<HostResolver>(new DefaultHostResolver(*testContext->elGroup, 8, 30, allocator));
    testContext->clientBootstrap =
        unique_ptr<ClientBootstrap>(new ClientBootstrap(*testContext->elGroup, *testContext->resolver, allocator));
    testContext->secureTunnel = SecureTunnelBuilder(
                                    allocator,
                                    *testContext->clientBootstrap,
                                    SocketOptions(),
                                    "access_token",
                                    testContext->localProxyMode,
                                    "endpoint")
                                    .WithRootCa("")
                                    .WithOnConnectionComplete(s_OnConnectionComplete)
                                    .WithOnConnectionShutdown(s_OnConnectionShutdown)
                                    .WithOnSendDataComplete(s_OnSendDataComplete)
                                    .WithOnDataReceive(s_OnDataReceive)
                                    .WithOnStreamStart(s_OnStreamStart)
                                    .WithOnStreamReset(s_OnStreamReset)
                                    .WithOnSessionReset(s_OnSessionReset)
                                    .Build();
    return AWS_ERROR_SUCCESS;
}

static int after(struct aws_allocator *allocator, int setup_result, void *ctx)
{
    auto *testContext = static_cast<SecureTunnelingTestContext *>(ctx);

    testContext->secureTunnel.reset();
    testContext->clientBootstrap.reset();
    testContext->resolver.reset();
    testContext->elGroup.reset();
    testContext->deviceApiHandle.reset();

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

AWS_TEST_CASE_FIXTURE(
    SecureTunnelingHandleDataReceiveTest,
    before,
    s_SecureTunnelingHandleDataReceiveTest,
    after,
    &s_testContext);
static int s_SecureTunnelingHandleDataReceiveTest(Aws::Crt::Allocator *allocator, void *ctx)
{
    auto *testContext = static_cast<SecureTunnelingTestContext *>(ctx);

    /* Send StreamStart first */
    struct aws_iot_st_msg st_msg;
    AWS_ZERO_STRUCT(st_msg);
    st_msg.type = STREAM_START;
    st_msg.stream_id = STREAM_ID;
    s_send_secure_tunneling_frame_to_websocket(&st_msg, allocator, testContext->secureTunnel->GetUnderlyingHandle());

    /* Send data */
    AWS_ZERO_STRUCT(st_msg);
    st_msg.type = DATA;
    st_msg.stream_id = STREAM_ID;
    st_msg.payload = aws_byte_buf_from_c_str(PAYLOAD);
    s_OnDataReceiveCorrectPayload = false;
    s_send_secure_tunneling_frame_to_websocket(&st_msg, allocator, testContext->secureTunnel->GetUnderlyingHandle());

    ASSERT_TRUE(s_OnDataReceiveCorrectPayload);
    ASSERT_INT_EQUALS(STREAM_ID, testContext->secureTunnel->GetUnderlyingHandle()->stream_id);
    ASSERT_UINT_EQUALS(0, testContext->secureTunnel->GetUnderlyingHandle()->received_data.len);

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE_FIXTURE(
    SecureTunnelingHandleStreamResetTest,
    before,
    SecureTunnelingHandleStreamResetTest,
    after,
    &s_testContext);
static int SecureTunnelingHandleStreamResetTest(Aws::Crt::Allocator *allocator, void *ctx)
{
    auto *testContext = static_cast<SecureTunnelingTestContext *>(ctx);

    /* Send StreamStart first */
    struct aws_iot_st_msg st_msg;
    AWS_ZERO_STRUCT(st_msg);
    st_msg.type = STREAM_START;
    st_msg.stream_id = STREAM_ID;
    s_send_secure_tunneling_frame_to_websocket(&st_msg, allocator, testContext->secureTunnel->GetUnderlyingHandle());

    /* Send StreamReset */
    AWS_ZERO_STRUCT(st_msg);
    st_msg.type = STREAM_RESET;
    st_msg.stream_id = STREAM_ID;
    s_OnStreamResetCalled = false;
    s_send_secure_tunneling_frame_to_websocket(&st_msg, allocator, testContext->secureTunnel->GetUnderlyingHandle());

    ASSERT_TRUE(s_OnStreamResetCalled);
    ASSERT_INT_EQUALS(INVALID_STREAM_ID, testContext->secureTunnel->GetUnderlyingHandle()->stream_id);
    ASSERT_UINT_EQUALS(0, testContext->secureTunnel->GetUnderlyingHandle()->received_data.len);

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE_FIXTURE(
    SecureTunnelingHandleSessionResetTest,
    before,
    s_SecureTunnelingHandleSessionResetTest,
    after,
    &s_testContext);
static int s_SecureTunnelingHandleSessionResetTest(struct aws_allocator *allocator, void *ctx)
{
    auto *testContext = static_cast<SecureTunnelingTestContext *>(ctx);

    /* Send StreamStart first */
    struct aws_iot_st_msg st_msg;
    AWS_ZERO_STRUCT(st_msg);
    st_msg.type = STREAM_START;
    st_msg.stream_id = STREAM_ID;
    s_send_secure_tunneling_frame_to_websocket(&st_msg, allocator, testContext->secureTunnel->GetUnderlyingHandle());

    /* Send StreamReset */
    AWS_ZERO_STRUCT(st_msg);
    st_msg.type = SESSION_RESET;
    st_msg.stream_id = STREAM_ID;
    s_OnSessionResetCalled = false;
    s_send_secure_tunneling_frame_to_websocket(&st_msg, allocator, testContext->secureTunnel->GetUnderlyingHandle());

    ASSERT_TRUE(s_OnSessionResetCalled);
    ASSERT_INT_EQUALS(INVALID_STREAM_ID, testContext->secureTunnel->GetUnderlyingHandle()->stream_id);
    ASSERT_UINT_EQUALS(0, testContext->secureTunnel->GetUnderlyingHandle()->received_data.len);

    return AWS_OP_SUCCESS;
}
