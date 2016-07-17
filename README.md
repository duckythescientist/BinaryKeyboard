# BinaryKeyboard
1 and 0 are the only keys a true programmer needs

Now with Unicode support! 

![The keyboard](https://i.imgur.com/pCbeucK.jpg)

[Youtube](https://youtu.be/qHAFpK-0hug) and [Youtube](https://www.youtube.com/watch?v=DmWm68NnIkk)

[Gifycat](https://gfycat.com/SophisticatedWeepyButterfly)

[Imgur Album](http://imgur.com/a/icKnR)


In a fit of inspiration, I threw this together in an evening. It is a small keyboard with three keys: `0`, `1`, and `enter/submit`.

Unicode support happened a separate day. 

## Functionality

### Mode 0
The `0` key types a "0". The `1` key types a "1". The `enter/submit` key types the enter key.

### Mode 1
This is the true binay mode. To type an ASCII letter, press a combination of 8 `0`s and `1`s (MSB first) then press `enter/submit`. If you press `enter/submit` with greater or fewer than 8 bits, this clears all of the bits. This allows you to clear screwups without having an letters actually typed. 

### NEW: Unicode Hybrid Mode (1)
As a modification to "Mode 1", you can now enter Unicode characters!!! Instead of just typing 8 bits, type anywhere from 3 to 8 nibbles before pressing `enter/submit`. Typing exactly 8 bits will fallback to the standard Mode 1.

![Unicode Support](https://i.imgur.com/tYvccpK.png)

[Gifv](https://i.imgur.com/321nCg8.gifv)

For Linux, the keyboard uses a modification of the CTRL+SHIFT+U method. It has been tested with Debian Jessie (Xfce) and partially tested with Ubuntu 14.04 (Unity).

For Windows, the keyboard uses the ALT+NUMPAD_PLUS hexadecimal method. This requires a registry edit. To `HKEY_CURRENT_USER\Control Panel\Input Method` add a string (`REG_SZ`) called `EnableHexNumpad` with the value `1` then reboot. Unfortunately, Windows doesn't seem to like anything more than 4 nibbles with this method. There are other methods, but this is the most universal for Windows. I'm very open to suggestions for ways to fix this. 



## Mode Selection:
When plugging in the USB cable, the keyboard will startup and default to Mode 0, QWERTY, Linux. 
* To switch to Mode 1, hold the `0` key when plugging in the keyboard. 
* To switch to Dvorak, hold `1` when plugging in the keyboard. 
* To switch to Windows Unicode mode, hold the `enter/submit` key when plugging in the keyboard. 

Key(s) must be held for around 7 seconds (dependent mostly on AUTO_EXIT_MS defined in the Micronucleus bootloader). 

## Construction
The body of the keyboard is 3D printed. Mine was done with a Monoprice Select Mini, but it's a small and forgiving design, so just about any printer should work.

The 4 keyswitches should be Cherry MX style. I'm using Gateron Blues. The 2u keycap needs double stems. This could be changed to fit a single stem or triple stem with stabilizers 2u keycap if the OpenSCAD files are changed. 

The controller is a "tinyDev" board of my creation. Order it from [PCBS.io](https://pcbs.io/share/8Dmor) or from [OSH Park](https://oshpark.com/shared_projects/nc7kkKFu) or elsewhere. Both PCBS.io and OSH Park are super cheap for small boards. 

The keyswitches share a common ground on one pin. The other pin connects directly to pins on the tinyDev. `0` to B1, `1` to B2, and `enter/submit` to B0. This can be easily changed in the code. 

Place the keyswitches in the top panel, solder the switches to the tinyDev, place the tinyDev into the base (add a little hot glue to keep it in place), and fit the top panel to the base (should be a snug press fit, if not, use glue). 


## Programming
The attiny85 on the tinyDev comes blank from the factory, so an ICSP such as the USB-ASP is needed to initially program it with the bootloader. A SOIC test clip helps immensely. The bootloader can be found [here](https://github.com/micronucleus/micronucleus). You may need/want to use the one that ships with the Digispark Arduino installation. 

Install the [Digispark boardstuffs](https://digistump.com/wiki/digispark/tutorials/connecting) to Arduino.