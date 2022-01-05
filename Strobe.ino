void doStrobe(int FlashDelay)
{
  FastLED.showColor(CRGB::White);
  delay(FlashDelay);
  FastLED.showColor(CRGB::Black);
  delay(FlashDelay);
}
