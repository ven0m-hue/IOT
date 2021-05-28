#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "NODE_SERVER";
const char* pswd = "ESPESP8266";
//
IPAddress Client_IP(192, 168, 4, 2);
IPAddress Server_IP(192, 168, 4, 1); // By default
//
WiFiUDP Udp;
unsigned int localUdpPort = 51399;
unsigned int remoteUdpPort =51399;
char incomingPacket[255];
const char* replypacket = "Hey, found your packet :D";  // Acknowledgement

void setup()
{
  Serial.begin(115200);
  Serial.println("Setting up the Access point");  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, pswd);
  Serial.println();
  Serial.println("Starting SERVER");
  Serial.println("SARTING UDP...");
  
  Udp.begin(localUdpPort);
  Serial.printf("INFO IP_%s\n, UDP port_%d\n", WiFi.localIP().toString().c_str(), localUdpPort);
  Serial.println();
}

void loop()
{
 int packetSize = Udp.parsePacket();
 if(!packetSize)       // Routine to send data to the Client 
 {
    if(Serial.available()>0)
   {
    char temp; int i=0;
    Udp.beginPacket(Client_IP, remoteUdpPort);
    //String temp = Serial.readString();
    temp= Serial.read();
    if(temp != '\r' && temp!='\n'){
      out[i] = temp;
      i++;
    }   
    out[i] = '\0'; 
    //Serial.println(out);// check flag
    Udp.write(out);//(OutgoingPacket, tlen);
    Udp.endPacket();
   
   }
 } 
else{

  //Serial.printf("Recieved %d bytes from %s, port %d\n", packetSize, Client_IP.toString().c_str(), remoteUdpPort);
  int len = Udp.read(incomingPacket, 255);        
  if(len > 0)          //Routine to read recieved data from the Client
  {
    Serial.printf("%s", incomingPacket);  
    Serial.println();
  }

  Udp.beginPacket(Client_IP, remoteUdpPort); // acknowledgement back to the Client
  Udp.write(replypacket);
  Udp.endPacket();
  }
}
