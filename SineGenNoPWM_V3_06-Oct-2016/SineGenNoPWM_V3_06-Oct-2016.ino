/*____________SineGenNoPWM__________________________________
 * 
 *  Arduino software for Spinning laser for TIRF systems. Arduino program controls galvos
 * using TimerONE library for a faster PWN than the native Arduino PWM. Speed of one cycle
 * with delayMicroseconds set to zero is approx 1.68ms.
 * 
 * Author: Kandice Lau
 * Version 2
 * Date: 18-Aug-2016
 */
#include <TimerOne.h>
#define maxSamplesNum 100

//Variables for code
const byte pinA = 9;
const byte pinB = 10;
byte index = 0;

//Array for iterating through the sine wave
static byte SineArray [maxSamplesNum] = {
      128,136,144,152,160,168,176,183,190,197,
      204,210,216,222,227,232,237,241,244,248,
      250,252,254,255,255,255,255,255,253,251,
      249,246,243,239,235,230,225,219,213,207,
      201,194,187,179,172,164,156,148,140,132,
      124,116,108,100,92,84,77,69,62,55,
      49,43,37,31,26,21,17,13,10,7,
      5,3,1,0,0,0,1,2,4,6,
      8,12,15,19,24,29,34,40,46,52,
      59,66,73,80,88,96,104,112,120,128};

//Variable Pointers
float scaleX = 4;
float scaleY = 4;
int msDelay = 0;

void setup()  { 
  Timer1.initialize(32);
  Timer1.pwm(pinA, 0);
  Timer1.pwm(pinB, 0);
  Serial.begin(9600);
}
  
void loop()  {
  for(int i=0; i<100; i++) 
  {
    //iterate to next loop
    int Ytrans = (SineArray[i]-128)*scaleY + 512;
    Timer1.setPwmDuty(pinA, Ytrans);
    if (i<75)
      index = i+24;  
    else
      index = i-74;
      
    int Xtrans = (SineArray[index]-128)*scaleX + 512;
    Timer1.setPwmDuty(pinB, Xtrans);
    
    //check for serial commands
    while(Serial.available())
      readSerial(&scaleX, &scaleY, &msDelay);
    
     delayMicroseconds(msDelay);
  }
}

void readSerial(float *scalex, float *scaley, int *msdelay)  {
  char store = 'n';
  String str = "";
  float num = 0;
  
  while (Serial.available()) {
    char nextChar = Serial.read();
    if (nextChar == 'c')  {
      Serial.println("Laser at Center, press q to quit");
   ;
      char quit = ' ';
      while (1==1){
        quit = Serial.read();
        if (quit == 'q')
          return;
      }   
    }
    else if (nextChar == 'x') {
      Serial.println("Scaling in X");
      str = "";
      while (1==1){
        store = Serial.read();
        if (isDigit(store) or store == '.')
           str += store;
        else if (store == 'q')  {
          *scalex = str.toFloat();
          return;
        }
      }
    }
    else if (nextChar == 'y') {
      Serial.println("Scaling in y");
      str = "";
      while (1==1){
        store = Serial.read();
        if (isDigit(store) or store == '.')
           str += store;
        else if (store == 'q')  {
          *scaley = str.toFloat();
          return;
        }
      }
    }
    else if (nextChar == 's') {
      Serial.println("Changing ms delay");
       str = "";
      while (1==1){
        store = Serial.read();
        if (isDigit(store) or store == '.')
           str += store;
        else if (store == 'q')  {
          *msdelay = str.toInt();
          return;
        }
      }
    }
    else{
    Serial.println(nextChar);
    return;
    }
  }
}

/*Revision Log
 * V1 17-Aug-2015
 *  New, includes code to count time for 1000 cycles
 * V2 18-Aug-2015
 *  Serial Communication to control speed, xy scaling and centrepoint of Arduino
 * V3 -6-Oct-2016
 *  Circles scale from a fixed centre point rather than from the top left corner.  
 */