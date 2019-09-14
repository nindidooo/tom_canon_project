#include <ICSC.h>

#define SLAVE_COUNT 2

long lastAction;
unsigned long currentMillis;
int actionStep = -1;
int actionDelay = 1000; //slowly cycle, so the serial monitor isn't blown out with text
boolean actionLED;
float delays[50] = {0.1, 0.2, 0.25, 0.3333333333, 0.5, 0.6666666667, 1, 1.052631579, 1.111111111, 1.176470588, 1.25, 1.333333333, 1.428571429, 1.538461538, 1.666666667, 1.818181818, 2, 2.222222222, 2.5, 2.857142857, 3.333333333, 4, 5, 6.666666667, 10, 11.11111111, 13.33333333, 20, 20.83333333, 33.33333333, 40, 41.66666667, 41.70837504, 45.45454545, 62.5, 83.33333333, 100, 200, 500, 1000};

//STATION and POLLLING settings (most apply to master station 1)
int master = 1;
int station = 1;  // change station number here (BEFORE uploading to each node)
int stations = 6; // number of stations

// Global Button constants
const int HeartBeatPin =  13;    // the number of the pin used for HeartBeat

const int FocusBtn  = 4;    // the number of the pin connected to Focus Button (B1)
const int FocusPin =  6;    // the number of the pin connected to Optocoupler Focus

const int ShutterBtn = 5;   // the number of the pin connected to Shutter Button (B1)
const int ShutterPin =  7;  // the number of the pin connected to Optocoupler Shutter

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

const byte numLEDs = 54;
int ledPin[numLEDs]; // = {13, 12, 11, 10, 9};
int ledStatus[numLEDs];
int delayvals[50];

const byte buffSize = 4000;
//const byte buffSize = 256;
char inputBuffer[4000];
const char startMarker = '<';
const char endMarker = '>';
//const char endMarker = 'end';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;

char messageFromPC[buffSize]; // = {0};

unsigned long curMillis;

unsigned long prevReplyToPCmillis = 0;
unsigned long replyToPCinterval = 3500;
int i = 1;
long lastDebounceTime = 0;  // the last time the output pin was toggled
int lastphys_shutter_state = 0;

void setup()
{
  /*** master does not need DE pin on full duplex,
       it's the only driver on the downward bus,
       driver should be enabled by jumper on shield. ***/
  ICSC.begin(master, 115200, &Serial3);
  ICSC.registerCommand('P', &pingBack);

  Serial.begin(19200);
  Serial.println("Master Startup.");
  // flash LEDs so we know we are alive
  for (byte n = 0; n < numLEDs; n++) {
    pinMode(ledPin[n], OUTPUT);
    digitalWrite(ledPin[n], HIGH);
  }
  delay(500); // delay() is OK in setup as it only happens once

  for (byte n = 0; n < numLEDs; n++) {
    digitalWrite(ledPin[n], LOW);
  }

  // tell the PC we are ready
  Serial.print("<Arduino is ready>");

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
  Serial.print("<Matt is ready>");
}

