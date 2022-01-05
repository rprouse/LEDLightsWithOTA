#include <Adafruit_NeoPixel.h>
#include <math.h>
#include <SoftwareSerial.h>

#define NUM_LEDS_HALF (NUM_LEDS/2) 

#define MIC_PIN   A0  // Microphone is attached to this analog pin

//Below settings moved to main code module
//int INPUT_FLOOR=35;    //Lower range of analogRead input
//int INPUT_CEILING=300; //Max range of analogRead input, the lower the value the more sensitive (1023 = max)300 (150)
//int SAMPLE_WINDOW=20;  // Sample window for average level
//int PEAK_HANG=35;      //Time of pause before peak dot falls
//int PEAK_FALL=20;      //Rate of falling peak dot
//int DRAW_MAX=100;      //For vu8 (shatter)

#define SAMPLES   60  // Length of buffer for dynamic level adjustment
#define SPEED .20       // Amount to increment RGB color by each cycle

#define COLOR_MIN           0
#define COLOR_MAX         255
#define SEGMENTS            3  // Number of segments to carve amplitude bar into
#define COLOR_WAIT_CYCLES  10  // Loop cycles to wait between advancing pixel origin

uint8_t maxChanges = 48;      // Value for blending between palettes.
 
CRGBPalette16 blackPalette(CRGB::Black);
CRGBPalette16 currentPalette(CRGB::Black);
CRGBPalette16 targetPalette(CloudColors_p);
CRGBPalette16 partyPalette(PartyColors_p);

byte volCount  = 0;      // Frame counter for storing past volume data
int
  reading,
  sample,
  vol[SAMPLES],       // Collection of prior volume samples
  lvl       = 0,      // Current "dampened" audio level
  lvl2      = 0,
  minLvlAvg = 0,      // For dynamic adjustment of graph low & high
  maxLvlAvg = 512;    
float
  greenOffset = 30,
  blueOffset = 150;

//vu ripple
uint8_t colour; 
uint8_t myfade = 255;                                         // Starting brightness.
#define maxsteps 16                                           // Case statement wouldn't allow a variable.
int peakspersec = 0;
int peakcount = 0;
uint8_t bgcol = 0;   

//Samples
#define NSAMPLES 64
float samplearray[NSAMPLES];
float samplesum = 0;
float  sampleavg = 0;
int samplecount = 0;
//unsigned int sample = 0;
unsigned long oldtime = 0;
unsigned long newtime = 0;

//Ripple variables
int color;
int center = 0;
int step = -1;
int maxSteps = 16;
float fadeRate = 0.80;
int diff;

//vu 8 variables
int
  origin = 0,
  color_wait_count = 0,
  scroll_color = COLOR_MIN,
  last_intensity = 0,
  intensity_max = 0,
  origin_at_flip = 0;
byte  draw[1024][3]; // Should be DRAW_MAX
boolean
  growing = false,
  fall_from_left = true;



//background color
uint32_t currentBg = random(256);
uint32_t nextBg = currentBg;
TBlendType    currentBlending;  
     
    
int peak = 0;      // Peak level of column; used for falling dots
int lastpeak = 0;
     
byte dotCount = 0;  //Frame counter for peak dot
byte dotHangCount = 0; //Frame counter for holding peak dot








float fscale( float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve)
{
  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;


  // condition curve parameter
  // limit range

  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;

  curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function

  
  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }

  // Zero Refference the values
  OriginalRange = originalMax - originalMin;

  if (newEnd > newBegin){
    NewRange = newEnd - newBegin;
  }
  else
  {
    NewRange = newBegin - newEnd;
    invFlag = 1;
  }

  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float

 
  // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine
  if (originalMin > originalMax ) {
    return 0;
  }

  if (invFlag == 0){
    rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;

  }
  else     // invert the ranges
  { 
    rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange);
  }

  return rangedValue;
}


