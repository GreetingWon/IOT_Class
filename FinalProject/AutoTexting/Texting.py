import sys

from sdk.api.message import Message
from sdk.exceptions import CoolsmsException

if __name__ == "__main__":

    # set api key, api secret
    api_key = "NCSLVHZHKVPPAPAZ"
    api_secret = "KZS403Q9ZTDZYRIMHZ9V0CJFURCYH5CV"

    ## 4 params(to, from, type, text) are mandatory. must be filled
    params = dict()
    params['type'] = 'sms' # Message type ( sms, lms, mms, ata )
    params['to'] = '01094060235' # Recipients Number 
    params['from'] = '01094060235' # Sender number
    params['text'] = 'Help Me!' # Message

    cool = Message(api_key, api_secret)
    try:
        response = cool.send(params)
        print("Success Count : %s" % response['success_count'])
        print("Error Count : %s" % response['error_count'])
        print("Group ID : %s" % response['group_id'])
        
        if "error_list" in response:
            print("Error List : %s" % response['error_list'])

    except CoolsmsException as e:
        print("Error Code : %s" % e.code)
        print("Error Message : %s" % e.msg)

    sys.exit()

