#include "DigiKeyboard.h"

// Uncomment the following if you can't be bothered to patch DigiKeyboard with Dvorak support
// #define DigiKeyboard.write_dv DigiKeyboard.write

#define PIN0 (1<<1)
#define PIN1 (1<<2)
#define PINE (1<<0)

byte last_state[] = {0};
byte bmask[] = {PIN0, PIN1, PINE};
byte *my_keys;

byte qwerty_keys[] = {KEY_0, KEY_1, KEY_ENTER, 24};  // 0,1,ENTER,u
byte dvorak_keys[] = {KEY_0, KEY_1, KEY_ENTER, 9}; 

byte * my_numpad;
byte qwerty_numpad_keys[] = {0x62, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 4,5,6,7,8,9};  // numpad 0-9 a-f
byte dvorak_numpad_keys[] = {0x62, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x04,0x11,0x0c,0x0b,0x07,0x1C};  // numpad 0-9 a-f

unsigned long last_change[] = {0,0,0};
#define DEBOUNCE_TIME 25

byte mode = 0;
byte keymapmode = 0;
byte iswindows = 0;

void setup() {
  DDRB &= ~(PIN0 | PIN1 | PINE);  // All inputs
  PORTB |= (PIN0 | PIN1 | PINE);  // Pullups on
  DigiKeyboard.sendKeyStroke(0);  // Wakeup the keyboard
  DigiKeyboard.delay(50);  // Always use this delay instead of the builtin delay

  if(!(PINB & PIN0)) mode = 1;  // If "0" is pressed, switch to mode 1
  if(!(PINB & PIN1))  // If "1" is pressed, switch to Dvorak
  {
    my_keys = dvorak_keys;
    my_numpad = dvorak_numpad_keys;
    keymapmode = 1;
  }
  else
  {
    my_keys = qwerty_keys;
    my_numpad = qwerty_numpad_keys;
    keymapmode = 0;;
  }

  if(!(PINB & PINE)) iswindows = 1;


  while((PINB & (PIN0 | PIN1 | PINE)) != (PIN0 | PIN1 | PINE))
    DigiKeyboard.delay(100);  // Wait for switches to be idle
  

}

byte debounce()  // return mask for changed buttons
{
  byte ret = 0;
  for(byte i=0; i<3; i++)
  {
    if((PINB & bmask[i]) != last_state[i])
    {
      if(millis() - last_change[i] > DEBOUNCE_TIME)
      {
        last_state[i] = PINB & bmask[i];
        last_change[i] = millis();
        ret |= (1<<i);
      }
    }
  }
  return ret;
}

void mode_0()
{
  byte r = debounce();
  if(r)  // If any key is pressed
  {
    for(byte i=0; i<3; i++)  // For each key
    {
      if(r & (1<<i))  // If this key is pressed
      {
        if(!last_state[i])  // And it wasn't pressed last time we checked
        {
          // Send the corresponding letter
          DigiKeyboard.sendKeyStroke(my_keys[i]);
        }
      }
    }
  }
  else
  {
    //DigiKeyboard.sendKeyStroke(0);
    DigiKeyboard.delay(10);
  }
}


void write_unicode(unsigned long sval, byte count)
{
  // sanity check
  if(count < 8 || (count % 4))
    return;

  // For standard ASCII, just type the letter the simple way
  if(count == 8 && sval < 0x80)
  {
    if(keymapmode)
    {
      DigiKeyboard.write_dv(byte(sval & 0x7F));
    }
    else
    {
      DigiKeyboard.write(byte(sval & 0x7F));
    }
  }
  else
  {

    // Windows Unicode method:
    // HKEY_CURRENT_USER\Control Panel\Input Method contains a string type (REG_SZ) value called EnableHexNumpad, which has the value data 1
    // Note, Windows seems to only type 2-byte symbols.
    // There are other methods that may have more symbols, but this is more universal.
    // Hold ALT, type numpad_plus, type hexadecimal value, release alt

    // Linux Unicode method:
    // Note, this may be X specific or application specific.
    // I've had success in Ubuntu 14.04 with Unity (Chrome, Guake, Gedit, Sublime Text 3)
    // Type CTRL+[ALT]+SHIFT+U and release, type hexadecimal value (%x not %8x), type space 
    // Some applications need the ALT, and some don't. It seems to work to do both.


    byte charbuf[10];
    byte charcount = 0;

  /*
    // Convert to (reversed) decimal
    // Not needed because of Windows registry edit
    while(sval > 0)
    {
      byte decval = sval % 10;
      charbuf[charcount] = numpad_keys[decval];
      charcount += 1;
      sval /= 10;
    }
  */

    // Convert to (reversed) hexadecimal
    while(count > 0)
    {
      byte hexval = byte(sval&0xF);  // Get the low nibble
      charbuf[charcount] = my_numpad[hexval];  // Save hexadecimal nibble as ascii letter
      charcount += 1;
      count -= 4;
      sval >>=4;
    }


    if(iswindows)
    {
      DigiKeyboard.sendKeyPress(0x57, MOD_ALT_LEFT);  // NUMPAD_PLUS

      // Write out charbuf backward
      while(charcount>0)
      {
        DigiKeyboard.sendKeyPress(charbuf[--charcount], MOD_ALT_LEFT);
        // DigiKeyboard.sendKeyStroke(charbuf[--charcount]);  // debug
      }

      DigiKeyboard.sendKeyPress(0);  // release alt (everything)
    }
    else
    {
      // CTRL+ALT+SHIFT+U
      DigiKeyboard.sendKeyStroke(my_keys[3], MOD_CONTROL_LEFT | MOD_SHIFT_LEFT | MOD_ALT_LEFT);
      
      // CTRL+SHIFT+U
      DigiKeyboard.sendKeyStroke(my_keys[3], MOD_CONTROL_LEFT | MOD_SHIFT_LEFT);


      // Write out charbuf backward
      while(charcount>0)
      {
        DigiKeyboard.sendKeyStroke(charbuf[--charcount]);
      }

      DigiKeyboard.sendKeyStroke(KEY_SPACE);
    }

  }

}

void mode_1()
{
  byte count = 0;  // How many bits have been entered
  unsigned long sval = 0;  // Actual cumulative value for the bits
  while(1)
  {
    DigiKeyboard.delay(10);
    byte r = debounce();
    if(r)  // If any key has been pressed
    {
      if(r & (1<<2))  // If "submit" has been pressed
      {
        if(!last_state[2])  // And it wasn't previously pressed
        {
          if(count < 8 || (count % 4))  // Bad number of bits for character
          {
            //DigiKeyboard.write('0' + count);
            // Clear the count and clear the bits
            sval = 0;
            count = 0;
          }
          else  // good number of bits were pressed so actually type a letter
          {
            write_unicode(sval, count);
            sval = 0;
            count = 0;
          }
        }
      }
      else  // other key pressed
      {
        if((r & (1<<1)) && !last_state[1])  // "1" pressed
        {
          sval <<= 1;
          count ++;
          sval |= 1;
        }
        else if((r & (1<<0)) && !last_state[0])  // "0" pressed
        {
          sval <<= 1;
          count ++;
        }
      }
    }
  }
}

void loop() {
  switch(mode)
  {
    case 0:
      mode_0();
      break;
    case 1:
      mode_1();
      break;
    default:
      mode_0();
      break;
  }

}
