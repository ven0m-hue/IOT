# IOT
Internet of things with the ESP8266 SoC. 
All of these have been implemented using Arduino IDE (not very proud of it :\)

1. OTA (Over the air) internet via AWS web hosting.
2. --> This firmware(the code) to be updated is hosted over a server, aws in this case.
--> Checks for the frimware update every specified unit of time. 
--> However there is no check for a secure transmission of the data through the server. I'm talking about the security certificates (This has not been implemented). 
 

2. Use cases of MQTT protocol. 
--> MQTT protocol using AWS
--> MQTT protocol using ThingSpeak, however I haven't tried enough to get it right all the time. Loss of data and stuff while transmitting the payload.

3. Serial Commuinication using UDP protocol. 
--> Server-Client based one to one and one to many using UDP protocol.
