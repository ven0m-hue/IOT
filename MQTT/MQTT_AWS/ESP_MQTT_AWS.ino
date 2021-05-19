
#include "FS.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>


#define ssid  "user_name"                // your ssid
#define password  "********"           // password

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org"); // real-time clock

const char* AWS_endpoint = "X-X-X-X-X-X-X-X-X-X-X-X-X"; //MQTT broker ip , IMPORTANT!  // Remeber to change this if you are using other account !!!

// init
long int values;
unsigned int len;
char * JsonFile;

//function that converts byte payload (string to int) to (long) integer value   
/*
long int btoint(byte * payload, unsigned int length) {
  char * demo = (char *) malloc(sizeof(char) * 30);
  for (int i = 0; i < length; i++) {
    demo[i] = payload[i];
  }
  long int  values = atol(demo);
  free(demo);
  return values;
  // pointer to a callback function when message arives. 
// That is, when we Publish from the aws account.
}*/

 
void callback(char* topic, byte* payload, unsigned int length) {
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  JsonFile = (char *) malloc(sizeof(char) * 30); 
  
  for (int i = 0; i < length; i++) 
  {
    JsonFile[i] = payload[i]; 
    Serial.print((char)payload[i]);
  }
  Serial.println(JsonFile);
  
  // Parsing the vlaues from the json file arrived stores in payload
 // DynamicJsonDocument doc(2048);   latest verion of JSON library
  StaticJsonBuffer<300> JSONBuffer;  //creating a memory pool
  JsonObject& parsed = JSONBuffer.parseObject(JsonFile);
  free(JsonFile);
  if(!parsed.success()){
    Serial.println("Parsing failed");
    delay(5000);
    return;
  }
  
  const char * Message = parsed["Message"];   // set key value in the json file as  " ". 
  int result = parsed["Result"];           //  For blinky code, to blink the no. result no.of times
  Serial.print("Result is \n");   
  Serial.println(result);
  
  for(int i=0 ; i<result; i++)      // length of the Subscribed data just to demonstrate subscription 
    {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);                            // wait for a second 
      digitalWrite(LED_BUILTIN, LOW);       // turn the LED on 
      delay(1000);                           // wait for a second 
      }
 // values = btoint(payload, length);  // Not sure why this doesn't work, require debugging! 
  Serial.println();
//  Serial.println(values);       // Sometimes it print corrupted values

}

WiFiClientSecure espClient;
PubSubClient client(AWS_endpoint, 8883, callback, espClient); //set MQTT port number to 8883 

// usd
long lastMsg = 0;
char msg[50];
int value = 0;

// yada yada yada..
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  espClient.setBufferSizes(512, 512);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  timeClient.begin();
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }

  espClient.setX509Time(timeClient.getEpochTime());

}

void reconnect() {
  
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESPthing")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "Topic-name");    // Topic to publish to aws is outTopic --> Topic-name
      // ... and resubscribe
      client.subscribe("inTopic");                 // Topic to subscribe from aws inTopic
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      char buf[256];
      espClient.getLastSSLError(buf, 256);
      Serial.print("WiFiClientSecure SSL error: ");
      Serial.println(buf);

      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  pinMode(LED_BUILTIN, OUTPUT);                     // initialize digital pin LED_BUILTIN as an output.
  setup_wifi();                                    // wifi setup
  delay(1000);
  
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

  Serial.print("Heap: "); 
  Serial.println(ESP.getFreeHeap());
   
  // Load certificate, private key file and the CA file previously downloaded from the AWS.
 // Before uploading the sketch, upload the certificates to the flash.
  
  //Load certificate 
  File cert = SPIFFS.open("/cert.der", "r");                                  //replace cert.crt eith your uploaded file name
  if (!cert) {
    Serial.println("Failed to open cert file");
  }
  else
    Serial.println("Success to open cert file");

  delay(1000);

  if (espClient.loadCertificate(cert))
    Serial.println("cert loaded");
  else
    Serial.println("cert not loaded");

  // Load private key file
  File private_key = SPIFFS.open("/private.der", "r");                          //replace private eith your uploaded file name
  if (!private_key) {
    Serial.println("Failed to open private cert file");
  }
  else
    Serial.println("Success to open private cert file");

  delay(1000);

  if (espClient.loadPrivateKey(private_key))
    Serial.println("private key loaded");
  else
    Serial.println("private key not loaded");

  // Load CA file
  File ca = SPIFFS.open("/ca.der", "r");                                        //replace ca eith your uploaded file name
  if (!ca) {
    Serial.println("Failed to open ca ");
  }
  else
    Serial.println("Success to open ca");

  delay(1000);

  if (espClient.loadCACert(ca))
    Serial.println("ca loaded");
  else
    Serial.println("ca failed");

  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());
                                                                      ///////////////////////////////////////////
  
}


// the user code goes here
void loop() {

  if (!client.connected())
  {
      reconnect();
  }
  
  client.loop();
  
  long now = millis();
  if (now - lastMsg > 20000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "{\"message\": \"TEST FLIGHT #%ld\"}", value);  // publishing to AWS looping it so that it updates every now and then.
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
    client.subscribe("inTopic");
    

    Serial.print("Heap: "); 
    Serial.println(ESP.getFreeHeap());        // low heap can cause problems
  }
  
}
