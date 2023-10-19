import boto3
import uuid
import os
import subprocess
import platform
import sys

# The script will create an IoT thing with a policy that allows it to run Device Defender, then runs the Device Defender sample
# Usage:
#   - python DDTestRun.py        : Test Device Defender with Mqtt3 Client
#   - python DDTestRun.py mqtt5  : Test Device Defender with Mqtt5 Client

# On something other than Linux? Pass the test instantly since Device Defender is only supported on Linux
if platform.system() != "Linux":
    print("[Device Defender]Info: Skipped Test - " + platform.system() +
          " not supported (Only Linux supported currently)")
    exit(0)

##############################################
# Cleanup Certificates and Things and created certificate and private key file
def delete_thing_with_certi(thingName, certiId, certiArn):
    client.detach_thing_principal(
        thingName=thingName,
        principal=certiArn)
    client.update_certificate(
        certificateId=certiId,
        newStatus='INACTIVE')
    client.delete_certificate(certificateId=certiId, forceDelete=True)
    client.delete_thing(thingName=thingName)

    print("[Device Defender]Info: Deleted thing with name: " + thingName)


##############################################
# Initialize variables
# create aws client
client = boto3.client('iot', region_name='us-east-1')
# create an temporary certificate/key file path
certificate_path = os.path.join(os.getcwd(), 'certificate.pem.crt')
key_path = os.path.join(os.getcwd(), 'private.pem.key')
# Other variables
metrics_added = []
thing_arn = None
client_made_thing = False
client_made_policy = False
use_mqtt5 = False
if len(sys.argv) > 1:
    use_mqtt5 = (sys.argv[1] == "mqtt5")
    print("Run Device Defender with Mqtt5 Client")

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
    thing_arn = create_thing_response["thingArn"]
    client_made_thing = True

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
    client.delete_thing(thingName=thing_name)
    print("[Device Defender]Error: Failed to create certificate.")
    exit(-1)

##############################################
# Create policy
try:
    print("[Device Defender]Info: Started to create policy...")
    # {
    # 'policyName': 'string',
    # 'policyArn': 'string',
    # 'policyDocument': 'string',
    # 'policyVersionId': 'string'
    # }

    # We only need a short section of the thing arn
    thing_arn_split = thing_arn.split(":")
    thing_arn_short = thing_arn_split[0] + ':' + thing_arn_split[1] + ':' + thing_arn_split[2] + ':' + thing_arn_split[3] + ":" + thing_arn_split[4]
    policy_document_json = (
        '{'
        '"Version": "2012-10-17",'
        '"Statement": ['
            '{'
            '"Effect": "Allow",'
            '"Action": ['
                '"iot:Publish",'
                '"iot:Subscribe",'
                '"iot:RetainPublish"'
            '],'
            f'"Resource": "{thing_arn_short}:*/$aws/things/*/defender/metrics/*"'
            '},'
            '{'
            '"Effect": "Allow",'
            '"Action": "iot:Connect",'
            f'"Resource": "{thing_arn_short}:client/*"'
            '}'
        ']'
        '}'
    )
    create_policy_response = client.create_policy(
        policyName=thing_name + "_policy",
        policyDocument=policy_document_json
    )
    client_made_policy = True
except Exception as e:
    if client_made_thing:
        client.delete_thing(thingName=thing_name)
    if client_made_policy:
        client.delete_policy(policyName=thing_name + "_policy")
    print("[Device Defender]Error: Failed to create policy.")
    exit(-1)

##############################################

##############################################
# attach certification to thing
certificate_id = None
certificate_arn = None
try:
    print("[Device Defender]Info: Attach policy to certificate...")
    # attach policy to thing
    client.attach_policy(
        policyName=thing_name + "_policy",
        target=create_cert_response["certificateArn"]
    )

    print("[Device Defender]Info: Attach certificate to test thing...")
    # attache the certificate to thing
    client.attach_thing_principal(
        thingName=thing_name,
        principal=create_cert_response['certificateArn']
    )

    certificate_arn = create_cert_response['certificateArn']
    certificate_id = create_cert_response['certificateId']

