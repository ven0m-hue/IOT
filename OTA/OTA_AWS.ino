  // INCLUDE LIBRARIES
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <ESP8266httpUpdate.h>
   
  
  // Host URL
  // This was using AWS
  // Make sure you host your files in the amazon s3 bucket.
  // Copy and replace the link with your public ip 
  #define url_version "http://34.253.222.40/version.txt"   // the bin file --> make sure your bin file has the ota skeleton in it so that in future it could be updated.
  #define url_bin "http://34.253.222.40/firmware.bin"     // the version file --> update the version file every time. 
  
  //Credentials
  const char* ssid = "********";           // ssid 
  const char* password = "********";      // password
  
  // Initialization
  HTTPClient http;
  const float version= 1.00;        // Make sure version no. is always greater than the previous version.
  unsigned long previous = 0;      
  const long interval = 60000;     // 1min sec, checks for update every 1min, change as per need.
  int value;
  
  //Routine calls
  void repeatedCall();   
  void FirmwareUpdate();
  
  void setup()
  {
    // All the usuals 
    Serial.begin(115200);
    
    Serial.println("Start");
    
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
                                       
    Serial.println("Connected to WiFi");  // Connected to ap
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    pinMode(LED_BUILTIN, OUTPUT);       //pin config, just for the demo sake.
    
  }
  
  
  void loop()
  {
    
    digitalWrite(LED_BUILTIN, HIGH);          // Led on/off routine    
    delay(100);                       
    digitalWrite(LED_BUILTIN, LOW);           // For every 100 milliseconds... the file which is going to be downloaded blinks every 1 second. (firmware.bin)
    delay(100);    
    
    Serial.println("Checking for new updates");     //  Check flag 
    
    delay(1000);
    repeatedCall();     // loop call the entire routine 
    
    // And that's it.      
  }


  // Functions 
  
  void repeatedCall()
   {
      unsigned long current = millis();
      if ((current - previous) >= interval)  // look for update after every interval second
      {
        
        previous = current;
        
        FirmwareUpdate();  // The downloading of the files from the host after the firmware check  is done using this task.
          
      }
   }


  void FirmwareUpdate()
  {
    http.begin(url_version);               // for secure, transmission. SSL certification is required. 
    
    delay(100);
    
    int httpCode = http.GET();            // Get the version info 
    
    delay(100);                                     
    
    String payload; // String
    
    
    if (httpCode == HTTP_CODE_OK)         // Check flag for  version received 
    {
      
      payload = http.getString();         // save received version
      Serial.println(payload);
     
    }
    
    else
    {
      Serial.print("error in downloading version file:");
      Serial.println(httpCode);
  
    }
    
    http.end();
    if (httpCode == HTTP_CODE_OK)        
    {
    if( version >= payload.toFloat()  )              // Check flag, this is where it compares the previous version and the current version if( payload.equals(version))
    {   
       Serial.println("Device on latest firmware version");    // if its equal do nothing 
    }
    
    else
    {
       Serial.println("New firmware detected");    // else Get the frimware 
       Serial.println(payload.toFloat());
       WiFiClient client;
  
       ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);   // This blinks untill the code gets uploaded #notRequired tho', useful to note that uploading has initiated when the led flickers
  
      //," ","5B:FB:D1:D4:49:D3:0F:A9:C6:40:03:34:BA:E0:24:05:AA:D2:E2:01"); 
      t_httpUpdate_return ret = ESPhttpUpdate.update(url_bin);      // returns the binary of the firmware 
      
      
      switch (ret) {
        case HTTP_UPDATE_FAILED:
          Serial.printf("HTTP request failed (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str()); // error flag
          break;
  
        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("No updates");
          break;
  
        case HTTP_UPDATE_OK:
          Serial.println("HTTP_UPDATE_OK");
          break;
      } 
    }
   }  
  }
