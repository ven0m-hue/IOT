
  #include <PubSubClient.h>
  #include <ESP8266WiFi.h>
  #include <WiFiUdp.h>
  #include <ArduinoJson.h>
  
  char *ssid = "user-name";        // ssid
  char *pass = "*******";         // password
  
  
  
  const char*   server = "mqtt.thingspeak.com";             // MQTT broker
  // For subscribe
  const char*   MqttUsername = "usr-name";                       //If it's available
  const char*   Mqttpass = "api_key";    //MQTT API key                        
  const char*   ReadAPIKey = "insert-api-key";     //inset api key  
  unsigned long ReadChannelID = 1118504;             // Your  Read Channel ID
  // Publsih  1118504
  const char*   WriteAPIKey = "X4EYQ7Q54E5TBFTL";
  unsigned long WriteChannelID = 1118504;            // Write Channel ID
  unsigned int  FieldNumber = 1;                    // Field being used
  
  unsigned long LastConnection = 0;
  const signed long interval = 2000; // 20sec
  //char* message = " ";
  int value;
  String MyTopic ;
  int counter;
  char data[50];
  // Function prototypes
  void MqttConnect();
  void getID(char clientID[], int len);
  
  int MqttSubscribeFields(long SubchannelID, int field, const char* readkey);
  
  void MqttPublishField( int FieldNumber, char* data, unsigned long PubchannelID, const char* writeKey);
  
  //
 
  
  void callback(char* topic, byte* payload, unsigned int length) // For recieving and handelling the payload
  {
   Serial.print("Message arrived [" + MyTopic + "]" );
   //Serial.print(MyTopic);
   //Serial.print("]");
   
  char* message = (char*) malloc(sizeof(char) * 30);
  
  for(int i=0;i<length; i++)
  {
     message[i] = payload[i];
     Serial.print((char)payload[i]);  
  }
  
  Serial.println(message);

  StaticJsonBuffer<300> JSONBuffer;  // creating a memory pool
  JsonObject& parsed = JSONBuffer.parseObject(message);

  if(!parsed.success())
  {
   
    Serial.println( value);  
    Serial.println("Parsing failed");
    delay(5000);
    
    return;
  }
  
  char  ChannelID = parsed["channel_id"];  // Not neccesary
  Serial.println(ChannelID);
  char  EntryID = parsed["entry_id"];
  Serial.println(EntryID);
  String field1 = parsed["field1"];
  Serial.println(field1);
  value = field1.toInt();


  if(value == HIGH)
  {
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Light is ON" );
  delay(1000);
  }
  else if(value == LOW)
  {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Light is OFF" );
  delay(1000);
  }  
  } 

  WiFiClient  Client;
  PubSubClient MqttClient(Client);
  
  void setup() {
  
  Serial.begin(115200);
  
  // pin config
  pinMode(LED_BUILTIN, OUTPUT);   
  
  WiFi.mode(WIFI_STA);  
  
  WiFi.begin(ssid, pass);
  
  while(WiFi.status() != WL_CONNECTED)
  {
   Serial.print("*");
   delay(5000);     
  }
  
  Serial.println("\n Connected to WiFi");
  
  MqttClient.setServer(server, 1883);    // 1883 is a standard MQTT port no.  8883 for secure connection 
  
  MqttClient.setCallback(callback);
  
  }
  
  void loop() {
  
  if(WiFi.status() != WL_CONNECTED)
  { 
  Serial.print("Attempting to Connect to SSID: ");
  WiFi.begin(ssid, pass);
  }
  
  if(!MqttClient.connected())
  
  {
      MqttConnect();        // Connects each time if the MqttConnection is lost
   
      if(MqttSubscribeFields(ReadChannelID, FieldNumber, ReadAPIKey)== 1)  // establisin the connection 
      {
        MqttSubscribeFields(ReadChannelID, FieldNumber, ReadAPIKey);
        Serial.println("Subscribed");       // Check flag, for confirmation of the subscription
       // callback(topic, payload, length);
        
      }    
  } 
  
    MqttClient.loop();   // calls the loop continously to establish the MqttConnection
   
    if(millis() - LastConnection)
     {
      
      counter ++;
      snprintf(data, 75, "[%1d] HOW YOU DOING ", counter);
      Serial.print(data);
      LastConnection = interval;
      MqttPublishField( FieldNumber, data, WriteChannelID, WriteAPIKey);
      
     }
     
  }
  
  
  void MqttConnect()
  {
  // yada yada  
  // This function establishes the MQTT connection 
  
  while(!MqttClient.connected())
  {
  char clientID[9];  
  getID(clientID, 8);  
  Serial.print("Attempting for connection");
  if (MqttClient.connect(clientID, MqttUsername, Mqttpass))
  {
    Serial.println("Connection established with Client ID:" + String(clientID) + "User" + String(MqttUsername) + "Password" + String(Mqttpass)); 
  }
  
  else
  {
  Serial.print("Failed to connect, rc = "); 
  Serial.println(MqttClient.state());
  Serial.println("Trying again in 5 sec");
  delay(5000);
  }
  }
  }
  void getID(char clientID[], int len)
  {
    
  char alphanum[] = "0123456789" "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz";
  for (int i=0; i<len; i++) 
  {
  clientID[i] = alphanum[random(60)];
  }
  clientID[len] = '\0';
  }
  
  
  int MqttSubscribeFields(long SubchannelID, int field, const char* readkey)  // For subscribing to the field, setting the subscription 
  {   
    
  String MyTopic;
    // Inorder to subscribe to the whole channel, put the field number 0, because there is no feild number with 0.
  /*  if(field == 0)
    {
    MyTopic = "channels/";
    MyTopic += String(SubchannelID);
    MyTopic += "/subscribe/json/";          // subscribing to the whole field
    MyTopic += String(readkey);   
    }
    else{
   */ // channels/1118504/subscribe/json 
    MyTopic = "channels/";
    MyTopic += String(SubchannelID);  
    MyTopic += "/subscribe/json/";  // subscribing to the specfic field 
   // MyTopic += String(field) + "/";
   // MyTopic += String(readkey);     

  Serial.println("Subscribing to" + MyTopic);
  Serial.println("State= " + MqttClient.state());
  
  return MqttClient.subscribe(MyTopic.c_str(),0);
  
  }
  
  // For Publsihing the data
  void MqttPublishField( int FieldNumber, char* data, unsigned long PubchannelID, const char* writeKey)
  {
  // to write to the thingspeak
    String TopicString= "channels/" + String(PubchannelID) + "/publish" + String(writeKey);
    int len = TopicString.length();
    const char *buf;
    buf = TopicString.c_str();
    MqttClient.publish(buf, data);
    
  }
  
  
  