void loop()
{

  ICSC.process(); //check for incoming packets from slaves
  //  currentMillis = millis();
  //  checkActions(); //sends next ICSC action, if the delay is expired
  int old = analogRead(A3);
  int PhysFocusStatus;

  unsigned long currentMillis = millis();
  int phys_shutter = analogRead(A5);

  //    Serial.println(analogRead(A3));
  if (((analogRead(A3) - old) != 1023) && ((old != 1023)))
    PhysFocusStatus = 0;
  //    ICSC.send(0, 'C', 0, NULL);
  else
    //    ICSC.send(0, 'F', 0, NULL);
    PhysFocusStatus = 1;
  //
  //  Serial.flush();

  getDataFromPC();
  switchLEDs();

  int FocusStatus = ledStatus[0];
  int ShutterStatus = ledStatus[1];
  int LoopVal = ledStatus[2];
  int ReverseStatus = ledStatus[3];
  int DelayVal = ledStatus[4];

  //  if (ledStatus[3] != 1) {
  getDataFromPC();
  switchLEDs();

  //  Serial.println(phys_shutter);
  // If the switch changed, due to noise or pressing:
  if (phys_shutter != lastphys_shutter_state) {
    // reset the debouncing timer
    lastDebounceTime = millis();
    getDataFromPC();
    switchLEDs();
  }


  if (phys_shutter == 1023) {

    ShutterStatus = 1;

    if (ledStatus[2] != 0) {
      ShutterStatus = 1;
    }

  }

  // BEGIN FORWARD DIRECTION ----------------------------------
  if (ShutterStatus == 1)  {

    //      else if (DelayVal != 0 && ShutterStatus == 1) {
    getDataFromPC();
    switchLEDs();

    int cam1 = 11;
    int cam50 = 60;
    //          int DelayVal = ledStatus[11-7]; // delay vals start at index 4

    if (DelayVal == -1) {
      ICSC.send(0, 'S', 1, NULL);
      ICSC.send(0, 'S', 0, NULL);
      ledStatus[1] = 0;
      ShutterStatus = 0;
    }

    else
    {

      if (ReverseStatus == 0) {
        // you can't realistically loop more than 1 time with 0 delay, so leave 0 delay outside of loopval loop
        while (LoopVal != 0) { // while LoopVal != 0


ICSC.send( 11 , 'S', 0, NULL);
ICSC.send( 12 , 'S', 0, NULL);
ICSC.send( 13 , 'S', 0, NULL);
ICSC.send( 14 , 'S', 0, NULL);
ICSC.send( 15 , 'S', 0, NULL);
ICSC.send( 16 , 'S', 0, NULL);
ICSC.send( 17 , 'S', 0, NULL);
ICSC.send( 18 , 'S', 0, NULL);
ICSC.send( 19 , 'S', 0, NULL);
ICSC.send( 20 , 'S', 0, NULL);
ICSC.send( 21 , 'S', 0, NULL);
ICSC.send( 22 , 'S', 0, NULL);
ICSC.send( 23 , 'S', 0, NULL);
ICSC.send( 24 , 'S', 0, NULL);
ICSC.send( 25 , 'S', 0, NULL);
ICSC.send( 26 , 'S', 0, NULL);
ICSC.send( 27 , 'S', 0, NULL);
ICSC.send( 28 , 'S', 0, NULL);
ICSC.send( 29 , 'S', 0, NULL);
ICSC.send( 30 , 'S', 0, NULL);
ICSC.send( 31 , 'S', 0, NULL);
ICSC.send( 32 , 'S', 0, NULL);
ICSC.send( 33 , 'S', 0, NULL);
ICSC.send( 34 , 'S', 0, NULL);
ICSC.send( 35 , 'S', 0, NULL);
ICSC.send( 36 , 'S', 0, NULL);
ICSC.send( 37 , 'S', 0, NULL);
ICSC.send( 38 , 'S', 0, NULL);
ICSC.send( 39 , 'S', 0, NULL);
ICSC.send( 40 , 'S', 0, NULL);
ICSC.send( 41 , 'S', 0, NULL);
ICSC.send( 42 , 'S', 0, NULL);
ICSC.send( 43 , 'S', 0, NULL);
ICSC.send( 44 , 'S', 0, NULL);
ICSC.send( 45 , 'S', 0, NULL);
ICSC.send( 46 , 'S', 0, NULL);
ICSC.send( 47 , 'S', 0, NULL);
ICSC.send( 48 , 'S', 0, NULL);
ICSC.send( 49 , 'S', 0, NULL);
ICSC.send( 50 , 'S', 0, NULL);
ICSC.send( 51 , 'S', 0, NULL);
ICSC.send( 52 , 'S', 0, NULL);
ICSC.send( 53 , 'S', 0, NULL);
ICSC.send( 54 , 'S', 0, NULL);
ICSC.send( 55 , 'S', 0, NULL);
ICSC.send( 56 , 'S', 0, NULL);
ICSC.send( 57 , 'S', 0, NULL);
ICSC.send( 58 , 'S', 0, NULL);
ICSC.send( 59 , 'S', 0, NULL);
ICSC.send( 60 , 'S', 0, NULL);

          
//          for (i = cam1; i <= cam50; i++) {
////
////                          Serial.print("<DelayVal_");
////                          Serial.print(DelayVal);
////                          Serial.print(i-10);
////                          Serial.print(" = ");
////                          Serial.print(ledStatus[i-7]);
////                          Serial.print(">");
////                          delay(100);
//
//            ICSC.send(i, 'S', 0, NULL); // Shutter i
//
//            if (DelayVal != 0)
//            {
//            if (DelayVal == 1)
//              _delay_us(10);
//            else if (DelayVal == 2)
//                delayMicroseconds(200);
//            else if (DelayVal == 3)
//                delayMicroseconds(250);
//            else if (DelayVal == 4)
//                delayMicroseconds(0.3333333333 * 1000);
//            else if (DelayVal == 5)
//                delayMicroseconds(0.5 * 1000);
//            else if (DelayVal == 6)
//                delayMicroseconds(0.6666666667 * 1000);
//            else if (DelayVal == 7)
//                delayMicroseconds(1 * 1000);
//            else if (DelayVal == 8)
//                delayMicroseconds(1.052631579 * 1000);
//            else if (DelayVal == 9)
//                delayMicroseconds(1.111111111 * 1000);
//            else if (DelayVal == 10)
//                delayMicroseconds(1.176470588 * 1000);
//            else if (DelayVal == 11)
//                delayMicroseconds(1.25 * 1000);
//            else if (DelayVal == 12)
//                delayMicroseconds(1.333333333 * 1000);
//            else if (DelayVal == 13)
//                delayMicroseconds(1.428571429 * 1000);
//            else if (DelayVal == 14)
//                delayMicroseconds(1.538461538 * 1000);
//            else if (DelayVal == 15)
//                delayMicroseconds(1.666666667 * 1000);
//            else if (DelayVal == 16)
//                delayMicroseconds(1.818181818 * 1000);
//            else if (DelayVal == 17)
//                delayMicroseconds(2.0 * 1000);
//            else if (DelayVal == 18)
//                delayMicroseconds(2.222222222 * 1000);
//            else if (DelayVal == 19)
//                delayMicroseconds(2.5 * 1000);
//            else if (DelayVal == 20)
//                delayMicroseconds(2.857142857 * 1000);
//            else if (DelayVal == 21)
//                delayMicroseconds(3.333333333 * 1000);
//            else if (DelayVal == 22)
//                delayMicroseconds(4 * 1000);
//            else if (DelayVal == 23)
//                delayMicroseconds(5 * 1000);
//            else if (DelayVal == 24)
//                delayMicroseconds(6.666666667 * 1000);
//            else if (DelayVal == 25)
//                delayMicroseconds(10 * 1000);
//            else if (DelayVal == 26)
//                delayMicroseconds(11.11111111 * 1000);
//            else if (DelayVal == 27)
//                delayMicroseconds(13.33333333 * 1000);
//            else if (DelayVal == 28)
//                delay(20.0);
//            else if (DelayVal == 29)
//                delay(20.83333);
//            else if (DelayVal == 30)
//                delay(33.33333);
//            else if (DelayVal == 31)
//                delay(40.0);
//            else if (DelayVal == 32)
//                delay(41.66666667);
//            else if (DelayVal == 33)
//                delay(41.70837504);
//            else if (DelayVal == 34)
//                delay(45.45454545);
//            else if (DelayVal == 35)
//                delay(62.5);
//            else if (DelayVal == 36)
//                delay(83.33333333);
//            else if (DelayVal == 37)
//                delay(100);
//            else if (DelayVal == 38)
//                delay(200);
//            else if (DelayVal == 39)
//                delay(500);
//            else if (DelayVal == 40)
//                delay(1000);
//            }
//
//            ledStatus[1] = 0;         // shutter OFF
//
//            FocusStatus = ledStatus[0];
//            ShutterStatus = ledStatus[1];
//            //            Serial.print("<FocusStatus = ");
//            //            Serial.print(FocusStatus);
//            //            Serial.print(" ledStatus[0]  ");
//            //            Serial.print(ledStatus[0] );
//            //            Serial.print(">");
//
//
//          }
          if (LoopVal >= 0)
            LoopVal = LoopVal - 1; // decrement LoopVal
        }
      }

      // BEGIN REVERSE MODE
      else if (ReverseStatus == 1) {  // REVERSE MODE
        // you can't realistically loop more than 1 time with 0 delay, so leave 0 delay outside of loopval loop
        while (LoopVal != 0) { // while LoopVal != 0
          for (int i = cam50; i >= cam1; i--) {

            //              Serial.print("<DelayVal_");
            //              Serial.print(i-10);
            //              Serial.print(" = ");
            //              Serial.print(ledStatus[i-7]);
            //              Serial.print(">");

            ICSC.send(i, 'S', 0, NULL); // Shutter i

            if (DelayVal != 0)
            {

              if (DelayVal == 1)
                delayMicroseconds(0.8191998424 * 1 * 1000);
              else if (DelayVal == 2)
                delayMicroseconds(0.9103046487 * 2 * 1000);
              else if (DelayVal == 3)
                delayMicroseconds(0.9484942339 * 3 * 1000);
              else if (DelayVal == 4)
                delayMicroseconds(0.9593908629 * 4 * 1000);
              else if (DelayVal == 5)
                delayMicroseconds(0.9659639437 * 5 * 1000);
              else if (DelayVal == 6)
                delayMicroseconds(0.9678779935 * 6 * 1000);
              else if (DelayVal == 7)
                delayMicroseconds(0.9722222222 * 7 * 1000);
              else if (DelayVal == 8)
                delayMicroseconds(0.971659919 * 8 * 1000);
              else if (DelayVal == 9)
                delayMicroseconds(0.9791208791 * 9 * 1000);
              else
                delay(ledStatus[i - 7]);
            }

            ledStatus[1] = 0;         // shutter OFF

            FocusStatus = ledStatus[0];
            ShutterStatus = ledStatus[1];
            //            Serial.print("<FocusStatus = ");
            //            Serial.print(FocusStatus);
            //            Serial.print(" ledStatus[0]  ");
            //            Serial.print(ledStatus[0] );
            //            Serial.print(">");


          }
          if (LoopVal >= 0)
            LoopVal = LoopVal - 1; // decrement LoopVal
        }
      }

      else if (ReverseStatus == 2) {  // BOUNCE BACK MODE V1
        // you can't realistically loop more than 1 time with 0 delay, so leave 0 delay outside of loopval loop
        while (LoopVal != 0) { // while LoopVal != 0


          // GO FORWARD FIRST
          for (int i = cam1; i <= cam50; i++) {

//            Serial.print("<BOUNCEBACK V1 FORWARD DelayVal_");
//            Serial.print(i - 10);
//            Serial.print(" = ");
//            Serial.print(ledStatus[i - 7]);
//            Serial.print(">");

            ICSC.send(i, 'S', 0, NULL); // Shutter i

            if (DelayVal != 0)
            {

              if (DelayVal == 1)
                delayMicroseconds(0.8191998424 * 1 * 1000);
              else if (DelayVal == 2)
                delayMicroseconds(0.9103046487 * 2 * 1000);
              else if (DelayVal == 3)
                delayMicroseconds(0.9484942339 * 3 * 1000);
              else if (DelayVal == 4)
                delayMicroseconds(0.9593908629 * 4 * 1000);
              else if (DelayVal == 5)
                delayMicroseconds(0.9659639437 * 5 * 1000);
              else if (DelayVal == 6)
                delayMicroseconds(0.9678779935 * 6 * 1000);
              else if (DelayVal == 7)
                delayMicroseconds(0.9722222222 * 7 * 1000);
              else if (DelayVal == 8)
                delayMicroseconds(0.971659919 * 8 * 1000);
              else if (DelayVal == 9)
                delayMicroseconds(0.9791208791 * 9 * 1000);
              else
                delay(ledStatus[i - 7]);
            }

            ledStatus[1] = 0;         // shutter OFF

            FocusStatus = ledStatus[0];
            ShutterStatus = ledStatus[1];
            //            Serial.print("<FocusStatus = ");
            //            Serial.print(FocusStatus);
            //            Serial.print(" ledStatus[0]  ");
            //            Serial.print(ledStatus[0] );
            //            Serial.print(">");


          }


          // NOW GO BACKWARDS
          for (int i = cam50-1; i > cam1; i--) { // START BB V1 REVERSE AT CAM49 and end at CAM 2

//            Serial.print("<BOUNCEBACK V1 REVERSE DelayVal_");
//            Serial.print(i - 10);
//            Serial.print(" = ");
//            Serial.print(ledStatus[i - 7]);
//            Serial.print(">");

            ICSC.send(i, 'S', 0, NULL); // Shutter i

            if (DelayVal != 0)
            {

              if (DelayVal == 1)
                delayMicroseconds(0.8191998424 * 1 * 1000);
              else if (DelayVal == 2)
                delayMicroseconds(0.9103046487 * 2 * 1000);
              else if (DelayVal == 3)
                delayMicroseconds(0.9484942339 * 3 * 1000);
              else if (DelayVal == 4)
                delayMicroseconds(0.9593908629 * 4 * 1000);
              else if (DelayVal == 5)
                delayMicroseconds(0.9659639437 * 5 * 1000);
              else if (DelayVal == 6)
                delayMicroseconds(0.9678779935 * 6 * 1000);
              else if (DelayVal == 7)
                delayMicroseconds(0.9722222222 * 7 * 1000);
              else if (DelayVal == 8)
                delayMicroseconds(0.971659919 * 8 * 1000);
              else if (DelayVal == 9)
                delayMicroseconds(0.9791208791 * 9 * 1000);
              else
                delay(ledStatus[i - 7]);
            }

            ledStatus[1] = 0;         // shutter OFF

            FocusStatus = ledStatus[0];
            ShutterStatus = ledStatus[1];
            //            Serial.print("<FocusStatus = ");
            //            Serial.print(FocusStatus);
            //            Serial.print(" ledStatus[0]  ");
            //            Serial.print(ledStatus[0] );
            //            Serial.print(">");


          }
          if (LoopVal >= 0)
            LoopVal = LoopVal - 1; // decrement LoopVal
        }
        ICSC.send(cam1, 'S', 0, NULL); // Shutter cam 1 final time
        
//        Serial.print("<FINAL BOUNCEBACK V1 REVERSE DelayVal_");
//        Serial.print(1);
//        Serial.print(" = ");
//        Serial.print(1);
//        Serial.print(">");

      }

      else if (ReverseStatus == 3) {  // BOUNCE BACK MODE V2
        // you can't realistically loop more than 1 time with 0 delay, so leave 0 delay outside of loopval loop
        while (LoopVal != 0) { // while LoopVal != 0


          // GO FORWARD FIRST
          for (int i = cam1; i <= cam50; i++) {

//            Serial.print("<BOUNCEBACK V2 FORWARD DelayVal_");
//            Serial.print(i - 10);
//            Serial.print(" = ");
//            Serial.print(ledStatus[i - 7]);
//            Serial.print(">");


            ICSC.send(i, 'S', 0, NULL); // Shutter i

            if (DelayVal != 0)
            {

              if (DelayVal == 1)
                delayMicroseconds(0.8191998424 * 1 * 1000);
              else if (DelayVal == 2)
                delayMicroseconds(0.9103046487 * 2 * 1000);
              else if (DelayVal == 3)
                delayMicroseconds(0.9484942339 * 3 * 1000);
              else if (DelayVal == 4)
                delayMicroseconds(0.9593908629 * 4 * 1000);
              else if (DelayVal == 5)
                delayMicroseconds(0.9659639437 * 5 * 1000);
              else if (DelayVal == 6)
                delayMicroseconds(0.9678779935 * 6 * 1000);
              else if (DelayVal == 7)
                delayMicroseconds(0.9722222222 * 7 * 1000);
              else if (DelayVal == 8)
                delayMicroseconds(0.971659919 * 8 * 1000);
              else if (DelayVal == 9)
                delayMicroseconds(0.9791208791 * 9 * 1000);
              else
                delay(ledStatus[i - 7]);
            }

            ledStatus[1] = 0;         // shutter OFF

            FocusStatus = ledStatus[0];
            ShutterStatus = ledStatus[1];
            //            Serial.print("<FocusStatus = ");
            //            Serial.print(FocusStatus);
            //            Serial.print(" ledStatus[0]  ");
            //            Serial.print(ledStatus[0] );
            //            Serial.print(">");


          }


          // NOW GO BACKWARDS
          for (int i = cam50; i >= cam1; i--) { // START AT CAM 50 IN BB V2

//            Serial.print("<BOUNCEBACK V2 REVERSE DelayVal_");
//            Serial.print(i - 10);
//            Serial.print(" = ");
//            Serial.print(ledStatus[i - 7]);
//            Serial.print(">");
//            delay(100);

            ICSC.send(i, 'S', 0, NULL); // Shutter i

            if (DelayVal != 0)
            {

              if (DelayVal == 1)
                delayMicroseconds(0.8191998424 * 1 * 1000);
              else if (DelayVal == 2)
                delayMicroseconds(0.9103046487 * 2 * 1000);
              else if (DelayVal == 3)
                delayMicroseconds(0.9484942339 * 3 * 1000);
              else if (DelayVal == 4)
                delayMicroseconds(0.9593908629 * 4 * 1000);
              else if (DelayVal == 5)
                delayMicroseconds(0.9659639437 * 5 * 1000);
              else if (DelayVal == 6)
                delayMicroseconds(0.9678779935 * 6 * 1000);
              else if (DelayVal == 7)
                delayMicroseconds(0.9722222222 * 7 * 1000);
              else if (DelayVal == 8)
                delayMicroseconds(0.971659919 * 8 * 1000);
              else if (DelayVal == 9)
                delayMicroseconds(0.9791208791 * 9 * 1000);
              else
                delay(ledStatus[i - 7]);
            }

            ledStatus[1] = 0;         // shutter OFF

            FocusStatus = ledStatus[0];
            ShutterStatus = ledStatus[1];
            //            Serial.print("<FocusStatus = ");
            //            Serial.print(FocusStatus);
            //            Serial.print(" ledStatus[0]  ");
            //            Serial.print(ledStatus[0] );
            //            Serial.print(">");


          }
          if (LoopVal >= 0)
            LoopVal = LoopVal - 1; // decrement LoopVal
        }

      }



    }
      ledStatus[1] = 0;
      ShutterStatus = 0;
  }
  // END shutter stuff ---------------

  // Focus stuff

  if (station == 1) {
    getDataFromPC();
    switchLEDs();
    if (PhysFocusStatus == 1)
      FocusStatus = 1;


    if (FocusStatus == 1) { // || PhysFocusStatus == 1) { //digitalRead(3) == LOW ||
      ICSC.send(0, 'F', 0, NULL);
      digitalWrite(13, HIGH);
    }

    else if (FocusStatus == 0) {
      ICSC.send(0, 'C', 0, NULL);
      digitalWrite(13, LOW);
    }

  }

  currentMillis = millis();
  lastphys_shutter_state = phys_shutter;



}//end main loop

