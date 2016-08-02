#include <ICSC.h>
#include <digitalWriteFast.h>
#define DELAY_BLINK 25
long lastBlink;
unsigned long currentMillis;
boolean endBlink = true;
boolean ledState = false;

//STATION and POLLLING settings (most apply to master station 1)
int master = 1;
int station = 43;  // change station number here (BEFORE uploading to each node)
int FocusFlag = 0;

// Global Button constants
const int HeartBeatPin =  13;    // the number of the pin used for HeartBeat

const int FocusBtn  = 4;    // the number of the pin connected to Focus Button (B1)
const int FocusPin =  6;    // the number of the pin connected to Optocoupler Focus

const int ShutterBtn = 5;   // the number of the pin connected to Shutter Button (B1)
const int ShutterPin =  7;  // the number of the pin connected to Optocoupler Shutter

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status


void setup()
{
  /*** start first slave at address 10 to avoid problems with
        reserved ICSC library addresses in the single digits.
        DE pin is 2 on slaves. ***/
  ICSC.begin(station, 115200, &Serial, 2);

  ICSC.registerCommand('H', &HeartBeat);
  pinMode(HeartBeatPin, OUTPUT);

  ICSC.registerCommand('F', &FocusOn);
  ICSC.registerCommand('C', &FocusOff);
  pinMode(FocusBtn, INPUT);
  pinMode(FocusPin, OUTPUT);

  ICSC.registerCommand('S', &Shutter);
  pinMode(ShutterBtn, INPUT);
  pinMode(ShutterPin, OUTPUT);

  if (station == 1)
    pinMode(3, INPUT);


}

void loop()
{


  ICSC.process(); //for all nodes
  // read the state of the pushbutton value:
  if (FocusFlag == 0)
    ButtonControl(FocusBtn, FocusPin);

  ButtonControl(ShutterBtn, ShutterPin);



  //end LED update section
  //if (station == 1) {
  //  // Focus stuff
  //  ICSC.send(0, 'F', 0, NULL);
  //  if (digitalRead(3) == LOW){
  //    ICSC.send(0, 'F', 0, NULL);
  //  }
  //
  //  else{
  //  ICSC.send(0, 'C', 0, NULL);
  //  }
  //}
}



void HeartBeat(unsigned char src, char command, unsigned char len, char *data)
{


  digitalWriteFast(HeartBeatPin, HIGH);
  //  delay(100);
  digitalWriteFast(HeartBeatPin, LOW);
}

void Shutter(unsigned char src, char command, unsigned char len, char *data)
{
  digitalWriteFast(ShutterPin, HIGH);
  digitalWriteFast(HeartBeatPin, HIGH);
  delay(1);
  digitalWriteFast(ShutterPin, LOW);
  digitalWriteFast(HeartBeatPin, LOW);

}


void FocusOn(unsigned char src, char command, unsigned char len, char *data)
{
  if (FocusFlag == 0 ) {
    digitalWriteFast(FocusPin, HIGH);
    FocusFlag = 1;
  }
  //  digitalWriteFast(HeartBeatPin, HIGH);
}

void FocusOff(unsigned char src, char command, unsigned char len, char *data)
{
  if (FocusFlag == 1 ) {
    digitalWriteFast(FocusPin, LOW);
    FocusFlag = 0;
  }
}
void ButtonControl(const int buttonPin, const int ledPin) {

  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == LOW) { // usually buttonState is HIGH. Pressing the button makes buttonState low
    // turn LED on:
    digitalWriteFast(ledPin, HIGH);
    digitalWriteFast(HeartBeatPin, HIGH);
  } else {
    // turn LED off:
    digitalWriteFast(ledPin, LOW);
    digitalWriteFast(HeartBeatPin, LOW);
  }
}


