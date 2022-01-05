// Comment the below line out if compiling for Adrian
// Uncomment to compile for Don
// Will add the relevant extra pin and change the number of LEDS 
// as well as using the correct WiFi networks etc.

//#define ThisIsForDon true


#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h> // MQTT stuff
#include <EEPROM.h>

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
ESP8266WebServer HTTPserver(80);
std::string webPage = "";       // Used to store the "home" page with all the effect buttons on it
std::string settingsPage = "";  // Used to store the settings page
  
///////////////////////////////////////////////////////////////////////////
// MQTT and Domoticz Stuff
WiFiClient espClient;
PubSubClient MQTTclient(espClient);

const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";

#ifdef ThisIsForDon
  const char* mqttServer = "192.168.1.15";
  const char* mqttSubscription = "domoticz/out/ESP Devices/Fun Light";
  #define switchIDX 74   // The IDX of the device in Domoticz that we'll be acting upon/listening to
#else
  const char* mqttServer = "192.168.26.8";
  const char* mqttSubscription = "domoticz/out/ESP Devices/IKEA Light";
  #define switchIDX 147   // The IDX of the device in Domoticz that we'll be acting upon/listening to
#endif

///////////////////////////////////////////////////////////////////////////

// Telnet
WiFiServer TelnetServer(23);
WiFiClient Telnet;

