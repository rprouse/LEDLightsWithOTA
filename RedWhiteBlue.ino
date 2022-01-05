void RedWhiteBlue()
{
  // Patriotic
  int iSectionBreak = int(NUM_LEDS / 3);
    
  for(int thisLed = 0; thisLed < NUM_LEDS; thisLed = thisLed + 1) {
    if ((thisLed < iSectionBreak) && (thisLed < (iSectionBreak * 2))) {
      //leds[thisLed] = CRGB::Blue;
      SetLED(-1, thisLed, CRGB(0,0,255));
    }
    if ((thisLed >= iSectionBreak) && (thisLed < (iSectionBreak * 3))) {
      //leds[thisLed] = CRGB::White;
      SetLED(-1, thisLed, CRGB(255,255,255));
    }
    if (thisLed >= (iSectionBreak * 2)) {
      //leds[thisLed] = CRGB::Red;
      SetLED(-1, thisLed, CRGB(255,0,0));
    }
  }

  FastLED.show();
}
