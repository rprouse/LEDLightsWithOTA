///////////////////////////////////////////////////////////////
#include <sstream>

// HTTP Stuff
void BuildWebPage () {
  webPage += "<HTML>  <HEAD>  <TITLE>LED Light Controller v3.1</TITLE> </HEAD>";
  webPage += "<BODY> <CENTER>  <H1>LED Light Controller v3.1</H1> </CENTER>";

  char IPInfo[256];
  sprintf(IPInfo, "Connected to %s with IP address %s",WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
  std::string IPInfoStr = IPInfo;
  
  webPage += "<br/><Center><H2>" + IPInfoStr + "</H2></Center><br/><br/>";
    
  webPage += "<FORM method=\"post\" action=\"/form\">";
  webPage += "<H1>Light Effects</H1>";
  
  webPage += "<Table><tr>";

  // Loop over the effect names array and put a button on the page for each.
  // Only three buttons per row as will be used predominantly on a mobile
  int iCurButton=0;
  
  for(int i=0; i < iNumEffects; i++)
  {
    std::string temp;
    temp = Effectnames[i];
    webPage += "<td style=\"text-align: center\"><INPUT TYPE=SUBMIT VALUE=\"" + temp + "\" name=\"submit\" style=\"font-size : 30px; width: 300px; height: 75px;\"></td>";

    iCurButton++;
    if (iCurButton == 3)
    {
      // Need a new row in the table.  Throw a few blank rows in to space the buttons out a bit  
      webPage += "</tr><tr></tr><tr>  </tr><tr></tr><tr>";
      iCurButton = 0;
    }
  }

  webPage += "</td></tr></table>";

  // Add in the VU Effects
  webPage += "<hr>";
  webPage += "<H1>VU Effects</H1>";

  webPage += "<Table><tr>";
  iCurButton=0;
  for(int i=0; i < iNumVUEffects; i++)
  {
    std::string temp;
    temp = VUEffectnames[i];
    webPage += "<td style=\"text-align: center\"><INPUT TYPE=SUBMIT VALUE=\"" + temp + "\" name=\"submit\" style=\"font-size : 30px; width: 300px; height: 75px;\"></td>";

    iCurButton++;
    if (iCurButton == 3)
    {
      // Need a new row in the table.  Throw a few blank rows in to space the buttons out a bit  
      webPage += "</tr><tr></tr><tr>  </tr><tr></tr><tr>";
      iCurButton = 0;
    }
  }  
  webPage += "</td></tr></table>";

  webPage += "<H1>Settings</H1>";
  webPage += "<hr><INPUT TYPE=SUBMIT VALUE=\"Settings\" name=\"submit\" style=\"font-size : 30px; width: 300px; height: 75px;\">";

  webPage += "</FORM></BODY></HTML>";    

  buildSettingsPage(false);
}

void buildSettingsPage(bool HasBeenUpdated)
{
  std::ostringstream temp;

  settingsPage = "";
  settingsPage += "<HTML>  <HEAD>  <TITLE>LED Light Controller Settings</TITLE> </HEAD>";
  settingsPage += "<BODY> <CENTER>  <H1>LED Light Controller Settings</H1> </CENTER>";
  if (HasBeenUpdated)
  {
    settingsPage += "<br/><br/><CENTER>  <H1>Settings saved successfully!</H1> </CENTER><br/><br/>";
  }

  // Effect Settings
  settingsPage += "<FORM><TABLE>";

  // First setting is for the bouncing ball effect to determine the width (number of LEDs) for each ball
  settingsPage += "<tr><td>";
  settingsPage += "<label style=\"font-size:40px; font-family: Arial\" for=\"BallHeight\">Ball Height:</label>";
  settingsPage += "</td><td>";
  settingsPage += "<select style=\"font-size:40px; font-family: Arial; width: 150px; text-align-last: center;\" id=\"BallHeight\" name=\"BallHeight\">";

  // Create a drop down of options with values 1-4
  for (int i=1; i<5; i++)
  {
    temp.str("");
    temp << i;  // convert the integer i into a stringstream so we can concatenate it below
    if (i==BallHeight)
    {
      settingsPage += "<option value=\"" + temp.str() + "\" selected>" + temp.str() + "</option>";
    } else {
      settingsPage += "<option value=\"" + temp.str() + "\">" + temp.str() + "</option>";
    }
  }
  settingsPage += "</select></td></tr>";

  // Ball Fade to Black setting
  settingsPage += "<tr><td>";
  settingsPage += "<label style=\"font-size:40px; font-family: Arial\" for=\"BallFadeToBlackBy\">Ball Fade To Black:</label>";
  temp.str("");
  settingsPage += "</td><td>";
  temp << BallFadeToBlackBy;  // convert the integer i into a stringstream so we can concatenate it below
  settingsPage += "<input type=\"text\" style=\"font-size:40px; font-family: Arial; width: 150px; text-align: center;\" id=\"BallFadeToBlackBy\" name=\"BallFadeToBlackBy\" value=\"" + temp.str() + "\">";
  settingsPage += "</td></tr>";
  

  settingsPage += "</table><hr>";
  // VU Settings
  // Now the floor and ceiling settings for the VU meter "sensitivity"
  settingsPage += "<table><tr><td>";
  settingsPage += "<label style=\"font-size:40px; font-family: Arial\" for=\"InputFloor\">Input Floor:</label>";
  temp.str("");
  settingsPage += "</td><td>";
  temp << INPUT_FLOOR;  // convert the integer i into a stringstream so we can concatenate it below
  settingsPage += "<input type=\"text\" style=\"font-size:40px; font-family: Arial; width: 150px; text-align: center;\" id=\"InputFloor\" name=\"InputFloor\" value=\"" + temp.str() + "\">";
  settingsPage += "</td></tr>";

  settingsPage += "<tr><td>";
  settingsPage += "<label style=\"font-size:40px; font-family: Arial\" for=\"InputCeiling\">Input Ceiling:</label>";
  temp.str("");
  settingsPage += "</td><td>";
  temp << INPUT_CEILING;  // convert the integer i into a stringstream so we can concatenate it below
  settingsPage += "<input type=\"text\" style=\"font-size:40px; font-family: Arial; width: 150px; text-align: center;\" id=\"InputCeiling\" name=\"InputCeiling\" value=\"" + temp.str() + "\">";
  settingsPage += "</td></tr>";

  // VU Sample Window
  settingsPage += "<tr><td>";
  settingsPage += "<label style=\"font-size:40px; font-family: Arial\" for=\"SampleWindow\">Sample Window:</label>";
  temp.str("");
  settingsPage += "</td><td>";
  temp << SAMPLE_WINDOW;  // convert the integer i into a stringstream so we can concatenate it below
  settingsPage += "<input type=\"text\" style=\"font-size:40px; font-family: Arial; width: 150px; text-align: center;\" id=\"SampleWindow\" name=\"SampleWindow\" value=\"" + temp.str() + "\">";
  settingsPage += "</td></tr>";

  // VU Fade to black
  settingsPage += "<tr><td>";
  settingsPage += "<label style=\"font-size:40px; font-family: Arial\" for=\"FadeToBlackBy\">VU Fade To Black:</label>";
  temp.str("");
  settingsPage += "</td><td>";
  temp << FADE_TO_BLACK_BY;  // convert the integer i into a stringstream so we can concatenate it below
  settingsPage += "<input type=\"text\" style=\"font-size:40px; font-family: Arial; width: 150px; text-align: center;\" id=\"FadeToBlackBy\" name=\"FadeToBlackBy\" value=\"" + temp.str() + "\">";
  settingsPage += "</td></tr>";

  //VU Peak Hang Time
  settingsPage += "<tr><td>";
  settingsPage += "<label style=\"font-size:40px; font-family: Arial\" for=\"PeakHangTime\">Peak Hang Time:</label>";
  temp.str("");
  settingsPage += "</td><td>";
  temp << PEAK_HANG;  // convert the integer i into a stringstream so we can concatenate it below
  settingsPage += "<input type=\"text\" style=\"font-size:40px; font-family: Arial; width: 150px; text-align: center;\" id=\"PeakHangTime\" name=\"PeakHangTime\" value=\"" + temp.str() + "\">";
  settingsPage += "</td></tr>";

  // VU Peak Fall Rate
  settingsPage += "<tr><td>";
  settingsPage += "<label style=\"font-size:40px; font-family: Arial\" for=\"PeakFallRate\">Peak Fall Rate:</label>";
  temp.str("");
  settingsPage += "</td><td>";
  temp << PEAK_FALL;  // convert the integer i into a stringstream so we can concatenate it below
  settingsPage += "<input type=\"text\" style=\"font-size:40px; font-family: Arial; width: 150px; text-align: center;\" id=\"PeakFallRate\" name=\"PeakFallRate\" value=\"" + temp.str() + "\">";
  settingsPage += "</td></tr>";

  // VU Rolling Average
  settingsPage += "<tr><td>";
  settingsPage += "<label style=\"font-size:40px; font-family: Arial\" for=\"RollingAverageCount\">Rolling Average Count:</label>";
  settingsPage += "</td><td>";
  settingsPage += "<select style=\"font-size:40px; font-family: Arial; width: 150px; text-align-last: center;\" id=\"RollingAverageCount\" name=\"RollingAverageCount\">";

  // Create a drop down of options with values 2-16
  for (int i=2; i<17; i++)
  {
    temp.str("");
    temp << i;  // convert the integer i into a stringstream so we can concatenate it below
    if (i==ROLLING_AVERAGE_COUNT)
    {
      settingsPage += "<option value=\"" + temp.str() + "\" selected>" + temp.str() + "</option>";
    } else {
      settingsPage += "<option value=\"" + temp.str() + "\">" + temp.str() + "</option>";
    }
  }
  settingsPage += "</select></td></tr>";

  // VU Peak Dot Colour
  settingsPage += "<tr><td>";
  settingsPage += "<label style=\"font-size:40px; font-family: Arial\" for=\"PeakDotColour\">Peak Dot Colour:</label>";
  settingsPage += "</td><td>";
  settingsPage += "<select style=\"font-size:40px; font-family: Arial; width: 300px; text-align-last: center;\" id=\"PeakDotColour\" name=\"PeakDotColour\">";
  if (PEAK_DOT_COLOUR == 1)
  {
      settingsPage += "<option value=\"1\" selected>Effect Colour</option>";
      settingsPage += "<option value=\"2\">Red</option>";
  } else {
      settingsPage += "<option value=\"1\">Effect Colour</option>";
      settingsPage += "<option value=\"2\" selected>Red</option>";    
  }
  settingsPage += "</select></td></tr>";

  // DRAW_MAX for vu8 (shatter)
  settingsPage += "<tr><td>";
  settingsPage += "<label style=\"font-size:40px; font-family: Arial\" for=\"DrawMax\">Draw Max:</label>";
  temp.str("");
  settingsPage += "</td><td>";
  temp << DRAW_MAX;  // convert the integer i into a stringstream so we can concatenate it below
  settingsPage += "<input type=\"text\" style=\"font-size:40px; font-family: Arial; width: 150px; text-align: center;\" id=\"DrawMax\" name=\"DrawMax\" value=\"" + temp.str() + "\">";
  settingsPage += "</td></tr>";

  settingsPage += "</TABLE><br/><hr><br/><br/>";

  // Put a save button on so we can action the change of settings in the page handler
  settingsPage += "<INPUT TYPE=SUBMIT VALUE=\"Save Settings\" name=\"submit\" style=\"font-size : 30px; width: 300px; height: 75px;\">";
  settingsPage += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
  settingsPage += "<INPUT TYPE=SUBMIT VALUE=\"Back\" name=\"submit\" style=\"font-size : 30px; width: 300px; height: 75px;\">";
  
  settingsPage += "</FORM></BODY></HTML>";      
}

void handleRoot() {
 //String s = MAIN_page; //Read HTML contents
 //char webPage[sizeof(MAIN_page)];
 //memcpy(webPage, MAIN_page, sizeof(MAIN_page));
 const char * charWebPage = webPage.c_str();
 HTTPserver.send(200, "text/html", charWebPage); //Send web page
}

void handleForm()
{
  String sEffectName = HTTPserver.arg("submit");
  
  Telnet.printf("Received button press for effect: %s\n\r", sEffectName.c_str());
  Serial.printf("Received button press for effect: %s\n", sEffectName.c_str());
  
  strcpy(curEffect, sEffectName.c_str());  // Set the current effect name based on the "submit" parameter on the URL

  if (sEffectName == "Settings")
  {
    Serial.println("Redirecting to settings page");
    // redirect to the settings page
    HTTPserver.sendHeader("Location", "/settings");
    HTTPserver.send(302, "text/plain", "Done");
  } else {
    // handle the effect request
    // For most effects, we'll just handle the effect in the main loop
    // We'll know what the effect is based on the curEffect name
    // A few effects need setting up here so let's handle those
    doAllEffects = false;
    doAllVUEffects = false;
    switch (hash(curEffect))
    {
      case hash("Red/White/Blue"):
        RedWhiteBlue(); // Sets the colours so no need to do anything in the main loop
        break;
      case hash("Domoticz"):
        // Get the latest light settings from Domoticz and set the lights accordingly
        DomoticzGetDeviceInfo(); 
        break; 
      case hash("RainbowCycle"):
        iRainbowCounter = 0; // Initialise the rainbow sequence
        break;
      case hash("Balls"):
        InitialiseBouncingBalls(NumBalls, BallHeight);
        break;
      case hash("Rain"):
        meteorLoopCount = 0;
        break;      
      case hash("All Patterns"):
        // Clear all LEDs before we start and set the first pattern - all strips
        for(int q=0; q<NUM_STRIPS; q++)
        {
          fill_solid(leds[q], NUM_LEDS, CRGB::Black);
        }
        FastLED.show();
        
        iCurEffect = 1;
        strcpy(curEffect, Effectnames[iCurEffect]);
        doAllEffects = true;
        break;
      case hash("All VUs"):
        // Clear all LEDs before we start and set the first pattern - all strips
        for(int q=0; q<NUM_STRIPS; q++)
        {
          fill_solid(leds[q], NUM_LEDS, CRGB::Black);
        }
        FastLED.show();
        
        iCurVUEffect = 0;
        strcpy(curEffect, VUEffectnames[iCurVUEffect]);
        doAllVUEffects = true;
        break;
      defult: 
        Serial.println("Unknown effect name in HTTP handler!");
        break;
    }

    HTTPserver.sendHeader("Location", "/");
    HTTPserver.send(302, "text/plain", "Done");  //This Line Keeps It on Same Page 
  }
}

void handleSettings()
{
  const char * charSettingsPage = settingsPage.c_str();

  String sSettingsAction = HTTPserver.arg("submit");
  
  Serial.println("In settings!"); 
  Serial.printf("Received action from the settings page: %s\n", sSettingsAction.c_str());


  if (sSettingsAction == "Back")
  {
    Serial.println("Redirecting to home page");
    // redirect to the settings page
    HTTPserver.sendHeader("Location", "/");
    HTTPserver.send(302, "text/plain", "Done");
  } 
  
  if (sSettingsAction == "") {
    // We're here because we've been redirected here so display the page
    // rebuild the page first just to make sure
    buildSettingsPage(false);
    charSettingsPage = settingsPage.c_str();
    HTTPserver.send(200, "text/html", charSettingsPage); //Send web page
  }

  if (sSettingsAction == "Save Settings") {
    // We're here because we need to save the settings, so let's get on with it
    String sBallHeight = HTTPserver.arg("BallHeight");
    String sInputFloor = HTTPserver.arg("InputFloor");
    String sInputCeiling = HTTPserver.arg("InputCeiling");
    String sSampleWindow = HTTPserver.arg("SampleWindow");
    String sFadeToBlackBy = HTTPserver.arg("FadeToBlackBy");
    String sBallFadeToBlackBy = HTTPserver.arg("BallFadeToBlackBy");
    String sPeakHangTime = HTTPserver.arg("PeakHangTime");
    String sPeakFallRate = HTTPserver.arg("PeakFallRate");
    String sRollingAverageCount = HTTPserver.arg("RollingAverageCount");
    String sPeakDotColour = HTTPserver.arg("PeakDotColour");
    String sDrawMax = HTTPserver.arg("DrawMax");
    
    BallHeight = atoi(sBallHeight.c_str());
    BallFadeToBlackBy = atoi(sBallFadeToBlackBy.c_str());
    INPUT_FLOOR = atoi(sInputFloor.c_str());
    INPUT_CEILING = atoi(sInputCeiling.c_str());
    SAMPLE_WINDOW = atoi(sSampleWindow.c_str());
    FADE_TO_BLACK_BY = atoi(sFadeToBlackBy.c_str());
    PEAK_HANG = atoi(sPeakHangTime.c_str());
    PEAK_FALL = atoi(sPeakFallRate.c_str());
    ROLLING_AVERAGE_COUNT = atoi(sRollingAverageCount.c_str());
    PEAK_DOT_COLOUR = atoi(sPeakDotColour.c_str());
    DRAW_MAX = atoi(sDrawMax.c_str());

    // Save params to EEPROM
    EEProm_WriteParams();
    
    // Finally redirect back to the page
    buildSettingsPage(true);
    charSettingsPage = settingsPage.c_str();
    HTTPserver.send(200, "text/html", charSettingsPage); //Send web page
  }

}
