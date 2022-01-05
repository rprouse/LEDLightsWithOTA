void EEProm_ReadParams()
{
  // Read the relevant parameters from EEPROM if they've been saved there
  // EEProm usage:
  //    Byte  0: Ball height for bouncing balls
  //    Byte  1: Ball fade to black settings
  //    Byte  2: VU Meter Input Floor (High Byte)
  //    Byte  3: VU Meter Input Floor (Low Byte)
  //    Byte  4: VU Meter Input Ceiling (High Byte)
  //    Byte  5: VU Meter Input Ceiling (Low Byte)
  //    Byte  6: VU Sample window
  //    Byte  7: VU Fade to Black By setting
  //    Byte  8: VU Peak Hang setting
  //    Byte  9: VU Peak fall setting
  //    Byte 10: VU Peak dot colour
  //    Byte 11: VU Rolling Average count
  //    Byte 12: Draw Max (High Byte)
  //    Byte 13: Draw Max (Low Byte)
         
  // Read the Ball Height parameter
  int temp = 0;
  int temp1 = 0;
  
  temp = EEPROM.read(0);
  // Check if the value read is 255.  If it is, it means that the ESP has been reprogrammed and no value
  // is yet stored there.  User will need to go to settings page and save settings to set teh parameter
  // NOTE: Setting the "Erase Flash" option on the Arduino IDE Tools menu to "Sketch Only" will prevent
  // the settings being wiped out when re-programmed.
  if (temp == 255)
  {
    Serial.printf("No Ball Width parameter saved in EEPROM.  Using default of: %i\n", BallHeight);  
  } else {
    BallHeight = temp;
    Serial.printf("Ball Width parameter read from EEPROM.  Now set to: %i\n", BallHeight);      
  }

  // Ball fade to black
  temp = EEPROM.read(1);
  // Check if the value read is 255.  If it is, it means that the ESP has been reprogrammed and no value
  // is yet stored there.  User will need to go to settings page and save settings to set teh parameter
  // NOTE: Setting the "Erase Flash" option on the Arduino IDE Tools menu to "Sketch Only" will prevent
  // the settings being wiped out when re-programmed.
  if (temp == 255)
  {
    Serial.printf("No Ball Fade to Black parameter saved in EEPROM.  Using default of: %i\n", BallFadeToBlackBy);  
  } else {
    BallFadeToBlackBy = temp;
    Serial.printf("Ball Fade to Black parameter read from EEPROM.  Now set to: %i\n", BallFadeToBlackBy);      
  }


  // Read the Input Floor Parameter (Max value would be 1023 so if high byte is 255 assume we haven't save a value to
  // EEPROM yet
  temp = EEPROM.read(2);
  if (temp == 255)
  {
    Serial.printf("No Input Floor parameter saved in EEPROM.  Using default of: %i\n", INPUT_FLOOR);  
  } else {
    temp1 = EEPROM.read(3);
    INPUT_FLOOR = (temp << 8) + temp1; // shift the high byte 8 places right and add the low byte 
    Serial.printf("Input Floor parameter read from EEPROM.  Now set to: %i\n", INPUT_FLOOR);      
  }
  
  // Read the Input Ceiling Parameter (Max value would be 1023 so if high byte is 255 assume we haven't save a value to
  // EEPROM yet
  temp = EEPROM.read(4);
  if (temp == 255)
  {
    Serial.printf("No Input Ceiling parameter saved in EEPROM.  Using default of: %i\n", INPUT_CEILING);  
  } else {
    temp1 = EEPROM.read(5);
    INPUT_CEILING = (temp << 8) + temp1; // shift the high byte 8 places right and add the low byte 
    Serial.printf("Input Ceiling parameter read from EEPROM.  Now set to: %i\n", INPUT_CEILING);      
  }

  // Sample Window
  temp = EEPROM.read(6);
  if (temp == 255)
  {
    Serial.printf("No Sample Window parameter saved in EEPROM.  Using default of: %i\n", SAMPLE_WINDOW);  
  } else {
    SAMPLE_WINDOW = temp;
    Serial.printf("Sample Window parameter read from EEPROM.  Now set to: %i\n", SAMPLE_WINDOW);      
  }

  // Fade to Black by
  temp = EEPROM.read(7);
  if (temp == 255)
  {
    Serial.printf("No Fade to Black parameter saved in EEPROM.  Using default of: %i\n", FADE_TO_BLACK_BY);  
  } else {
    FADE_TO_BLACK_BY = temp;
    Serial.printf("Fade to Black parameter read from EEPROM.  Now set to: %i\n", FADE_TO_BLACK_BY);      
  }

  // Peak Hang
  temp = EEPROM.read(8);
  if (temp == 255)
  {
    Serial.printf("No Peak Hang parameter saved in EEPROM.  Using default of: %i\n", PEAK_HANG);  
  } else {
    PEAK_HANG = temp;
    Serial.printf("Peak Hang parameter read from EEPROM.  Now set to: %i\n", PEAK_HANG);     
  } 

  // Peak Fall
  temp = EEPROM.read(9);
  if (temp == 255)
  {
    Serial.printf("No Peak Fall parameter saved in EEPROM.  Using default of: %i\n", PEAK_FALL);  
  } else {
    PEAK_FALL = temp;
    Serial.printf("Peak Fall parameter read from EEPROM.  Now set to: %i\n", PEAK_FALL);        
  } 

  // Peak Dot Colour (1 = effect colour, 2 = red)
  temp = EEPROM.read(10);
  if (temp == 255)
  {
    Serial.printf("No Peak Colour parameter saved in EEPROM.  Using default of: %i\n", PEAK_DOT_COLOUR);  
  } else {
    PEAK_DOT_COLOUR = temp;
    Serial.printf("Peak Colour parameter read from EEPROM.  Now set to: %i\n", PEAK_DOT_COLOUR);        
  }   

  // Rolling average count - the number of previous readings to keep an average over
  temp = EEPROM.read(11);
  if (temp == 255)
  {
    Serial.printf("No Rolling Average Count parameter saved in EEPROM.  Using default of: %i\n", ROLLING_AVERAGE_COUNT);  
  } else {
    ROLLING_AVERAGE_COUNT = temp;
    Serial.printf("Rolling Average Count parameter read from EEPROM.  Now set to: %i\n", ROLLING_AVERAGE_COUNT);        
  }  

  // Read the Draw MAX Parameter (Max value would be 1023 so if high byte is 255 assume we haven't save a value to
  // EEPROM yet
  temp = EEPROM.read(12);
  if (temp == 255)
  {
    Serial.printf("No Draw Max parameter saved in EEPROM.  Using default of: %i\n", DRAW_MAX);  
  } else {
    temp1 = EEPROM.read(13);
    DRAW_MAX = (temp << 8) + temp1; // shift the high byte 8 places right and add the low byte 
    Serial.printf("Draw Max parameter read from EEPROM.  Now set to: %i\n", DRAW_MAX);      
  }

}

