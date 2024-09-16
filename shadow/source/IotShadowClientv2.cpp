/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
* SPDX-License-Identifier: Apache-2.0.
*
* This file is generated
*/

#include <aws/iotshadow/IotShadowClientv2.h>

#include <aws/crt/UUID.h>
#include <aws/iotshadow/GetNamedShadowRequest.h>

namespace Aws
{
    namespace Iotshadow
    {

        class Clientv2 : public IClientv2 {
          public:

            Clientv2(Aws::Crt::Allocator *allocator, Aws::Iot::RequestResponse::IMqttRequestResponseClient *bindingClient);
            virtual ~Clientv2();

            virtual bool GetNamedShadow(const Aws::Iotshadow::GetNamedShadowRequest &request, std::function<void(GetNamedShadowResult &&)> &&handler);

          private:

            Aws::Crt::ScopedResource<Aws::Iot::RequestResponse::IMqttRequestResponseClient> m_bindingClient;
        };

        Clientv2::Clientv2(Aws::Crt::Allocator *allocator, Aws::Iot::RequestResponse::IMqttRequestResponseClient *bindingClient) :
            m_bindingClient(bindingClient, [allocator](Aws::Iot::RequestResponse::IMqttRequestResponseClient *bindingClient) { Aws::Crt::Delete(bindingClient, allocator); })
        {
        }

        Clientv2::~Clientv2() {
        }

        bool Clientv2::GetNamedShadow(const Aws::Iotshadow::GetNamedShadowRequest &request, const std::function<void(GetNamedShadowResult &&)> &handler) {
            Aws::Crt::StringStream publishTopic;
            publishTopic << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName << "/get";

            Aws::Crt::StringStream subscriptionTopic;
            subscriptionTopic << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName << "/get/+";

            struct aws_byte_cursor subscriptionTopicFilters[1] = {
                Aws::Crt::ByteCursorFromString(subscriptionTopic.str())
            };

            Aws::Crt::StringStream responsePathTopic1;
            responsePathTopic1 << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName << "/get/accepted";

            Aws::Crt::StringStream responsePathTopic2;
            responsePathTopic2 << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName << "/get/rejected";

            struct aws_mqtt_request_operation_response_path responsePaths[2];
            responsePaths[0].topic = Aws::Crt::ByteCursorFromString(responsePathTopic1.str());
            responsePaths[0].correlation_token_json_path = Aws::Crt::ByteCursorFromCString("clientToken");
            responsePaths[1].topic = Aws::Crt::ByteCursorFromString(responsePathTopic2.str());
            responsePaths[1].correlation_token_json_path = Aws::Crt::ByteCursorFromCString("clientToken");

            auto uuid = Aws::Crt::UUID().ToString();

            Aws::Crt::JsonObject jsonObject;
            request.SerializeToObject(jsonObject);
            Aws::Crt::String outgoingJson = jsonObject.View().WriteCompact(true);

            struct aws_mqtt_request_operation_options options;
            AWS_ZERO_STRUCT(options);
            options.subscription_topic_filters = subscriptionTopicFilters;
            options.subscription_topic_filter_count = 1;
            options.response_paths = responsePaths;
            options.response_path_count = 2;
            options.publish_topic = Aws::Crt::ByteCursorFromString(publishTopic.str());
            options.serialized_request = Aws::Crt::ByteCursorFromArray((uint8_t *)outgoingJson.data(), outgoingJson.length());
            options.correlation_token = Aws::Crt::ByteCursorFromString(uuid);

            auto resultHandler = [](Aws::Iot::RequestResponse::UnmodeledResult &&result){

            };

            int submitResult = m_bindingClient->submitRequest(options, std::move(resultHandler));

            return submitResult == AWS_OP_SUCCESS;
        }

        std::shared_ptr<IClientv2> IClientv2::newFrom5(const Aws::Crt::Mqtt5::Mqtt5Client &protocolClient,
                                   Aws::Iot::RequestResponse::RequestResponseClientOptions &&options,
                                   Aws::Crt::Allocator *allocator) {

            Aws::Iot::RequestResponse::IMqttRequestResponseClient *bindingClient = Aws::Iot::RequestResponse::IMqttRequestResponseClient::newFrom5(protocolClient, std::move(options), allocator);
            if (!bindingClient) {
                return nullptr;
            }

            return Aws::Crt::MakeShared<Clientv2>(allocator, allocator, bindingClient);
        }

        std::shared_ptr<IClientv2> IClientv2::newFrom311(const Aws::Crt::Mqtt::MqttConnection &protocolClient,
                                     Aws::Iot::RequestResponse::RequestResponseClientOptions &&options,
                                     Aws::Crt::Allocator *allocator) {

            Aws::Iot::RequestResponse::IMqttRequestResponseClient *bindingClient = Aws::Iot::RequestResponse::IMqttRequestResponseClient::newFrom311(protocolClient, std::move(options), allocator);
            if (!bindingClient) {
                return nullptr;
            }

            return Aws::Crt::MakeShared<Clientv2>(allocator, allocator, bindingClient);
        }

    }
}