float GetAnalog_PeakToPeak()
{
  unsigned long startMillis= millis();  // Start of sample window
  float peakToPeakLevel = 0;   // peak-to-peak level
  unsigned int signalMax = 0;
  unsigned int signalMin = 1023;
  
  while (millis() - startMillis < SAMPLE_WINDOW)
  {
    sample = analogRead(MIC_PIN);
    if (sample < 1024)  
    {
      if (sample > signalMax)
      {
        signalMax = sample;  
      }
      else if (sample < signalMin)
      {
        signalMin = sample;  
      }
    }
  }
  peakToPeakLevel = signalMax - signalMin;  

  return peakToPeakLevel;
}


float rollingAverage(float NewReading) 
{
  // Keep track of a rolling average of the readings
  samplesum = samplesum + NewReading - samplearray[samplecount];      // Add the new sample and remove the oldest sample in the array 
  sampleavg = samplesum / ROLLING_AVERAGE_COUNT;                      // Get an average
  samplearray[samplecount] = NewReading;                              // Update oldest sample in the array with new sample
  samplecount = (samplecount + 1) % ROLLING_AVERAGE_COUNT;            // Update the counter for the array

  return sampleavg;
}




void VU_BottomUp(int ColourStyle) 
{
  // ColourStyle Parameter Values:
  //    1 = VU is rainbow colours
  //    2 = VU is classic colours (green bar with red peak)
  //    3 = VU is gradiented colours but changing all the time
  
  uint8_t  i;
  int height;
  byte *c;
  CRGB PeakDotCol;
  CRGB thisLEDColour;
  
  float peakToPeak = 0;   // peak-to-peak level
  peakToPeak = GetAnalog_PeakToPeak();
  peakToPeak = rollingAverage(peakToPeak);  // Dampen the value to prevent too much flickering
   
  // Calculate bar height based on dynamic min/max levels (fixed point):
  height = fscale(INPUT_FLOOR, INPUT_CEILING, 0, NUM_LEDS, peakToPeak, 2);

  if (peak > NUM_LEDS) peak=0; // Reset peak if we've transitioned from another VU effect that has higher peaks than the max that this effect allows 
  if (height < 0L)       height = 0;      // Clip output
  if (height > NUM_LEDS) height = NUM_LEDS;
  if (height > peak)     peak   = height; // Keep 'peak' dot at top
  if (peak > lastpeak) dotHangCount=0; // reset the hang count if the peak has risen
  lastpeak = peak;

  if (ColourStyle == 3)  // Colour changing VU
  {
    greenOffset += SPEED;
    blueOffset += SPEED;
    if (greenOffset >= 255) greenOffset = 0;
    if (blueOffset >= 255) blueOffset = 0;
  }  
  
  // Fade all leds to black when not any other colour
  // This prevents jittery displays where analog reading goes from e.g. 0 to 100 to 0 to 100 in quick succession
  multi_fadeToBlackBy(-1, NUM_LEDS, FADE_TO_BLACK_BY);
  
  // Color pixels based on rainbow gradient
  for(i=0; i<height; i++) {
    switch(ColourStyle)
    {
      case 1: // Rainbow colours
        c=Wheel(map(i,0,NUM_LEDS-1,30,150));
        thisLEDColour = CRGB(*c, *(c+1), *(c+2));
        break;
      case 2: // Classic green bar
        thisLEDColour = CRGB::Green;
        break;
      case 3: // Colour changing bar
        c=Wheel(map(i, 0, NUM_LEDS-1, (int)greenOffset, (int)blueOffset));
        thisLEDColour = CRGB(*c, *(c+1), *(c+2));
        break;
    }
    SetLED(-1, i, thisLEDColour);
  }
 
 
  // Draw peak dot  
  if(peak > 0 && peak <= NUM_LEDS-1) 
  {
    if ((PEAK_DOT_COLOUR == 1) && (ColourStyle != 2)) // Peak dot should be in the effect colour and not "classic" (green/red) colours
    {
      switch(ColourStyle)
      {
        case 1: // Rainbow colours
          c = Wheel(map(peak,0,NUM_LEDS-1,30,150));
          PeakDotCol = CRGB(*c, *(c+1), *(c+2));
          break;
        case 3: // Colour changing bar
          c=Wheel(map(peak, 0, NUM_LEDS-1, (int)greenOffset, (int)blueOffset));
          PeakDotCol = CRGB(*c, *(c+1), *(c+2));
          break;
      }
    } else {
      PeakDotCol = CRGB::Red;
    }
    SetLED(-1, peak, PeakDotCol);
    SetLED(-1, peak-1, PeakDotCol); // make peak 2 dots wide
    SetLED(-1, peak-2, PeakDotCol); // make peak 3 dots wide    
  }
  
  FastLED.show(); // Update strip

  // Frame based peak dot animation
  if(dotHangCount > PEAK_HANG) { //Peak pause length
    if(++dotCount >= PEAK_FALL) { //Fall rate 
      if(peak > 0) peak--;
      dotCount = 0;
    }
  } 
  else {
    dotHangCount++; 
  }
}

 

