import paho.mqtt.client as mqtt
import requests

topic = "evt/#"
server = "34.193.131.206"

def on_connect(client, userdata, flags, rc):
    print("Connected with RC : " + str(rc))
    client.subscribe(topic)

def on_message(client, userdata, msg):
    if msg.topic == topic.replace("#","temperature"):
        temp = float(msg.payload)
        r = requests.post(f'http://34.193.131.206:8086/write?db=sensorDB', data=f'environ,did=esp12 temp={temp}')
    elif msg.topic == topic.replace("#","humidity"):
        humi = float(msg.payload)
        r = requests.post(f'http://34.193.131.206:8086/write?db=sensorDB', data=f'environ,did=esp12 humi={humi}')

client = mqtt.Client()
client.connect(server, 1883, 60)
client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()
