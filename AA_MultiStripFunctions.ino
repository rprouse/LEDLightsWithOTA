// A bunch of functions to allow us to handle multiple LED strips without making the main code too cumbersome

// First function is SetLED which we will use instead of the normal e.g. 
//    leds[12] = CRGB::Red;
//    FastLED.show();
//
// The above only works for a single dimension "leds" array.  Will multiple strips our array is 
// two dimensional i.e.
//    leds[1][12] = CRGB::Red;
//    FastLED.show();
//
// Where the [1] in the above is the strip number
//
// SetLED will take a LEDNum parameter, StripNum parameter and a colour parameter
// 
// We will simply set the requested LED on the requested strip to the requested colour
//
// The catch is that if we call this with the StripNum parameter = -1 then we'll set the same LED to
// the same colour on all strips.  This gives us back the functionality of treating each strip as a 
// mirror of the other.
//
// We'll have two versions of this function - one that takes the colour as a CRGB type and one as CHSV

void SetLED(int StripNum, int LEDNum, CRGB Colour)
 {
  if (StripNum != -1)
  {
    // Protect against trying to set LEDs outside of the allowable range
    if ((LEDNum >= 0) && (LEDNum < NUM_LEDS))
    {
      leds[StripNum][LEDNum] = Colour;
    }
  } else {
    for(int z=0; z<NUM_STRIPS; z++)
    {
      // Protect against trying to set LEDs outside of the allowable range
      if ((LEDNum >= 0) && (LEDNum < NUM_LEDS))
      {
        leds[z][LEDNum] = Colour;
      }
    }
  }
 }

 void multi_fadeToBlackBy(int StripNum, uint16_t num_leds, uint8_t fadeBy) 
 {
   if (StripNum != -1)
  {
    fadeToBlackBy(leds[StripNum], num_leds, fadeBy);
  } else {
    for(int z=0; z<NUM_STRIPS; z++)
    {
      fadeToBlackBy(leds[z], num_leds, fadeBy);
    }
  } 
 }