except:
    if certificate_id:
        delete_thing_with_certi(thing_name, certificate_id, certificate_arn)
    else:
        client.delete_thing(thingName=thing_name)

    if client_made_policy:
        client.delete_policy(policyName=thing_name + "_policy")

    print("[Device Defender]Error: Failed to attach certificate.")
    exit(-1)

##############################################
# Run device defender
try:

    # Get the Device Defender endpoint
    endpoint_response = client.describe_endpoint(
        endpointType='iot:Data-ATS')["endpointAddress"]

    print("[Device Defender]Info: Adding custom metrics...")
    metrics_to_add = [
        {"name": "CustonNumber", "display_name": "DD Custom Number", "type": "number"},
        {"name": "CustomNumberTwo",
            "display_name": "DD Custom Number 2", "type": "number"},
        {"name": "CustomNumberList",
            "display_name": "DD Custom Number List", "type": "number-list"},
        {"name": "CustomStringList",
            "display_name": "DD Custom String List", "type": "string-list"},
        {"name": "CustomIPList", "display_name": "DD Custom IP List",
            "type": "ip-address-list"},
        {"name": "cpu_usage", "display_name": "DD Cpu Usage", "type": "number"},
        {"name": "memory_usage", "display_name": "DD Memory Usage", "type": "number"},
        {"name": "process_count", "display_name": "DD Process count", "type": "number"}
    ]
    for current_metric in metrics_to_add:
        try:
            client.create_custom_metric(
                metricName=current_metric["name"],
                displayName=current_metric["display_name"],
                metricType=current_metric["type"],
                tags=[]
            )
            metrics_added.append(current_metric["name"])
            print("[Device Defender]Info: Metric with name: " +
                  current_metric["name"] + " added.")
        except:
            print("[Device Defender]Info: Metric with name: " + current_metric["name"] +
                  " already present. Skipping and will not delete...")
            continue

    print("[Device Defender]Info: Running sample (this should take ~60 seconds).")

    if use_mqtt5:
        # Run the sample:
        exe_path = "build/samples/device_defender/mqtt5_basic_report/"
        # If running locally, comment out the line above and uncomment the line below:
        #exe_path = "samples/device_defender/basic_report/build/"

        # Windows has a different build folder structure, but this ONLY runs on Linux currently so we do not need to worry about it
        exe_path = os.path.join(exe_path, "mqtt5-basic-report")
    else:
        # Run the sample:
        exe_path = "build/samples/device_defender/basic_report/"
        # If running locally, comment out the line above and uncomment the line below:
        #exe_path = "samples/device_defender/basic_report/build/"

        # Windows has a different build folder structure, but this ONLY runs on Linux currently so we do not need to worry about it
        exe_path = os.path.join(exe_path, "basic-report")

    print("[Device Defender]Info: Start to run: " + exe_path)
    # The Device Defender sample will take ~1 minute to run even if successful
    # (since samples are sent every minute)
    arguments = [exe_path, "--endpoint", endpoint_response, "--cert",
                 certificate_path, "--key", key_path, "--thing_name", thing_name, "--count", "2"]
    result = subprocess.run(arguments, timeout=60*2, check=True)
    print("[Device Defender]Info: Sample finished running.")

    # There does not appear to be any way to get the metrics from the device - so we'll assume that if it didn't return -1, then it worked

    # delete custom metrics we added
    for metric_name in metrics_added:
        client.delete_custom_metric(metricName=metric_name)

    # Delete
    delete_thing_with_certi(thing_name, certificate_id, certificate_arn)
    client.delete_policy(policyName=thing_name + "_policy")

except Exception as e:
    # delete custom metrics we added
    for metric_name in metrics_added:
        client.delete_custom_metric(metricName=metric_name)

    if client_made_thing:
        delete_thing_with_certi(thing_name, certificate_id, certificate_arn)
    if client_made_policy:
        client.delete_policy(policyName=thing_name + "_policy")

    print("[Device Defender]Error: Failed to test: Basic Report")
    exit(-1)

print("[Device Defender]Info: Basic Report sample test passed")
exit(0)
