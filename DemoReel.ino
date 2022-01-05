#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
    for(int i=0; i<NUM_STRIPS; i++)
    {
      fill_rainbow(leds[i], NUM_LEDS, gHue, 5);
    }
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    //leds[ random16(NUM_LEDS) ] += CRGB::White;
    int temp = random16(NUM_LEDS);
    for(int q=0; q<NUM_STRIPS; q++)
    {
      leds[q][temp] += CRGB::White;
    }  
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  //fadeToBlackBy( leds, NUM_LEDS, 10);
  multi_fadeToBlackBy(-1, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  
  //leds[pos] += CHSV( gHue + random8(64), 200, 255);
  for(int q=0; q<NUM_STRIPS; q++)
  {
    leds[q][pos] += CHSV( gHue + random8(64), 200, 255);
  }
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  multi_fadeToBlackBy(-1, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  SetLED(-1, pos, CHSV( gHue, 255, 255));
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { 
    //leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    SetLED(-1, i, ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10)));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  multi_fadeToBlackBy( -1, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    //leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 255, 255);
    for(int q=0; q<NUM_STRIPS; q++)
    {
      leds[q][beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 255, 255);
    }
    dothue += 32;
  }
}
