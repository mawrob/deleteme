
//
const int firstRunTest = 11221211221121122;

// struct to save created TS channel Id and keys
struct Settings {
  int channelId;
  char writeKey[17];
  char readKey[17];
  int testCheck;
  boolean firstRun;
};
Settings TSChanIdKeys;

LEDStatus blinkMagenta(RGB_COLOR_MAGENTA, LED_PATTERN_BLINK);

void setup() {
    // Check for first run
    // Is not first run if true
    EEPROM.get(0, TSChanIdKeys);
    if (firstRunTest == TSChanIdKeys.testCheck){
        // Not first run - do normal setup

    
   
    }
    else // Do first run setup
    {
        // Create a ThingSpeak channel the first time this code is run
        // assumes that Particle webhooks have been set up using the app

        // Get some data
        String TSChanSettings = "{\"n\":\"Delete me\",\"d\":\"Sentient Things set up test\"}";
        // Trigger the integration
        Particle.publish("TSCreateChannel", TSChanSettings, PRIVATE);    
        
        // Subscribe to the integration response event
        Particle.subscribe(System.deviceID() + "/hook-response/TSCreateChannel", saveTSIdKeys, MY_DEVICES);
	TSChanIdKeys.firstRun = true;
    }

}

void loop() {
  if (!TSChanIdKeys.firstRun){
    // {
    //   "write_api_key": "{{k}}",
    //   "time_format": "{{t}}",
    //   "updates": "{{d}}"
    // }
    // TIMESTAMP,FIELD1_VALUE,FIELD2_VALUE,FIELD3_VALUE,FIELD4_VALUE,FIELD5_VALUE,FIELD6_VALUE,FIELD7_VALUE,FIELD8_VALUE,LATITUDE,LONGITUDE,ELEVATION,STATUS |
    String channelData = "{\"c\":\"" + String(TSChanIdKeys.channelId) + "\"," + "\"k\":\"" + String(TSChanIdKeys.writeKey) + "\"," + "\"t\": \"absolute\"," + "\"d\":\"" + String(Time.now()) + "," + String(random(10)) +",,,,,,,,,,,\"}";
    Particle.publish("TSBulkWriteCSV", channelData);
    blinkMagenta.setActive(true);
    delay(3000);
    blinkMagenta.setActive(false);
    delay(57000);
  }
}



void saveTSIdKeys(const char *event, const char *data) {
    // Handle the integration response
    // The first time the device is run the TS channel Id and keys are
    // saved in EEPROM

	// {{id}}~{{#api_keys}}{{api_key}}~{{/api_keys}}
	String TSdata=String(data);

    int tildeIndexBegin = 0;
    int tildeIndexEnd = TSdata.indexOf('~');

    int n=0;
    String responseValues[3];
    while (tildeIndexEnd>0)
    {
        responseValues[n] = TSdata.substring(tildeIndexBegin, tildeIndexEnd);
        tildeIndexBegin = tildeIndexEnd + 1;
        tildeIndexEnd = TSdata.indexOf('~',tildeIndexBegin);
        n++;
    }
    TSChanIdKeys.channelId = responseValues[0].toInt();
    unsigned int len;
    len = responseValues[1].length()+1;
    responseValues[1].toCharArray(TSChanIdKeys.writeKey, len);
    responseValues[2].toCharArray(TSChanIdKeys.readKey, len);
    TSChanIdKeys.testCheck = firstRunTest;
    TSChanIdKeys.firstRun = false;
    // Save settings
    EEPROM.put(0, TSChanIdKeys);
    // Read back to check
    EEPROM.get(0, TSChanIdKeys);
    TSChanIdKeys.firstRun = false;
String message = String(TSChanIdKeys.channelId) + ":" + String(TSChanIdKeys.writeKey) + ":" + String(TSChanIdKeys.readKey);
    
Particle.publish("Channel created",message);
    
}
          