void checkActions()
{
  //step -1 is for broadcast turn LED on, or turn LED off, alternating each cycle.
  //steps 0+ are for individial pings, which we expect a response.

  /*** the cycle speed is set above, i left it at 1000 so you could actually
        read the serial monitor, and see the LEDs on the slaves. ***/
  currentMillis = millis();
  if (currentMillis - lastAction > actionDelay) {
    lastAction = currentMillis;
    if (actionStep < 0) {
      Serial.println(); Serial.println(); //blank line in serial monitor to cleanly seperate the next cycle.
      if (!actionLED) {
        actionLED = true;  ICSC.send(0, 'H', 0, NULL); Serial.print("Broadcast LEDs ON to all slaves  >>>>>");
      }//end if actionLED not true
      else {
        actionLED = false; ICSC.send(0, 'L', 0, NULL); Serial.print("Broadcast LEDs OFF to all slaves >>>>>");
      }//end else (actionLED was true)
    }//end actionstep<0
    else { //actionStep must be >= 0, so these next steps will send individial pings to slaves
      /*** first slave is 10, so action step 0 means we directly ping id 10. ***/
      Serial.println();
      Serial.print("Direct ping to ID"); Serial.print(actionStep + 10); Serial.print("---> |");
      ICSC.send(actionStep + 10, 'P', 0, NULL);
    }//end else (actionStep NOT < 0)
    actionStep++;
    if (actionStep >= SLAVE_COUNT) {
      actionStep = -1;
    } //no more slaves, reset to the broadcast ping (-1)
  }//end if actionDelay expired
}//end void checkActions

