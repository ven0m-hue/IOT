#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "SSID";//"NODE_SERVER";
const char* pswd = "********";//"ESPESP8266";    // credentials to connect to server side AP
//
IPAddress Client_IP(192, 168, 4, 2);
IPAddress Server_IP(192, 168, 1, 100);  // Server IP, default IP is taken
//
WiFiUDP Udp;
unsigned int localUdpPort = 51399;   // port to listen and transmit
//unsigned int remoteUdpPort = 55486;//4210;
char incomingPacket[255];
char out[255];
const char* replypacket = "Hey, found your packet :D";  // Acknowledgement

void setup()
{
  Serial.begin(115200);
  Serial.println();  
  Serial.printf("connecting to %s", ssid);
  WiFi.begin(ssid, pswd);
  while(WiFi.status()!= WL_CONNECTED)
  {
    delay(500);
    Serial.print("*");  
  }
  Serial.println();
  Serial.println("connected");

  Udp.begin(localUdpPort);
  Serial.printf("INFO IP_%s\n, UDP port_%d\n", WiFi.localIP().toString().c_str(), localUdpPort);
  Serial.println();
}

void loop()
{
 int packetSize = Udp.parsePacket();
 if(!packetSize)       // Routine to send data to the server 
 {
    if(Serial.available()>0)
   {
    String temp; int i=0; 
    
    //String temp = Serial.readString();
    temp = Serial.readString();
    char output[temp.length()];
    strcpy(output, temp.c_str());
    if(output[0] == '*')
    Serial.println(output);
    /*{//if(temp[0] == '*'){     //temp != '\r' && temp!='\n'
      output[i] = temp[i];
      //i++;
     Serial.println(output);
    } */   
    //output[i] = '\0';
    //Serial.println(output);// check flag 
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());  
    Udp.write(output);//(OutgoingPacket, tlen);
    Udp.endPacket();
   } 
    
 } 
else{

  //Serial.printf("Recieved %d bytes from %s, port %d\n", packetSize, Server_IP.toString().c_str(), localUdpPort);
  int len = Udp.read(incomingPacket, 255);        
  if(len > 0)          //Routine to read recieved data from the server
  {
    //Serial.printf("UDP packet recieved :o %s", incomingPacket);  
    Serial.printf("%s", incomingPacket);  
    Serial.println();
  }

  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort()); // acknowledgement 
  Udp.write(replypacket);
  Udp.endPacket();
  }
}
