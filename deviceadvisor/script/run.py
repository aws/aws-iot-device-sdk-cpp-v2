import boto3
import uuid
import json
import os


def delete_thing_with_certi(thingName, certiId, certiArn):
    client.detach_thing_principal(
        thingName = thingName,
        principal = certiArn)
    client.update_certificate(
    certificateId =certiId,
    newStatus ='INACTIVE')
    client.delete_certificate(certificateId = certiId, forceDelete = True)
    client.delete_thing(thingName = thingName)

client = boto3.client('iot')
f = open('deviceadvisor/script/DATestConfig.json')
DATestConfig = json.load(f)
f.close()
certificate_path = os.path.join(".",'private.pem.key') # = os.path.join(env.install_dir, 'certificate.pem.crt')
key_path = os.path.join(".",'private.pem.key') # = os.path.join(env.install_dir,'private.pem.key')


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
except:
    print("[Device Advisor]Error: Failed to create thing.")
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
    # f = open(certificate_path, "w")
    # print(create_cert_response)
    # f.write(create_cert_response['certificatePem'])
    # f.close()

    # write private key to file
    # f = open(key_path, "w")
    # f.write(create_cert_response['keyPair']['PrivateKey'])
    # f.close()
    
    # setup environment variable 
    # os.environ["DA_CERTI"] = certificate_path
    # os.environ["DA_KEY"] = key_path

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
import boto3
deviceAdvisor = boto3.client('iotdeviceadvisor')

for test_name in DATestConfig['tests']:
    try:
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
        while True:
            test_result_responds = deviceAdvisor.get_suite_run(
                suiteDefinitionId=DATestConfig['test_suite_ids'][test_name],
                suiteRunId=test_start_response['suiteRunId']
            )

            print("test status:" + test_result_responds['status'])
            if (test_result_responds['status'] == 'PENDING' or
            len(test_result_responds['testResult']['groups']) == 0 or # test group has not been loaded
            len(test_result_responds['testResult']['groups'][0]['tests']) == 0 or #test case has not been loaded
            test_result_responds['testResult']['groups'][0]['tests'][0]['status'] == 'PENDING'):
                continue
            if (test_result_responds['status'] == 'RUNNING' and 
            test_result_responds['testResult']['groups'][0]['tests'][0]['status'] == 'RUNNING'):
                print("start running " + DATestConfig['test_ext_path'][test_name])
                result = subprocess.call(DATestConfig['test_ext_path'][test_name])
                print("running " + DATestConfig['test_ext_path'][test_name] + " result : " + result)
            else:
                break
        print("[Device Advisor]Info: Finish test :" + test_name)
        exit(0)
    except:
        delete_thing_with_certi(thing_name, certificate_id ,certificate_arn )
        print("[Device Advisor]Error: Failed to test")
        exit(-1)