void handleTelnet() {
  if (TelnetServer.hasClient()) {
    if (!Telnet || !Telnet.connected()) {
      if (Telnet) Telnet.stop();
      Telnet = TelnetServer.available();
    } else {
      TelnetServer.available().stop();
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////
// FastLED Stuff
#define FASTLED_ALLOW_INTERRUPTS 0


FASTLED_USING_NAMESPACE

     
#define DATA_PIN1            13
#define DATA_PIN2            12
#define DATA_PIN3            5
#define DATA_PIN4            4

#ifdef ThisIsForDon
  #define DATA_PIN5           16
  #define NUM_LEDS            144
  #define COLOR_ORDER         RGB
  #define NUM_STRIPS          5
#else
  #define NUM_LEDS            75
  #define COLOR_ORDER         GRB
  #define NUM_STRIPS          4
#endif
#define LED_TYPE            WS2812B

CRGB leds[NUM_STRIPS][NUM_LEDS];

int icurDomRed = 0;
int icurDomGreen = 0;
int icurDomBlue = 0;
int icurDomBrightness = 0;
int icurDomOffOn = 0;

const char    Effectnames[][20] = {"Domoticz", "Chase", "Rainbow", "Pacific", "Red/White/Blue", "Flames", "Rainbow Demo", "Balls", "Rain", "Juggle", "Fire Blue", "All Patterns"};
int           iNumEffects = (sizeof(Effectnames) / sizeof((Effectnames)[0]));

const char    VUEffectnames[][20] = {"Rainbow-1", "Rainbow-2", "Rainbow-3", "Classic-1", "Classic-2", "Classic-3", "Colour Change-1", "Colour Change-2", "Colour Change-3", "Shatter", "Stream", "Pulse", "All VUs"};
int           iNumVUEffects = (sizeof(VUEffectnames) / sizeof((VUEffectnames)[0]));


char          curEffect[20] = "Domoticz";
int           iCyclingEffectDuration = 15;   // When we choose "All" effects the effects will run in a cycle.  This is how long each effect will run for (in seconds) before moving to the next
int           iCurEffect = 1;                // The effect number we're currently on when cycling through all patterns
bool          doAllEffects = false;

int           iCurVUEffect = 0;                // The VU effect number we're currently on when cycling through all pattern
bool          doAllVUEffects = false;

// For bouncing balls effect
byte BallColors[3][3]={{0xff,0,0},{0,0xff,0},{0,0,0xff}};
int  NumBalls = sizeof(BallColors) / (sizeof(byte) * 3);
int  BallHeight = 2;  // The number of pixels wide that each ball will be.  Just an initial value.  The real value will be read from EEProm if it's been saved there
int  BallFadeToBlackBy = 4;

// For the rainbow effects
int iRainbowCounter = 0; // Used to cycle through the colour wheel - 256 colours on the wheel

// For Meteor Rain effect
int meteorLoopCount = 0;

// For VU Meter Effects
int INPUT_FLOOR=30;       //Lower range of analogRead input
int INPUT_CEILING=600;    //Max range of analogRead input, the lower the value the more sensitive (1023 = max)300 (150)
int SAMPLE_WINDOW=10;     // Sample window for average level
int FADE_TO_BLACK_BY=96;  // The speed to fade pixels to black in order to prevent flickering Vu displays.  Values 0-255.  Higher number = faster fade. Effectively a percentage e.g. fade to blac
int PEAK_HANG=35;         //Time of pause before peak dot falls
int PEAK_FALL=10;         //Rate of falling peak dot
int ROLLING_AVERAGE_COUNT = 4; // Number of samples to calculate the rolling average over
int PEAK_DOT_COLOUR=1;    //Colour of peak dot.  1 = In line with effect colour.  2 = Always red.
int DRAW_MAX=512;         //For vu8 (shatter)

typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, juggle };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0;
#define FRAMES_PER_SECOND  120
  

//////////////////////////////////////////////////////////////////////////////////
// Hash function used for converting effect names to an int so that we can do a
// case/switch statement against them (C doesn't support a case/switch statement
// against a string or char array

constexpr unsigned int hash(const char *s, int off = 0) {                        
    return !s[off] ? 5381 : (hash(s, off+1)*33) ^ s[off];                           
}   

//////////////////////////////////////////////////////////////////////////////////

     
void setup() {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  WiFi.mode(WIFI_STA);
  #ifdef ThisIsForDon
    wifiMulti.addAP("NETGEAR_2GEXT", "QmNpHcuvG4J4a9");
  
    IPAddress staticIP(192, 168, 1, 20);
    IPAddress gateway(192, 168, 1, 254);
    IPAddress subnet(255, 255, 255, 0);  
  #else  
    wifiMulti.addAP("BHOB", "poet-crawl-one");
    wifiMulti.addAP("EE-BrightBox-rxn5qx", "poet-crawl-one");
    wifiMulti.addAP("DAVE", "poet-crawl-one");
  
    IPAddress staticIP(192, 168, 26, 20);
    IPAddress gateway(192, 168, 26, 1);
    IPAddress subnet(255, 255, 255, 0);
  #endif

  Serial.println("V3.1");
  Serial.println("Connecting ...");
  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(250);
    Serial.print('.');
  }
  WiFi.config(staticIP, gateway, subnet);
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer
  
  ArduinoOTA.setHostname("FunLight");
  ArduinoOTA.setPassword("esp8266");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA ready");


    // Connect to MQTT and subscribe to the switch state queue
    MQTTclient.setServer(mqttServer, mqttPort);
    MQTTclient.setBufferSize(1024);  
    MQTTclient.setKeepAlive(90);
    MQTTclient.setCallback(MQTTCallback);


    // Only wait the same amount of time for a MQTT connection as we did for WiFi
    // If we don't get a godo connection then don't wait forever
    int WiFiWaitSoFar = 0;
    int MaxWiFiConnectWait = (10 * 1000); // 10s
    while ((!MQTTclient.connected()) && (WiFiWaitSoFar < MaxWiFiConnectWait)) {
      Serial.println("Connecting to MQTT...");
    
      if (MQTTclient.connect("LEDLightv1", mqttUser, mqttPassword )) {
        Serial.println("connected to MQTT");  
      } else {
        Serial.print("failed to connect to MQTT with state ");
        Serial.print(MQTTclient.state());
        WiFiWaitSoFar += 500;
        delay(500);
      }
    }

    if (MQTTclient.connected())
    {
      //MQTTclient.publish("domoticz/in", "{\"command\": \"udevice\", \"idx\":91, \"nvalue\":0, \"svalue\":\"LED Light v1 has just rebooted!\"}");
      Serial.printf("Subscribing to MQTT subscription: %s\n", mqttSubscription);
      MQTTclient.subscribe(mqttSubscription);   
      DomoticzGetDeviceInfo();
    }

    // Start Telnet Server
    TelnetServer.begin();
    TelnetServer.setNoDelay(true); 
    
    // Start HTTP Server
    BuildWebPage();
    HTTPserver.on("/", handleRoot);      //Which routine to handle at root location
    HTTPserver.on("/form", handleForm); 
    HTTPserver.on("/settings", handleSettings); 
    
    HTTPserver.begin();                  //Start server
    Serial.println("HTTP server started");

  // Use the on board EEProm to save some parameters
  // NOTE: Setting the "Erase Flash" option on the Arduino IDE Tools menu to "Sketch Only" will prevent
  // the settings being wiped out when re-programmed.
  // See comments in the EEPROM module for how the EEPROM locations are used
  EEPROM.begin(15); 
  EEProm_ReadParams();
  
    // FastLED
  //delay(3000); // 3 second delay for boot recovery, and a moment of silence
  pinMode(DATA_PIN1, OUTPUT);
  pinMode(DATA_PIN2, OUTPUT);
  pinMode(DATA_PIN3, OUTPUT);
  pinMode(DATA_PIN4, OUTPUT);
     
  FastLED.addLeds<LED_TYPE,DATA_PIN1,COLOR_ORDER>(leds[0], NUM_LEDS).setCorrection( TypicalLEDStrip );    
  FastLED.addLeds<LED_TYPE,DATA_PIN2,COLOR_ORDER>(leds[1], NUM_LEDS).setCorrection( TypicalLEDStrip );    
  FastLED.addLeds<LED_TYPE,DATA_PIN3,COLOR_ORDER>(leds[2], NUM_LEDS).setCorrection( TypicalLEDStrip );    
  FastLED.addLeds<LED_TYPE,DATA_PIN4,COLOR_ORDER>(leds[3], NUM_LEDS).setCorrection( TypicalLEDStrip );    
  
#ifdef ThisIsForDon
  pinMode(DATA_PIN5, OUTPUT);
  FastLED.addLeds<LED_TYPE,DATA_PIN5,COLOR_ORDER>(leds[4], NUM_LEDS).setCorrection( TypicalLEDStrip );    
#endif

  FastLED.setTemperature(WarmFluorescent);
  // Make sure everything is off before we start
  FastLED.setBrightness(0);  
  for(int q=0; q<NUM_STRIPS; q++)
  {
    fill_solid(leds[q], NUM_LEDS, CRGB::Black);
  }
  FastLED.show();
}


void loop() {
  ArduinoOTA.handle();

  // Check for MQTT messages
  // Double check that we're still connected 
  // Can become disconnected during some long running effects
  if (!MQTTclient.connected())
  {
    MQTTclient.connect("LEDLightv1", mqttUser, mqttPassword);  
  }
  MQTTclient.loop();

  // HTTP Stuff
  HTTPserver.handleClient();

  // Telnet Stuff
  handleTelnet(); 

  // Do any effect stuff
  // Most effects run continuously and the relevant routines must be repeatedly called
  // A couple of exceptions noteably the red/white/blue effect and Domoticz effects
  // Because these two are static effects in terms of the lights not changing
  // we set the light pattern as part of the HTTP request handling so there's no need
  // to do anything here

  // Let's see what effect we're supposed to be running
  // Use the hash function to convert the effect name string to an int so we can do the "switch" statement on it
  switch (hash(curEffect))
  {
    case hash("Rainbow"):
      rainbowCycle(20);
      break;
    case hash("Flames"):
      Fire(55,120,15);
      break;
    case hash("Pacific"):
      pacifica_loop();
      FastLED.show();
      delay(20);
      break;
    case hash("Chase"):
      sinelon();
      FastLED.show();  
      FastLED.delay(1000/FRAMES_PER_SECOND); 
    
      EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
      break;
    case hash("Rain"):
      //meteorRain(0xff,0xff,0xff,10, 64, true, 20);
      EVERY_N_MILLISECONDS( 20 ) { meteorRain(0xff,0xff,0xff,10, 64, true); }
      break;
    case hash("Balls"):
      BouncingColoredBallsLoop(NumBalls,BallColors, BallHeight);
      break;
    case hash("Strobe"):
      doStrobe(50);
    case hash("Rainbow Demo"):
      rainbow();
      FastLED.show();  
      FastLED.delay(1000/FRAMES_PER_SECOND); 
    
      EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
      break;
    case hash("Juggle"):
      juggle();
      FastLED.show();  
      FastLED.delay(1000/FRAMES_PER_SECOND); 
    
      EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
      break;      
    case hash("Red/White/Blue"):
      break; // Handled as part of the HTTP request
    case hash("Domoticz"):
      break; // Handled as part of the HTTP request
    case hash("Fire Blue"):
      FireBlue(55,120,15,false);
      break;
      
    // VU Effects
    case hash("Rainbow-1"):
      VU_BottomUp(1);
      break;  
    case hash("Rainbow-2"):
      VU_MiddleOut(1);
      break;  
    case hash("Rainbow-3"):
      VU_TopBottomIn(1);
      break;  
    case hash("Classic-1"):
      VU_BottomUp(2);
      break;  
    case hash("Classic-2"):
      VU_MiddleOut(2);
      break;  
    case hash("Classic-3"):
      VU_TopBottomIn(2);
      break;  
    case hash("Colour Change-1"):
      VU_BottomUp(3);
      break;  
    case hash("Colour Change-2"):
      VU_MiddleOut(3);
      break;  
    case hash("Colour Change-3"):
      VU_TopBottomIn(3);
      break;  

    case hash("Shatter"):
      VU_Shatter();
      break;  
    case hash("Stream"):
      VU_Stream();
      break;  
    case hash("Pulse"):
      VU_Pulse();
      break;  
    defult: 
      Serial.println("Unknown effect name in Loop!");
      break;
  }

  // If we're cycling through all patterns then move on to the next one when appropriate
  if (doAllEffects)
  {
    EVERY_N_SECONDS(iCyclingEffectDuration) 
    {
      iCurEffect = (iCurEffect + 1) % iNumEffects;
      if (iCurEffect == 0) iCurEffect=1;  // Skip effect zero (Domoticz) when cycling through the effects
      if (iCurEffect == (iNumEffects - 1)) iCurEffect=1;  // Skip the "All Patterns" effect when cycling through the effects
      strcpy(curEffect, Effectnames[iCurEffect]);
      Telnet.printf("Current Effect: %s\n\r", curEffect);

      // Clear the LEDs between effects
      // Clear all strips
      for(int q=0; q<NUM_STRIPS; q++)
      {
        fill_solid(leds[q], NUM_LEDS, CRGB::Black);
      }
      FastLED.show();

      //re-initialise the effects that need initialisation as we cycle through
      switch (hash(curEffect))
      {
        case hash("Red/White/Blue"):
          RedWhiteBlue(); // Sets the colours so no need to do anything in the main loop
          break;
        case hash("RainbowCycle"):
          iRainbowCounter = 0; // Initialise the rainbow sequence
          break;
        case hash("Balls"):
          InitialiseBouncingBalls(3, BallHeight);
          break;
        case hash("Rain"):
          meteorLoopCount = 0;
          break;      
      }
    }    
  }

    if (doAllVUEffects)
  {
    EVERY_N_SECONDS(iCyclingEffectDuration) 
    {
      iCurVUEffect = (iCurVUEffect + 1) % iNumVUEffects;
     if (iCurVUEffect == (iNumVUEffects - 1)) iCurVUEffect=0;  // Skip the "All VUs" effect when cycling through the effects
      strcpy(curEffect, VUEffectnames[iCurVUEffect]);
      Telnet.printf("Current VU Effect: %s\n\r", curEffect);

      // Clear the LEDs between effects - all strips
      for(int q=0; q<NUM_STRIPS; q++)
      {
        fill_solid(leds[q], NUM_LEDS, CRGB::Black);
      }
      FastLED.show();
    }    
  }
}