void VU_MiddleOut(int ColourStyle) 
{
  // ColourStyle Parameter Values:
  //    1 = VU is rainbow colours
  //    2 = VU is classic colours (green bar with red peak)
  //    3 = VU is gradiented colours but changing all the time  
  uint8_t  i;
  int height;
  byte *c;
  CRGB PeakDotCol;
  CRGB thisLEDColour;
   
  float peakToPeak = 0;   // peak-to-peak level
  peakToPeak = GetAnalog_PeakToPeak();
  peakToPeak = rollingAverage(peakToPeak);  // Dampen the value to prevent too much flickering
   
  // Calculate bar height based on dynamic min/max levels (fixed point):
  height = fscale(INPUT_FLOOR, INPUT_CEILING, 0, NUM_LEDS_HALF, peakToPeak, 2);

  if (peak > NUM_LEDS_HALF) peak=0; // Reset peak if we've transitioned from another VU effect that has higher peaks than the max that this effect allows 
  if (height < 0L)       height = 0;      // Clip output
  if (height > NUM_LEDS_HALF) height = NUM_LEDS_HALF;
  if (height > peak)     peak   = height; // Keep 'peak' dot at top
  if (peak > lastpeak) dotHangCount=0; // reset the hang count if the peak has risen
  lastpeak = peak;

  if (ColourStyle == 3)  // Colour changing VU
  {
    greenOffset += SPEED;
    blueOffset += SPEED;
    if (greenOffset >= 255) greenOffset = 0;
    if (blueOffset >= 255) blueOffset = 0;
  }  

  // Fade all leds to black when not any other colour
  // This prevents jittery displays where analog reading goes from e.g. 0 to 100 to 0 to 100 in quick succession
  multi_fadeToBlackBy(-1, NUM_LEDS, FADE_TO_BLACK_BY);

  // Color pixels based on rainbow gradient
  for(i=0; i<height; i++) {
    switch(ColourStyle)
    {
      case 1: // Rainbow colours
        c=Wheel(map(i,0,NUM_LEDS_HALF-1,30,150));
        thisLEDColour = CRGB(*c, *(c+1), *(c+2));
        break;
      case 2: // Classic green bar
        thisLEDColour = CRGB::Green;
        break;
      case 3: // Colour changing bar
        c=Wheel(map(i,0,NUM_LEDS_HALF-1,(int)greenOffset, (int)blueOffset));
        thisLEDColour = CRGB(*c, *(c+1), *(c+2));
      break;
    }

    SetLED(-1, NUM_LEDS_HALF-i-1, thisLEDColour);
    SetLED(-1, NUM_LEDS_HALF+i, thisLEDColour);
  } 
 
  // Draw peak dot  
  if(peak > 2 && peak <= NUM_LEDS_HALF-1) 
  {
    if ((PEAK_DOT_COLOUR == 1) && (ColourStyle != 2)) // Peak dot should be in the effect colour and not "classic" (green/red) colours
    {
      switch(ColourStyle)
      {
        case 1: // Rainbow colours
          c = Wheel(map(peak,0,NUM_LEDS_HALF-1,30,150));
          PeakDotCol = CRGB(*c, *(c+1), *(c+2));
          break;
        case 3: // Colour changing bar
          c=Wheel(map(i,0,NUM_LEDS_HALF-1,(int)greenOffset, (int)blueOffset));
          PeakDotCol = CRGB(*c, *(c+1), *(c+2));
          break;
      }
    } else {
      PeakDotCol = CRGB::Red;
    }
    SetLED(-1, NUM_LEDS_HALF-peak, PeakDotCol);
    SetLED(-1, NUM_LEDS_HALF-peak+1, PeakDotCol); // make peak 2 dots wide
    SetLED(-1, NUM_LEDS_HALF-peak+2, PeakDotCol); // make peak 3 dots wide
    
    SetLED(-1, NUM_LEDS_HALF+peak, PeakDotCol);
    SetLED(-1, NUM_LEDS_HALF+peak-1, PeakDotCol); // make peak 2 dots wide  
    SetLED(-1, NUM_LEDS_HALF+peak-2, PeakDotCol); // make peak 3 dots wide  
  }
  FastLED.show(); // Update strip

  // Frame based peak dot animation
  if(dotHangCount > PEAK_HANG) { //Peak pause length
    if(++dotCount >= PEAK_FALL) { //Fall rate 
      if(peak > 0) peak--;
      dotCount = 0;
    }
  } 
  else {
    dotHangCount++; 
  }
}



