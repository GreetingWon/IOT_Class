import paho.mqtt.client as mqtt
topic = "deviceid/2016146033/evt/light"
server = "3.90.5.110"

def on_connect(client, userdata, flags, rc):
    print("Connected with RC : " + str(rc))
    client.subscribe(topic)

def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload.decode('UTF-8')))
    Save_Message = int(msg.payload)

    if(Save_Message >= 100):{
        client.publish("deviceid/2016146033/cmd/lamp","1")
    }
    else:{
        client.publish("deviceid/2016146033/cmd/lamp","0")
    }
    

client = mqtt.Client()
client.connect(server, 1883, 60)
client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()