void EEProm_WriteParams()
{
  // Write the relevant parameters to EEPROM
  // See EEProm_ReadParams for EEPROM usage

  EEPROM.write(0, BallHeight & 0xFF);
  EEPROM.write(1, BallFadeToBlackBy & 0xFF);

  EEPROM.write(2, (INPUT_FLOOR >> 8) & 0xFF); // High byte
  EEPROM.write(3, INPUT_FLOOR & 0xFF);        // Low byte
  
  EEPROM.write(4, (INPUT_CEILING >> 8) & 0xFF); // High byte
  EEPROM.write(5, INPUT_CEILING & 0xFF);        // Low byte

  EEPROM.write(6, SAMPLE_WINDOW & 0xFF);
  EEPROM.write(7, FADE_TO_BLACK_BY & 0xFF);

  EEPROM.write(8, PEAK_HANG & 0xFF);
  EEPROM.write(9, PEAK_FALL & 0xFF);
  EEPROM.write(10, PEAK_DOT_COLOUR & 0xFF);
  EEPROM.write(11, ROLLING_AVERAGE_COUNT & 0xFF);

  EEPROM.write(12, (DRAW_MAX >> 8) & 0xFF); // High byte
  EEPROM.write(13, DRAW_MAX & 0xFF);        // Low byte  
    
  EEPROM.commit();
}
