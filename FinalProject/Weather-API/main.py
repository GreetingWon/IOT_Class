import sys
import paho.mqtt.client as mqtt
from datetime import datetime
import requests
import json

current_time = datetime.now()

apikey = "e513004b07ee1b8a0dbd36eb4ee70f96"
api = "http://api.openweathermap.org/data/2.5/weather?q={city}&APPID={key}"

k2c = lambda k: k - 273.15

url = api.format(city="Seoul", key=apikey)
res = requests.get(url)
data = json.loads(res.text)

print("도시 = ", data["name"])
print("날씨 = ", data["weather"][0]["description"])
print("현재 온도 = ", k2c(data["main"]["temp"]), "°C")
print("최저 기온 = ", k2c(data["main"]["temp_min"]), "°C")
print("최고 기온 = ", k2c(data["main"]["temp_max"]), "°C")
print("습도 = ", data["main"]["humidity"], "%")
print(" ")

server = ""

def on_connect(client, userdata, flags, rc):
    print("Connected with RC : " + str(rc))
    client.subscribe("#")

def on_message(client, userdata, msg):
    who, m = msg.payload.decode('UTF-8').split(" : ")
    if who != sys.argv[1]:
        print(msg.payload.decode('UTF-8'))

if len(sys.argv) <= 1:
    print("Usage : " + sys.argv[0] + " myID")

client = mqtt.Client()
client.connect(server, 1883, 60)
client.on_connect = on_connect
client.on_message = on_message

client.loop_start()

for line in sys.stdin:
    client.publish("", (str(sys.argv[1])+" : "+line.strip()).encode('UTF-8'))