void VU_TopBottomIn(int ColourStyle) 
{
  // ColourStyle Parameter Values:
  //    1 = VU is rainbow colours
  //    2 = VU is classic colours (green bar with red peak)
  //    3 = VU is gradiented colours but changing all the time  
  
  float peakToPeak = 0;   // peak-to-peak level
 
  unsigned int height;
  byte * c;
  CRGB PeakDotCol;
  CRGB thisLEDColour;
    
  peakToPeak = GetAnalog_PeakToPeak();
  peakToPeak = rollingAverage(peakToPeak);  // Dampen the value to prevent too much flickering
 
  height = fscale(INPUT_FLOOR, INPUT_CEILING, 0, NUM_LEDS_HALF, peakToPeak, 2);
 
  if (peak > NUM_LEDS_HALF) peak=0; // Reset peak if we've transitioned from another VU effect that has higher peaks than the max that this effect allows 
  if (height < 0L)       height = 0;      // Clip output
  if (height > NUM_LEDS_HALF) height = NUM_LEDS_HALF;
  if (height > peak)     peak   = height; // Keep 'peak' dot at top
  if (peak > lastpeak) dotHangCount=0; // reset the hang count if the peak has risen
  lastpeak = peak;

  if (ColourStyle == 3)  // Colour changing VU
  {
    greenOffset += SPEED;
    blueOffset += SPEED;
    if (greenOffset >= 255) greenOffset = 0;
    if (blueOffset >= 255) blueOffset = 0;
  }  

  // Fade all leds to black when not any other colour
  // This prevents jittery displays where analog reading goes from e.g. 0 to 100 to 0 to 100 in quick succession
  multi_fadeToBlackBy(-1, NUM_LEDS, FADE_TO_BLACK_BY);
  
  // Now display the vu bar
  if (height != 0)
  { 
    for (int i=0;i<height;i++){
    switch(ColourStyle)
    {
      case 1: // Rainbow colours
        c=Wheel(map(i,0,NUM_LEDS_HALF-1,30,150));
        thisLEDColour = CRGB(*c, *(c+1), *(c+2));
        break;
      case 2: // Classic green bar
        thisLEDColour = CRGB::Green;
        break;
      case 3: // Colour changing bar
        c=Wheel(map(i,0,NUM_LEDS_HALF-1,(int)greenOffset, (int)blueOffset));
        thisLEDColour = CRGB(*c, *(c+1), *(c+2));
      break;
    }

    SetLED(-1, (NUM_LEDS-i-1), thisLEDColour);
    SetLED(-1, (0+i), thisLEDColour);
    }  
  }

  // Do peak dots
  if(peak >= 2 && peak <= NUM_LEDS_HALF-1) {
    if ((PEAK_DOT_COLOUR == 1) && (ColourStyle != 2)) // Peak dot should be in the effect colour and not "classic" (green/red) colours
    {
      switch(ColourStyle)
      {
        case 1: // Rainbow colours
          c = Wheel(map(peak,0,NUM_LEDS_HALF-1,30,150));
          PeakDotCol = CRGB(*c, *(c+1), *(c+2));
          break;
        case 3: // Colour changing bar
          c=Wheel(map(peak,0,NUM_LEDS_HALF-1,(int)greenOffset, (int)blueOffset));
          PeakDotCol = CRGB(*c, *(c+1), *(c+2));
          break;
      }
    } else {
      PeakDotCol = CRGB::Red;
    }
    SetLED(-1, peak, PeakDotCol);
    SetLED(-1, (peak-1), PeakDotCol);  // Make peak 2 dots wide
    SetLED(-1, (peak-2), PeakDotCol);  // Make peak 3 dots wide
  
    SetLED(-1, (NUM_LEDS-peak), PeakDotCol);
    SetLED(-1, (NUM_LEDS-(peak+1)), PeakDotCol);   // Make peak 2 dots wide
    SetLED(-1, (NUM_LEDS-(peak+2)), PeakDotCol);   // Make peak 3 dots wide
  }
     
  FastLED.show();

  // Frame based peak dot animation
  if(dotHangCount > PEAK_HANG) { //Peak pause length
    if(++dotCount >= PEAK_FALL) { //Fall rate 
      if(peak > 0) peak--;
      dotCount = 0;
    }
  } 
  else {
    dotHangCount++; 
  }
}


