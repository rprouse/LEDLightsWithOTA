void rainbowCycle(int SpeedDelay) {
  byte *c;
  uint16_t i;

  // Each time through set the LED colours from the wheel and increment the counter
  // This routine will be called repeatedly for a set period of time
  for(i=0; i< NUM_LEDS; i++) {
    c=Wheel(((i * 256 / NUM_LEDS) + iRainbowCounter) & 255);
    //leds[i] = CRGB(*c, *(c+1), *(c+2));
    SetLED(-1, i, CRGB(*c, *(c+1), *(c+2)));
  }
  FastLED.show();
  delay(SpeedDelay);

  iRainbowCounter++;
}

byte * Wheel(byte WheelPos) {
  static byte c[3];
 
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
}

void Wheel1(byte WheelPos,byte * c) {

  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }
}

uint32_t WheelCRGB(byte WheelPos) {

  uint32_t WheelCol;
  
  if(WheelPos < 85) {
    WheelCol = CRGB(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   WheelCol = CRGB(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   WheelCol = CRGB(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  return WheelCol;
}
