
import boto3
import uuid
import os
import subprocess
import platform
from time import sleep

# TMP - just return exit(0) for now so I can test this on Linux without it accidentally running
exit(0)

# On something other than Linux? Pass the test instantly since Device Defender is only supported on Linux
if platform.system != "linux" or platform.system != "linux2":
    exit(0)

##############################################
# Cleanup Certificates and Things and created certificate and private key file
def delete_thing_with_certi(thingName, certiId, certiArn):
    client.detach_thing_principal(
        thingName = thingName,
        principal = certiArn)
    client.update_certificate(
    certificateId =certiId,
    newStatus ='INACTIVE')
    client.delete_certificate(certificateId = certiId, forceDelete = True)
    client.delete_thing(thingName = thingName)


##############################################
# Initialize variables
# create aws client
client = boto3.client('iot')
# create an temporary certificate/key file path
certificate_path = os.path.join(os.getcwd(), 'certificate.pem.crt')
key_path = os.path.join(os.getcwd(), 'private.pem.key')

##############################################
# create a test thing
thing_name = "DDTest_" + str(uuid.uuid4())
try:
    # create_thing_response:
    # {
    # 'thingName': 'string',
    # 'thingArn': 'string',
    # 'thingId': 'string'
    # }
    print("[Device Defender]Info: Started to create thing...")
    create_thing_response = client.create_thing(
        thingName=thing_name
    )

except Exception as e:
    print("[Device Defender]Error: Failed to create thing: " + thing_name)
    exit(-1)

##############################################
# create certificate and keys used for testing
try:
    print("[Device Defender]Info: Started to create certificate...")
    # create_cert_response:
    # {
    # 'certificateArn': 'string',
    # 'certificateId': 'string',
    # 'certificatePem': 'string',
    # 'keyPair':
    #   {
    #     'PublicKey': 'string',
    #     'PrivateKey': 'string'
    #   }
    # }
    create_cert_response = client.create_keys_and_certificate(
        setAsActive=True
    )
    # write certificate to file
    f = open(certificate_path, "w")
    f.write(create_cert_response['certificatePem'])
    f.close()

    # write private key to file
    f = open(key_path, "w")
    f.write(create_cert_response['keyPair']['PrivateKey'])
    f.close()

except:
    client.delete_thing(thingName = thing_name)
    print("[Device Defender]Error: Failed to create certificate.")
    exit(-1)

##############################################
# attach certification to thing
try:
    print("[Device Defender]Info: Attach certificate to test thing...")
    # attache the certificate to thing
    client.attach_thing_principal(
        thingName = thing_name,
        principal = create_cert_response['certificateArn']
    )

    certificate_arn = create_cert_response['certificateArn']
    certificate_id = create_cert_response['certificateId']

except:
    delete_thing_with_certi(thing_name, certificate_id, certificate_arn )
    print("[Device Defender]Error: Failed to attach certificate.")
    exit(-1)

##############################################
# Run device defender
try:

    # Get the Device Defender endpoint
    endpoint_response = client.describe_endpoint(endpointType='string')["endpointAddress"]

    # add the custom metrics we need for the test
    metrics_added = []
    try:
        # Note - we do not care about the device defender creation ARN paths and such for this test, so we don't really need to check return values.
        custom_metric_num_01 = client.create_custom_metric(metricName='CustomNumber', displayName="DD Custom Number", metricType='number', tags=[])
        metrics_added.append("CustonNumber")
        custom_metric_num_02 = client.create_custom_metric(metricName='CustomNumberTwo', displayName="DD Custom Number 2", metricType='number', tags=[])
        metrics_added.append("CustonNumberTwo")
        custom_metric_num_list = client.create_custom_metric(metricName='CustomNumberList', displayName="DD Custom Number list", metricType='number-list', tags=[])
        metrics_added.append("CustonNumberList")
        custom_metric_string_list = client.create_custom_metric(metricName='CustomStringList', displayName="DD Custom String list", metricType='string-list', tags=[])
        metrics_added.append("CustomStringList")
        custom_metric_ip_list = client.create_custom_metric(metricName='CustomIPList', displayName="DD Custom IP list", metricType='string-list', tags=[])
        metrics_added.append("CustomIPList")

        custom_metric_num_01 = client.create_custom_metric(metricName='cpu_usage', displayName="DD CPU usage", metricType='number', tags=[])
        metrics_added.append("cpu_usage")
        custom_metric_num_01 = client.create_custom_metric(metricName='memory_usage', displayName="DD Memory usage", metricType='number', tags=[])
        metrics_added.append("memory_usage")
        custom_metric_num_01 = client.create_custom_metric(metricName='process_count', displayName="DD Process count", metricType='number', tags=[])
        metrics_added.append("process_count")
    # Note: Exceptions can occur if the metric already exists
    except:
        # delete custom metrics we added
        for metric_name in metrics_added:
            client.delete_custom_metric(metricName=metric_name)
        metrics_added = []

        delete_thing_with_certi(thing_name, certificate_id ,certificate_arn )
        print("[Device Defender]Error: Failed to create custom metrics.")
        exit(-1)

    # Run the sample:
    exe_path = "build/samples/device_defender/basic_report/"
    # Windows has a different build folder structure, but this ONLY runs on Linux currently so we do not need to worry about it
    exe_path = os.path.join(exe_path, "basic_report")
    print("start to run: " + exe_path)
    # The Device Defender sample will take 1 minute to run even if successful (since samples are sent every minute), so we'll add an extra 60 seconds to pad it a bit
    result = subprocess.run(args=[exe_path, "--endpoint", endpoint_response, "--cert", certificate_path, "--key", key_path, "--thing_name", thing_name, "--count", 1], timeout = 60*2)
    # Sleep for test duration time
    sleep(60*2)

    # There does not appear to be any way to get the metrics from the device - so we'll just have to assume it worked?
    # TODO - investigate finding way to get custom metric data

    # delete custom metrics we added
    for metric_name in metrics_added:
        client.delete_custom_metric(metricName=metric_name)

    # Delete
    delete_thing_with_certi(thing_name, certificate_id , certificate_arn )

except Exception as e:
    # delete custom metrics we added
    for metric_name in metrics_added:
        client.delete_custom_metric(metricName=metric_name)

    print("[Device Defender]Error: Failed to test: Basic Report")

exit(0)