void VU_Shatter() {
  int intensity = calculateIntensity();
  updateOrigin(intensity);
  assignDrawValues(intensity);
  writeSegmented();
}

int calculateIntensity() {
  int   intensity;
  float peakToPeak = 0;   // peak-to-peak level
  
  peakToPeak = GetAnalog_PeakToPeak();
  peakToPeak = rollingAverage(peakToPeak);  // Dampen the value to prevent too much flickering  

  // Calculate bar height based on dynamic min/max levels (fixed point):
  intensity = fscale(INPUT_FLOOR, INPUT_CEILING, 0, DRAW_MAX, peakToPeak, 2);
  
  return constrain(intensity, 0, DRAW_MAX-1);
}

void updateOrigin(int intensity) {
  // detect peak change and save origin at curve vertex
  if (growing && intensity < last_intensity) {
    growing = false;
    intensity_max = last_intensity;
    fall_from_left = !fall_from_left;
    origin_at_flip = origin;
  } else if (intensity > last_intensity) {
    growing = true;
    origin_at_flip = origin;
  }
  last_intensity = intensity;
  
  // adjust origin if falling
  if (!growing) {
    if (fall_from_left) {
      origin = origin_at_flip + ((intensity_max - intensity) / 2);
    } else {
      origin = origin_at_flip - ((intensity_max - intensity) / 2);
    }
    // correct for origin out of bounds
    if (origin < 0) {
      origin = DRAW_MAX - abs(origin);
    } else if (origin > DRAW_MAX - 1) {
      origin = origin - DRAW_MAX - 1;
    }
  }
}

void assignDrawValues(int intensity) {
  // draw amplitue as 1/2 intensity both directions from origin
  int min_lit = origin - (intensity / 2);
  int max_lit = origin + (intensity / 2);
  if (min_lit < 0) {
    min_lit = min_lit + DRAW_MAX;
  }
  if (max_lit >= DRAW_MAX) {
    max_lit = max_lit - DRAW_MAX;
  }
  for (int i=0; i < DRAW_MAX; i++) {
    // if i is within origin +/- 1/2 intensity
    if (
      (min_lit < max_lit && min_lit < i && i < max_lit) // range is within bounds and i is within range
      || (min_lit > max_lit && (i > min_lit || i < max_lit)) // range wraps out of bounds and i is within that wrap
    ) {
      Wheel1(scroll_color, &draw[i][0]);
    } else {
      draw[i][0] = 0;
      draw[i][1] = 0;
      draw[i][2] = 0;
    }
  }
  
  //advance color wheel ready for next time through
  color_wait_count++;
  if (color_wait_count > COLOR_WAIT_CYCLES) {
    color_wait_count = 0;
    scroll_color++;
    if (scroll_color > COLOR_MAX) {
      scroll_color = COLOR_MIN;
    }
  }
}

