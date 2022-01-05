float Gravity = -9.81;
int   StartHeight = 1;
float Height[10];
float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
float ImpactVelocity[10];
float TimeSinceLastBounce[10];
int   Position[10];
long  ClockTimeSinceLastBounce[10];
float Dampening[10];

void InitialiseBouncingBalls(int BallCount, int BallHeight)
{
  // Clear the display before starting - all strips
  for(int q=0; q<NUM_STRIPS; q++)
  {
    fill_solid(leds[q], NUM_LEDS, CRGB::Black);
  }
  FastLED.show();

  int AdjustedStartHeight = StartHeight + (BallHeight - 1); // Adjust start height for width of balls
  int AdjustedStartImpactVelocityStart = sqrt( -2 * Gravity * AdjustedStartHeight ); // Adjust start height for width of balls
  
  for (int i = 0 ; i < BallCount ; i++) {   
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = AdjustedStartHeight;
    Position[i] = 0; 
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i)/pow(BallCount,2); 
  }  
}

void BouncingColoredBallsLoop(int BallCount, byte colors[][3], int BallHeight) {
  int AdjustedStartHeight = StartHeight + (BallHeight - 1); // Adjust start height for width of balls
  int AdjustedStartImpactVelocityStart = sqrt( -2 * Gravity * AdjustedStartHeight ); // Adjust start height for width of balls
  
  for (int i = 0 ; i < BallCount ; i++) {
    TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
    Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i]/1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i]/1000;
    if ( Height[i] < 0 ) {                      
      Height[i] = 0;
      ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
      ClockTimeSinceLastBounce[i] = millis();
      if ( ImpactVelocity[i] < 0.01 ) {
        ImpactVelocity[i] = AdjustedStartImpactVelocityStart;
      }
    }
    Position[i] = round( Height[i] * (NUM_LEDS - 1) / AdjustedStartHeight);
  }
  for (int i = 0 ; i < BallCount ; i++) {
    for (int j=0; j < BallHeight; j++)
    {
      //leds[Position[i]-j] = CRGB(colors[i][0],colors[i][1],colors[i][2]);
      SetLED(-1, (Position[i]-j), CRGB(colors[i][0],colors[i][1],colors[i][2]));
    }
  }
  FastLED.show();

  // Fade all strips
  for(int q=0; q<NUM_STRIPS; q++)
  {
      fadeToBlackBy(leds[q], NUM_LEDS, BallFadeToBlackBy);
  }

  /*
  for(int z = 0; z < BallCount; z++) {
    for (int j=0; j < BallHeight; j++)
     {
      leds[Position[z]-j] = CRGB::Black;
     }
  }
  */
  FastLED.delay(5); // will call show for us so no need to do so
}
