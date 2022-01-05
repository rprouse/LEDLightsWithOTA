///////////////////////////////////////////////////////////////

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay) {  
  if (meteorLoopCount == 0)
  {
    // Blank all the strips
    for(int q=0; q<NUM_STRIPS; q++)
    {
      fill_solid(leds[q], NUM_LEDS, CRGB::Black);
    }
  }
  
  // fade brightness all LEDs one step
  for(int j=0; j<NUM_LEDS; j++) {
    if( (!meteorRandomDecay) || (random(10)>5) ) {
      for(int q=0; q<NUM_STRIPS; q++)
      {
        leds[q][NUM_LEDS-j].fadeToBlackBy( meteorTrailDecay );      
      }
    }
  }
 
  // draw meteor
  for(int j = 0; j < meteorSize; j++) {
    if( ( meteorLoopCount-j <NUM_LEDS) && (meteorLoopCount-j>=0) ) {
      //leds[NUM_LEDS-(meteorLoopCount-j)] = CRGB(red, green, blue);
      SetLED(-1, (NUM_LEDS-(meteorLoopCount-j)), CRGB(red, green, blue));
    }
  }
 
  FastLED.show();
  //delay(SpeedDelay);
  meteorLoopCount++;
  if (meteorLoopCount == (NUM_LEDS+NUM_LEDS))
  {
    meteorLoopCount = 0;
  }
}
