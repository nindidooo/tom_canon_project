#include <ICSC.h>

#define SLAVE_COUNT 2

long lastAction;
unsigned long currentMillis;
int actionStep = -1;
int actionDelay = 1000; //slowly cycle, so the serial monitor isn't blown out with text
boolean actionLED;

void setup()
{
  /*** master does not need DE pin on full duplex,
       it's the only driver on the downward bus,
       driver should be enabled by jumper on shield. ***/
  ICSC.begin(1, 115200, &Serial3);
  ICSC.registerCommand('P', &pingBack);
  
  Serial.begin(115200);
  Serial.println("Master Startup.");
}

void loop()
{
  ICSC.process(); //check for incoming packets from slaves
  currentMillis = millis();
  checkActions(); //sends next ICSC action, if the delay is expired
}//end main loop

void checkActions()
{
  //step -1 is for broadcast turn LED on, or turn LED off, alternating each cycle.
  //steps 0+ are for individial pings, which we expect a response.

  /*** the cycle speed is set above, i left it at 1000 so you could actually
        read the serial monitor, and see the LEDs on the slaves. ***/
  currentMillis = millis();
  if (currentMillis-lastAction>actionDelay) {
    lastAction=currentMillis;
    if (actionStep<0) {
      Serial.println();Serial.println(); //blank line in serial monitor to cleanly seperate the next cycle.
      if (!actionLED) {
        actionLED=true;  ICSC.send(0, 'H', 0, NULL); Serial.print("Broadcast LEDs ON to all slaves  >>>>>");
      }//end if actionLED not true
      else {
        actionLED=false; ICSC.send(0, 'L', 0, NULL); Serial.print("Broadcast LEDs OFF to all slaves >>>>>");
      }//end else (actionLED was true)
    }//end actionstep<0
    else { //actionStep must be >= 0, so these next steps will send individial pings to slaves
      /*** first slave is 10, so action step 0 means we directly ping id 10. ***/
      Serial.println();
      Serial.print("Direct ping to ID"); Serial.print(actionStep+10); Serial.print("---> |");
      ICSC.send(actionStep+10, 'P', 0, NULL); 
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