void writeSegmented() {
  int seg_len = NUM_LEDS / SEGMENTS;
  int drawindex;

  
  for (int s = 0; s < SEGMENTS; s++) {
    for (int i = 0; i < seg_len; i++) {
      drawindex = map(i, 0, seg_len, 0, DRAW_MAX);
      //leds[i + (s*seg_len)] = CRGB(draw[drawindex][0], draw[drawindex][1], draw[drawindex][2]);
      SetLED(-1, i + (s*seg_len), CRGB(draw[drawindex][0], draw[drawindex][1], draw[drawindex][2]));
    }
  }
  FastLED.show();
}

void VU_Stream() {
  currentPalette = blackPalette;
  
  EVERY_N_SECONDS(5) {                                        // Change the target palette to a random one every 5 seconds.
      for (int i = 0; i < 16; i++) {
        targetPalette[i] = CHSV(random8(), 255, 255);
      }
    }
  
  EVERY_N_MILLISECONDS(100) {
    uint8_t maxChanges = 24; 
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);   // AWESOME palette blending capability.
  }
  
  EVERY_N_MILLISECONDS(20) {                           // FastLED based non-blocking delay to update/display the sequence.
    soundtun();
    FastLED.show();
  }
} // loop()



void soundtun() {

  int sample;
  float peakToPeak = 0;   // peak-to-peak level

  peakToPeak = GetAnalog_PeakToPeak();
  peakToPeak = rollingAverage(peakToPeak);  // Dampen the value to prevent too much flickering
   
  // Calculate bar height based on dynamic min/max levels (fixed point):
  sample = fscale(INPUT_FLOOR, INPUT_CEILING, 0, 255, peakToPeak, 2);
  
  CRGB newcolour = ColorFromPalette(partyPalette, sample, sample, currentBlending);
  for(int q=0; q<NUM_STRIPS; q++)
  {
    nblend(leds[q][0], newcolour, 128);
  }
  for (int i = NUM_LEDS-1; i>0; i--) {
    for (int q=0; q<NUM_STRIPS; q++)
    {
      leds[q][i] = leds[q][i-1];
    }
  }
  
} 

void VU_Pulse() {
  currentPalette = OceanColors_p;                             // Initial palette.
  currentBlending = LINEARBLEND; 

  EVERY_N_SECONDS(5) {                                        // Change the palette every 5 seconds.
    for (int i = 0; i < 16; i++) {
      targetPalette[i] = CHSV(random8(), 255, 255);
    }
  }
  
  EVERY_N_MILLISECONDS(100) {                                 // AWESOME palette blending capability once they do change.
    uint8_t maxChanges = 24; 
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);
  }


  EVERY_N_MILLISECONDS(20) 
  {                            
    multi_fadeToBlackBy(-1, NUM_LEDS, 16);                        // 1 = slow, 255 = fast fade. Depending on the faderate, the LED's further away will fade out.
    soundble();
  }
  FastLED.show();

}



void soundble() {                                            // Quick and dirty sampling of the microphone.
  
  int sample;
  float peakToPeak = 0;   // peak-to-peak level

  peakToPeak = GetAnalog_PeakToPeak();
  peakToPeak = rollingAverage(peakToPeak);  // Dampen the value to prevent too much flickering
   
  // Calculate bar height based on dynamic min/max levels (fixed point):
  sample = fscale(INPUT_FLOOR, INPUT_CEILING, 0, 512, peakToPeak, 2);

  for(int q=0; q<NUM_STRIPS; q++)
  {
    leds[q][NUM_LEDS/2] = ColorFromPalette(currentPalette, sample, sample*2, currentBlending); // Put the sample into the center
    
    for (int i = NUM_LEDS - 1; i > NUM_LEDS/2; i--) {       //move to the left      // Copy to the left, and let the fade do the rest.
      leds[q][i] = leds[q][i - 1];
    }
  
    for (int i = 0; i < NUM_LEDS/2; i++) {                  // move to the right    // Copy to the right, and let the fade to the rest.
      leds[q][i] = leds[q][i + 1];
    }
    //addGlitter(sample); 
  }  
}  
