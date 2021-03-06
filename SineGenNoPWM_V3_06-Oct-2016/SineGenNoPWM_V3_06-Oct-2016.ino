/*____________SineGenNoPWM - Speed Edit__________________________________
 * Updated archetecture to increase the speed of scanning. It turns out that certain functions
 * (addition) greatly increase the time it takes to scan the lasers. Archetecture updated to 
 * optimize for speed
 * 
 *  Arduino software for Spinning laser for TIRF systems. Arduino program controls galvos
 * using TimerONE library for a faster PWN than the native Arduino PWM. Speed of one cycle
 * with delayMicroseconds set to zero is approx 1.68ms.
 * 
 * Author: Kandice Lau
 * Version 6-SE
 * Date: 10-Apr-2017
 * 
 * SERIAL MONITOR COMMANDS
 * ** TYPE ONLY CONTENTS OF <> FOLLOWED BY RETURN KEY
 * ** #int# #float# refers to number of specified type
 * <p>  Normal Tirf mode, <q> to quit
 * <c>  Centred mode, <q> to quit
 * <s#int#q> Change the delay between loops, increasing will slow rotation. 0 by default
 * <x#float#q> Changes scaling in x direction to #float#, default 1
 * <y#float#q> Changes scaling in y direction to #float#, default 1
 * <>#int#q> Increases shift in x direction by #int#
 * <^#int#q> Increases shift in y direction by #int#
 */
#include <TimerOne.h>
#define maxSamplesNum 100

//Variables for code
const byte pinA = 10;
const byte pinB = 9;
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

int XArray [maxSamplesNum] = {
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

int YArray [maxSamplesNum] = {
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
float scaleX = 3;
float scaleY = 3;
int centreX = 512;
int centreY = 512;
int msDelay = 0;
int elipsis = 28; //default 25 This will change the elipticity of your circle
int counter = 0;

void setup()  { 
  Timer1.initialize(32);
  Timer1.pwm(pinA, 0);
  Timer1.pwm(pinB, 0);
  Serial.begin(9600);

  updateloop();

  // Uncomment following section to check the values of the X and Y arrays
/*
  for (int i=0; i<maxSamplesNum; i++){
  Serial.println(XArray[i]);
  }
  for (int i=0; i<maxSamplesNum; i++){
  Serial.println(XArray[i]);
  } 
*/
}
  
void loop()  {
  for(int i=0; i<maxSamplesNum; i++) 
  {
    //iterate to next loop
    Timer1.setPwmDuty(pinA, YArray[i]);
    Timer1.setPwmDuty(pinB, XArray[i]); 
    
    delayMicroseconds(msDelay);
        //      check for serial commands
    }

//Uncomment following section to print system time per 1000 cycles in us
//Uncommenting this WILL pause your code slightly while printing
/*
   counter = counter + 1;
    if (counter>999){
      Serial.println(micros());
      counter = 0;
    }
*/
    while(Serial.available()){
        readSerial(&scaleX, &scaleY, &msDelay, &centreX, &centreY);
    }
}

void updateloop(){
  //Function reads stored scale and centre values and updates the arrays for X and Y
  for (int i=0; i<maxSamplesNum; i++){
  YArray[i] = (SineArray[i]-128)*scaleY + centreY;
  index = i + elipsis;  
    if (index > 99){
      index = index - 100;}
  XArray[i] = (SineArray[index]-128)*scaleX + centreX;
  } 
}
  
void readSerial(float *scalex, float *scaley, int *msdelay, int *centreX, int *centreY)  {
  char store = 'n';
  String str = "";
  float num = 0;
  
  while (Serial.available()) {
    char nextChar = Serial.read();
    if (nextChar == 'c')  {
      Timer1.setPwmDuty(pinA, *centreY);
      Timer1.setPwmDuty(pinB, *centreX);
      Serial.println("Laser at Center, press q to quit");

      char quit = ' ';
      while (1==1){
        quit = Serial.read();
        if (quit == 'q')
          return;
      }   
    }
        if (nextChar == 'p')  {
      Timer1.setPwmDuty(pinA, (128-128)*(*scaley*(0.8 + msDelay)/(1 + msDelay)) + *centreY);
      //0.8 is a factor to tune sensitivity to speed changes
      Timer1.setPwmDuty(pinB, (255-128)*(*scalex*(0.8 + msDelay)/(1 + msDelay)) + *centreX);
      Serial.println("Laser at normal TIRF, press q to quit");
 
      char quit = ' ';
      while (1==1){
        quit = Serial.read();
        if (quit == 'q')
          return;
      }   
    }
    else if (nextChar == 'x'){
      Serial.println("Scaling in X");
      str = "";
      while (1==1){
        store = Serial.read();
        if (isDigit(store) or store == '.')
           str += store;
        else if (store == 'q')  {
          *scalex = str.toFloat();
           updateloop();
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
          updateloop();
          return;
        }
      }
    }
    else if (nextChar == 's') {
      Serial.println("Changing ms delay");
       str = "";
      while (1==1){
        store = Serial.read();
        if (isDigit(store))
           str += store;
        else if (store == 'q')  {
          *msdelay = str.toInt();
          return;
        }
      }
    }
    else if (nextChar == '^') {
      Serial.println("Shifting centre Y");
       str = "";
      while (1==1){
        store = Serial.read();
        if (isDigit(store) or store == '.' or store =='-')
           str += store;
        else if (store == 'q')  {
          *centreY = *centreY + str.toInt();
          updateloop();
          return;
        }
      }
    }
    else if (nextChar == '>') {
      Serial.println("Shifting centre X");
       str = "";
      while (1==1){
        store = Serial.read();
        if (isDigit(store) or store == '.' or store == '-')
           str += store;
        else if (store == 'q')  {
          *centreX = *centreY + str.toInt();
          updateloop();
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
 * V3 6-Oct-2016
 *  Circles scale from a fixed centre point rather than from the top left corner.  
 * V4 20-Mar-2017
 *    Flipped pinouts for pinA and pinB because galvos are flipped upside down
 *    Laser is at centre position rather than top left when stopped
 *    Changed default scale to 1:1
 *    Add function to add shift centreX centreY from Serial Monitor
 * V5 20-Mar-2017
 *  Removed digit in speed input
 *  Included functionality to change to normal TIRF
 * V6 04-Apr-2017
 *  Moved serial checking to once per circle vs. once per point to increase speed of spinning
 * V6 Speed Edit
 *  Updated entire archetecture. ReadSerial now updates arrays XArray and YArray, scan speed of 
 *  code is greatly increased. From 7.61ms in V6 to 1.469ms in V6 Speed Edit
 */