void pingBack(unsigned char src, char command, unsigned char len, char *data)
{
  Serial.print("| <---Response from ID"); Serial.print(src);
}



//=============

void getDataFromPC() {

  // receive data from PC and save it into inputBuffer

  if (Serial.available() > 0) {

    char x = Serial.read();
    //    Serial.print("<inputBuffer = ");
    //    Serial.print(inputBuffer);
    //    Serial.print(">");

    // the order of these IF clauses is significant

    if (x == endMarker) {
      readInProgress = false;
      newDataFromPC = true;
      inputBuffer[bytesRecvd] = 0;
      bytesRecvd = 0;
      parseData();
    }

    if (readInProgress) {
      inputBuffer[bytesRecvd] = x;
      bytesRecvd ++;
      if (bytesRecvd == buffSize) {
        bytesRecvd = buffSize - 1;
      }
    }

    if (x == startMarker) {
      bytesRecvd = 0;
      readInProgress = true;
    }
  }
}

//=============

void parseData() {

  // split the data into its parts
  // assumes the data will be received as (eg) 0,1,35

  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(inputBuffer, ","); // get the first part
  ledStatus[0] = atoi(strtokIndx); //  convert to an integer
  //  Serial.print("<MATT ledStatus[");
  //  Serial.print(0);
  //  Serial.print("]");
  //  Serial.print(ledStatus[0]);
  //  Serial.print(">");

  // delay obtaining section
  for (int i = 1; i <= 53; i++) { // number of elements in incoming string is 53
    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    ledStatus[i] = atoi(strtokIndx);
    if (i == 53) {
      if (ledStatus[52] == 1000) {
        if (ledStatus[4] == 1000) {
          if (ledStatus[53] == 10)
            ledStatus[53] = 1000;
        }
      }
    }


    //    Serial.print("<MATT ledStatus[");
    //    Serial.print(i);
    //    Serial.print("]");
    //    Serial.print(ledStatus[i]);
    //    Serial.print(">");
  }


}

