#include <ICSC.h>

#define DELAY_BLINK 25
long lastBlink;
unsigned long currentMillis;
boolean endBlink=true;
boolean ledState=false;

void setup()
{
  /*** start first slave at address 10 to avoid problems with
        reserved ICSC library addresses in the single digits.
        DE pin is 2 on slaves. ***/
  ICSC.begin(10, 115200, &Serial, 2);
  ICSC.registerCommand('H', &ledHIGH); //master broadcast sets LED high solid
  ICSC.registerCommand('L', &ledLOW); //master broadcast sets LED Low solid
  ICSC.registerCommand('P', &pingDirect); //direct packet causes LED to blink/flicker depending on it's current state.
  //the blink is so you can see the direct packet to each slave.
  //if the last broadcast was LED is ON, it will flicker off briefly; or if it's OFF, it will blink on briefly.
  pinMode(13, OUTPUT);
  ledState=false;
}

void loop()
{
  /*** Check for incoming ICSC packets, stations
        won't recieve unless this is called frequently. ***/
  ICSC.process();
  currentMillis = millis();
  undoBlink();
  digitalWrite(13, ledState);
}

void ledHIGH(unsigned char src, char command, unsigned char len, char *data)
{
  ledState=true;
}

void ledLOW(unsigned char src, char command, unsigned char len, char *data)
{
  ledState=false;
}

void pingDirect(unsigned char src, char command, unsigned char len, char *data)
{
  ICSC.send(1, 'P', 0, NULL); //respond to master direct ping
  endBlink=false;
  lastBlink = millis(); //set the clock for the flicker/blink
  if (ledState) {ledState=false;} //if it's on, briefly kill the LED for now...
  else {ledState=true;} //if it's OFF, blink the led on for now...
}

void undoBlink() 
{
  //this undoes(finishes) the flicker or blink caused by the master direct ping.
  if (!endBlink){ //set false by ping direct only!
    if (currentMillis-lastBlink>DELAY_BLINK){ //has the blink/flicker lasted long enough?
      endBlink=true;
      if (ledState) {ledState=false;} //if it's on, kill the LED for good. (undo the blink)
      else {ledState=true;} //if it's OFF, turn the LED on for good. (undo the flicker)
    }//end if blink/flicker delay has lapsed
  }//end if endBlink false
}


