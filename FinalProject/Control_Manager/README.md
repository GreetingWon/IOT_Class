# Control_Magager
=======================================================================================
# controlmanager.py  
- pi home 의 python 파일을 제어한다
- 라즈베리파이와 esp8266에서 publish 한 메세지를 중계하여 Texting.py와 weather.py 를 실행한다.
- pi/1 : emergency message request
- pi/2 : weather request
                  
# Texting.py      
- 비상상황시 문자를 보냄

# weather.py      
- Weather-API를 통해 받은 데이터를 esp8266의 무드등 제어를 위해 cloud/weather 로 publish 한다.
- cloud/weather  : 날씨정보 - data["weather"][0]["description"]    publish
