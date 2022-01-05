void RiseOn()
{
  FastLED.setBrightness(icurDomBrightness);  
    
  for(int iLED = 0; iLED < NUM_LEDS; iLED = iLED + 1) {
    // Turn our current led on to white, then show the leds
    //leds[iLED] = CRGB(icurDomRed, icurDomGreen, icurDomBlue);
    SetLED(-1, iLED, CRGB(icurDomRed, icurDomGreen, icurDomBlue));

    // Show the leds (only one of which is set to white, from above)
    FastLED.show();

    // Wait a little bit
    FastLED.delay(25);
 }
}

void FallOff()
{
 for(int iLED = NUM_LEDS; iLED > 0; iLED = iLED - 1) {
    // Turn our current led on to white, then show the leds
    //leds[iLED-1] = CRGB::Black;
    SetLED(-1, iLED-1, CRGB::Black);
    // Show the leds (only one of which is set to white, from above)
    FastLED.show();

    // Wait a little bit
    FastLED.delay(25);
 }
 //FastLED.setBrightness(0);
}
