# Notes from Adrian

## Part 1

I know you have other projects ahead of this in the queue but I thought I’d send some details re. the LED lights that Don and I have built. I’ll send the code later today hopefully. This will be more a collection of thoughts than anything else but I’m sure you’ll get the gist.

Probably not relevant to you, but our lights are based on the [Ikea Vidja floor lamp](https://www.ikea.com/gb/en/p/vidja-floor-lamp-white-80309203/).

We’re both using [WS2815 LED strips](./Datasheets/WS2815%20LED%20Datasheet.pdf) (apologies – I think I previously told you WS2812. Data sheet is attached. We’re using the 12V version and ours are obviously the non-waterproof version. A guide to the [“IP” ratings for dust and water ingress can be found here](https://uk.rs-online.com/web/generalDisplay.html?id=ideas-and-advice/ip-ratings): Don went for the 144 LED per metre version, mine is the 60 LEDs per metre.

We’re both using the ESP8266 dev boards as controllers. We bring a 12V supply into the light and then drop it down to 5v for the dev board with a [buck converter similar to this](https://www.amazon.co.uk/Youmile-LM2596S-Converter-Efficiency-Regulator/dp/B07ZCRTMXK). The data line on the led strips is fine being driven at 3.3v directly from the GPIO pin of the ESP8266. A 5v signal would be equally good if you use a different microcontroller. The power supply will obviously need to be chosen to be suitable for the LED strips you use.

I know you’ve already been looking at different form factors/effects so I won’t send you tons of links. Having said that, I think my next project will be a [lava lamp similar to this](https://www.youtube.com/watch?v=64X5sJJ4YKM)

Most of the effects in the code I’ll send have come from one of three places:

- The FastLED examples library

- A bunch of [non-sound related effects here](https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/)

- The sound related effects have mostly come from the code linked in the comments of [this video](https://www.youtube.com/watch?v=YVuYO014h0M) (note that the code here is a mess – I’ve done a fair bit of tidying up on it)

On the sound related effects, the ESP8266 dev board has a single ADC which accepts voltages from 0 to 1V. There’s a voltage divider on the dev board already to cater for the fact that the chip is powered at 3.3v. The microphone board I gave you is a [MAX9814 module](https://www.amazon.co.uk/gp/product/B093SJ5X48/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1). I tried others but they were very noisy – this board gave me the best results. It has a variable gain on it. Can’t remember which gain setting I’m using but, if you need me to at some point, I’ll open up my light and take a look.

## Part 2

I forgot to mention earlier that FastLED doesn’t have a “LED Type” for the WS2815. However, since the LED Type basically just determines the protocol and pulse widths etc. for the data signal to drive the strips, we use a LED type of WS2812B which works just fine.

As discussed, we’ve been using the Arduino IDE to build and upload the code. It’s nice because it supports OTA programming of the ESP8266 which means we don’t have to keep opening up the light and connecting a USB cable every time we want to reprogram the effects. I’m sure other IDEs support this too.

I’m not going to bother talking you through the code – I doubt that’s necessary. Just give me a shout if you’re curious about anything when you get to look at it.

The only other thing really is to go through the “Settings” page. As you’ve seen, the code serves a couple of pages to trigger the effects and to change some of the settings. It’s worth going through some of the settings briefly:

- **Ball Height**: Determines the height (in LEDs) of each ball in the bouncing ball effect

- **Ball Fade to Black**: Determines the “tail length” of the balls in the bouncing ball effect

- **Input Floor/Input Ceiling**: Allows for a kind of “programmable gain” on the microphone input. Consider that the microphone board is set to a particular (fixed) gain level and tucked away inside the light. One night you might be having a raucous party with music blaring and the next you might be chilling at home with background music on. Because the effects triggered by sound are basically working on the volume of the sound, we want to be able to adjust the sensitivity of the microphone. The ADC on the ESP8266 is a 10 bit ADC. Output values are therefore from 0 to 1023. The “Input Floor” setting allows us to basically ignore any readings below a certain value. I have this set to 30 which avoids the lights “twitching” at extremely low sound levels. The “Input Ceiling” basically determines what will be considered the ADC reading representing the maximum volume. I have mine set to 600. With the values stated, the code then basically scales ADC readings 30-600 back to the 0-1023 range. So, in this way, setting “Input Ceiling” to a lower value makes the lights more sensitive to volume whereas setting to a higher value makes the lights less sensitive to volume. I hope that all makes sense.

- **Sample Window**: The number of milliseconds to sample the sound for. We sample for this period, and calculate a peak to peak value (max reading – min reading) which we use as the ADC reading. This, together with the rolling average mechanism, prevents the lights twitching too much when being controlled by the microphone.

- **Rolling Average Count**: The number of ADC samples to average to smooth the light effects.

- **Peak hang Time/Peak Fall Rate**: Where the sound effect has a peak dot, these determine the amount of time the dot hangs at the peak and the rate at which it falls back down afterwards.

- **Draw Max**: Related to “Shatter” effect. Not spent much time working out how this works but basically it affects the sensitivity of the effect.

If I’m honest the “Vu_Meter_final” code module started life basically as the one downloaded from the link previously sent. It was a terrible mess. It’s slightly less of a mess now but still not anywhere near “tidy”. Sorry about that.