//=============

void replyToPC() {

  if (newDataFromPC) {
    newDataFromPC = false;
    Serial.print("<LedA ");
    Serial.print(ledStatus[0]),
                 Serial.print(" LedB ");
    Serial.print(ledStatus[1]);
    Serial.print(" Time ");
    Serial.print(curMillis >> 9); // divide by 512 is approx = half-seconds
    Serial.println(">");
    Serial.flush();
  }
}

//=============

void switchLEDs() {

  for (byte n = 0; n <= numLEDs; n++) {

    digitalWrite( ledPin[n], ledStatus[n]);

  }
}


//=============

void sendToPC() {
  if (curMillis - prevReplyToPCmillis >= replyToPCinterval) {
    prevReplyToPCmillis += replyToPCinterval;
    int valForPC = curMillis >> 9; // approx half seconds
    Serial.print('<');
    Serial.print(valForPC);
    Serial.print('>');
    Serial.flush();

  }

}


void HeartBeat(unsigned char src, char command, unsigned char len, char *data)
{

  digitalWrite(HeartBeatPin, HIGH);
  digitalWrite(HeartBeatPin, LOW);
}

void Shutter(unsigned char src, char command, unsigned char len, char *data)
{
  digitalWrite(ShutterPin, HIGH);
  digitalWrite(HeartBeatPin, HIGH);
  //  delay(250);
  digitalWrite(ShutterPin, LOW);
  digitalWrite(HeartBeatPin, LOW);
}


void FocusOn(unsigned char src, char command, unsigned char len, char *data)
{

  digitalWrite(FocusPin, HIGH);

}

void FocusOff(unsigned char src, char command, unsigned char len, char *data)
{

  digitalWrite(FocusPin, LOW);

}
void ButtonControl(const int buttonPin, const int ledPin) {

  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == LOW) { // usually buttonState is HIGH. Pressing the button makes buttonState low
    // turn LED on:
    digitalWrite(ledPin, HIGH);
    digitalWrite(HeartBeatPin, HIGH);
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
    digitalWrite(HeartBeatPin, LOW);
  }
}
