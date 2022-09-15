import os
import paho.mqtt.client as mqtt
import sys
from datetime import datetime
import requests
import json

topic = "pi/#"
server = "34.193.131.206" #EC2 address

def on_connect(client, userdata, flags, rc):
    print("Start Program...")
    client.subscribe(topic)

def on_message(client, userdata, msg):
    print(msg.payload)
    if(int(msg.payload) == 1):
        os.system("python3 /home/ubuntu/Control_Magager/Texting.py") #Texting.py dir 
    elif(int(msg.payload) == 2) :
        os.system("python3 /home/ubuntu/Control_Magager/weather.py") #weather.py dir

client =mqtt.Client()
client.connect(server, 1883, 60)
client.on_connect = on_connect
client.on_message = on_message
client.subscribe("pi/#")
client.loop_forever()
