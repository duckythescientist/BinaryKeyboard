#include "DigiKeyboard.h"

// Uncomment the following if you can't be bothered to patch DigiKeyboard with Dvorak support
// #define DigiKeyboard.write_dv DigiKeyboard.write

#define PIN0 (1<<1)
#define PIN1 (1<<2)
#define PINE (1<<0)

byte last_state[] = {0};
byte bmask[] = {PIN0, PIN1, PINE};
byte *my_keys;

// This ended up being unnecessary
byte qwerty_keys[] = {KEY_0, KEY_1, KEY_ENTER};
byte dvorak_keys[] = {KEY_0, KEY_1, KEY_ENTER};

unsigned long last_change[] = {0,0,0};
#define DEBOUNCE_TIME 25

byte mode = 0;
byte keymapmode = 0;

void setup() {
  DDRB &= ~(PIN0 | PIN1 | PINE);  // All inputs
  PORTB |= (PIN0 | PIN1 | PINE);  // Pullups on
  DigiKeyboard.sendKeyStroke(0);  // Wakeup the keyboard
  DigiKeyboard.delay(50);  // Always use this delay instead of the builtin delay

  if(!(PINB & PIN0)) mode = 1;  // If "0" is pressed, switch to mode 1
  if(!(PINB & PIN1))  // If "1" is pressed, switch to Dvorak
  {
    my_keys = dvorak_keys;
    keymapmode = 1;
  }
  else
  {
    my_keys = qwerty_keys;
    keymapmode = 0;;
  }
  //if(!(PINB & PINE)) mode = 3;

  //mode = 0; // Others not implemented yet
  

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

void mode_1()
{
  byte count = 0;  // How many bits have been entered
  byte sval = 0;  // Actual cumulative value for the bits
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
          if(count != 8)  // If there haven't been 8 bits entered
          {
            //DigiKeyboard.write('0' + count);
            // Clear the count and clear the bits
            sval = 0;
            count = 0;
          }
          else  // 8 bits were pressed so actually type a letter
          {
            if(keymapmode)  // Dvorak mode?
            {
              DigiKeyboard.write_dv(sval);
            }
            else 
            {
              DigiKeyboard.write(sval);
            }
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
