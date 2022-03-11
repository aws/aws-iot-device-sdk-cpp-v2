import boto3
import uuid
import json
import os
import subprocess
import platform
from time import sleep

def delete_thing_with_certi(thingName, certiId, certiArn):
    client.detach_thing_principal(
        thingName = thingName,
        principal = certiArn)
    client.update_certificate(
    certificateId =certiId,
    newStatus ='INACTIVE')
    client.delete_certificate(certificateId = certiId, forceDelete = True)
    client.delete_thing(thingName = thingName)

# debug linux environment variables
print(os.environ)

client = boto3.client('iot')
dataClient = boto3.client('iot-data')
f = open('deviceadvisor/script/DATestConfig.json')
DATestConfig = json.load(f)
f.close()
certificate_path = 'certificate.pem.crt' # = os.path.join(env.install_dir, 'certificate.pem.crt')
key_path = 'private.pem.key' # = os.path.join(env.install_dir,'private.pem.key')
shadowProperty = os.environ['DA_SHADOW_PROPERTY']
shadowDefault = os.environ['DA_SHADOW_VALUE_DEFAULT']

##############################################
# create a test thing 
thing_name = "DATest_" + str(uuid.uuid4())
try:
    # create_thing_response:
    # {
    # 'thingName': 'string',
    # 'thingArn': 'string',
    # 'thingId': 'string'
    # }
    print("[Device Advisor]Info: Started to create thing...")
    create_thing_response = client.create_thing(
        thingName=thing_name
    )
    os.environ["DA_THING_NAME"] = thing_name
    
except Exception as e:
    print("[Device Advisor]Error: Failed to create thing: " + e)
    exit(-1)


##############################################
# create certificate and keys used for testing
try:
    print("[Device Advisor]Info: Started to create certificate...")
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
    
    # setup environment variable 
    os.environ["DA_CERTI"] = certificate_path
    os.environ["DA_KEY"] = key_path

except:
    client.delete_thing(thingName = thing_name)
    print("[Device Advisor]Error: Failed to create certificate.")
    exit(-1)

##############################################
# attach certification to thing
try:
    print("[Device Advisor]Info: Attach certificate to test thing...")
    # attache the certificate to thing
    client.attach_thing_principal(
        thingName = thing_name,
        principal = create_cert_response['certificateArn']
    )

    certificate_arn = create_cert_response['certificateArn']
    certificate_id = create_cert_response['certificateId']

except:
    delete_thing_with_certi(thing_name, certificate_id ,certificate_arn )
    print("[Device Advisor]Error: Failed to attach certificate.")
    exit(-1)


##############################################
# Run device advisor
deviceAdvisor = boto3.client('iotdeviceadvisor')
test_result = {}
for test_name in DATestConfig['tests']:
    try:
        # set default shadow
        payload_shadow = json.dumps(
        {
        "state": {
            "desired": {
                shadowProperty: shadowDefault
                },
            "reported": {
                shadowProperty: shadowDefault
                }
            }
        })
        shadow_response = dataClient.update_thing_shadow(
            thingName = thing_name,
            payload = payload_shadow)
        get_shadow_response = dataClient.get_thing_shadow(thingName = thing_name)
        # make sure shadow is created
        while(get_shadow_response is None): 
            get_shadow_response = dataClient.get_thing_shadow(thingName = thing_name)
        
        # start device advisor test
        # test_start_response
        # {
        # 'suiteRunId': 'string',
        # 'suiteRunArn': 'string',
        # 'createdAt': datetime(2015, 1, 1)
        # }
        print("[Device Advisor]Info: Start device advisor test: " + test_name)
        test_start_response = deviceAdvisor.start_suite_run(
        suiteDefinitionId=DATestConfig['test_suite_ids'][test_name],
        suiteRunConfiguration={
            'primaryDevice': {
                'thingArn': create_thing_response['thingArn'],
            },
            'parallelRun': True
        })

        # get DA endpoint
        endpoint_response = deviceAdvisor.get_endpoint(
            thingArn = create_thing_response['thingArn']
        )
        os.environ['DA_ENDPOINT'] = endpoint_response['endpoint']

        while True:
            # sleep for 0.01s every loop to avoid TooManyRequestsException
            sleep(0.05)
            test_result_responds = deviceAdvisor.get_suite_run(
                suiteDefinitionId=DATestConfig['test_suite_ids'][test_name],
                suiteRunId=test_start_response['suiteRunId']
            )
            if (test_result_responds['status'] == 'PENDING' or
            len(test_result_responds['testResult']['groups']) == 0 or # test group has not been loaded
            len(test_result_responds['testResult']['groups'][0]['tests']) == 0 or #test case has not been loaded
            test_result_responds['testResult']['groups'][0]['tests'][0]['status'] == 'PENDING'):
                continue
            elif (test_result_responds['status'] == 'RUNNING' and 
            test_result_responds['testResult']['groups'][0]['tests'][0]['status'] == 'RUNNING'):
                exe_path = os.path.join("build/deviceadvisor/tests/",DATestConfig['test_exe_path'][test_name])
                if platform.system() == 'Windows':
                    exe_path = os.path.join(exe_path, "RelWithDebInfo",DATestConfig['test_exe_path'][test_name])
                else:
                    exe_path = os.path.join(exe_path, DATestConfig['test_exe_path'][test_name])

                print("start running " + exe_path)
                result = subprocess.run(exe_path)
            elif (test_result_responds['status'] != 'RUNNING'):
                test_result[test_name] = test_result_responds['status']
                break
    except Exception as e:
        delete_thing_with_certi(thing_name, certificate_id ,certificate_arn )
        print("[Device Advisor]Error: Failed to test: " + e)
        exit(-1)

#cleanup things
delete_thing_with_certi(thing_name, certificate_id ,certificate_arn )
print(test_result)
