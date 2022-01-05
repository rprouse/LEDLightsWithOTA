//////////////////////////////////////////////////////////////////////////////////////////
// MQTT Stuff
void MQTTCallback(char* topic, byte* payload, unsigned int length) {
  // Handle messages coming through on the Domoticz subscription
  // Some debug stuff that can be uncommented if required
  
  Telnet.printf("Message arrived in topic: %s\n\r", topic);
  
  Telnet.println("Message:");
  for (int i = 0; i < length; i++) {
    Telnet.print((char)payload[i]);
    if ((char)payload[i] == '\n') {
      Telnet.print("\r");
    }
  }
 
  Telnet.println();
  Telnet.println("-----------------------");
  
  
  StaticJsonDocument<1024> jsonBuffer;
  DeserializationError  res = deserializeJson(jsonBuffer, payload);
  if (res) {
    Serial.println("parseObject() failed");
  } else {
    int iTempBrightness = jsonBuffer["Level"];
    // Convert Domoticz brightness (0-100) to FastLED brightness (0-255)
    icurDomBrightness = int(float((255 * iTempBrightness)/100));
    Serial.printf("Brightness (level): %i\n", icurDomBrightness);
    Telnet.printf("Brightness (level): %i\n\r", icurDomBrightness);
    
    int iOnOff = jsonBuffer["nvalue"];
    if (iOnOff != 0) {
      iOnOff = 1;
    }
    icurDomOffOn = iOnOff;
    Serial.printf("On/Off (nvalue): %i\n", iOnOff);
    
    icurDomRed = jsonBuffer["Color"]["r"];
    icurDomGreen = jsonBuffer["Color"]["g"];
    icurDomBlue = jsonBuffer["Color"]["b"];

    if ((icurDomRed==255) && (icurDomGreen==255) && (icurDomBlue==255)) {
      // Domoticz has requested "white" but we want a warm white so adjust the colours
      // Really used when requesting a colour change from Apple Home as it only understands certain
      // colour names e.g. Cyan, Magenta, Blue, White etc.
      // Unfortunately it doesn't understand "warm white" so if we request "white" via Apple Home we'll adjust
      // to warm white
      // 255,131,28 captured from previous setting
      Telnet.println("I'm here!");
      icurDomRed = 255;
      icurDomGreen = 131;
      icurDomBlue = 28;
    }
    Telnet.println("New state from Domoticz - doing command");
    SetLightsFromDomoticz (icurDomRed, icurDomGreen, icurDomBlue, icurDomBrightness, icurDomOffOn);
    
    //serializeJsonPretty(jsonBuffer, Serial);
    //Serial.println("");
    }
}

//////////////////////////////////////////////////////////////////////////////////////////

void DomoticzGetDeviceInfo()
{
  // Request the RGB switch device info via MQTT
  // Will result in the latest state info being sent back via MQTT 
  // which will be handled in the same way as a manual switch command
    char domMsg[256];
    sprintf(domMsg, "{\"command\": \"getdeviceinfo\", \"idx\":%i}\0", switchIDX);  // Message to Domoticz will something look like {"command": "getdeviceinfo", "idx": 74}
    MQTTclient.publish("domoticz/in", domMsg);
}

//////////////////////////////////////////////////////////////////////////////////////////

void SetLightsFromDomoticz(int r, int g, int b, int bright, int OnOff)
{
  // Set all leds per domoticz
  if (OnOff == 0)
  {
    Telnet.println("Off");
    strcpy(curEffect,"Domoticz"); // Set the current effect to Domoticz
    FallOff();
  } else {
    //Turn lights on
    Telnet.println("On");
    strcpy(curEffect,"Domoticz"); // Set the current effect to Domoticz
    RiseOn();
  }
}
