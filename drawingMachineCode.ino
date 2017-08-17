/* Arduino Pro Micro Drawing Machine Code (Continuous Line Drawing Machine - Super Make Something Episode 12) - https://youtu.be/-qeD2__yK4c
 * by: Alex - Super Make Something
 * date: February 24th, 2017
 * license: Creative Commons - Attribution - Non-Commercial.  More information available at: http://creativecommons.org/licenses/by-nc/3.0/
 */
 
 // Includes derivative of "ReadWrite" by David A. Mellis and Tom Igoe available at: https://www.arduino.cc/en/Tutorial/ReadWrite
 // Includes derivative of EasyDriver board sample code by Joel Bartlett available at: https://www.sparkfun.com/tutorials/400

/*
 * This code contains the follow functions:
 * - void setup(): initializes Serial port, SD card
 * - void loop(): main loop
 * - void rotateMotors(): rotates motors based on SD card values
 */
 
 /* 
 * Pinout:
 * SD card attached to SPI bus as follows:
 * - MOSI - pin 16
 * - MISO - pin 14
 * - CLK - pin 15
 * - CS - pin 10
 * 
 * X-Axis stepper motor driver board:
 * - STEP - pin 2
 * - DIR - pin 3 
 * - MS1 - pin 4
 * - MS2 - pin 5
 * - Enable - pin 6
 * 
 * Y-Axis stepper motor driver board:
 * - STEP - pin 7
 * - DIR - pin 8
 * - MS1 - pin 9
 * - MS2 - pin 18 (A0 on Arduino Pro Micro silkscreen)
 * - ENABLE - pin 19 (A1 on Arduino Pro Micro silkscreen)
 */

#include <SPI.h>
#include <SD.h>

File xyValues;
String filename="drawing.txt";
int noXSteps=0;
int noYSteps=0;

int csPin=10;

int xStep=2;
int xDir=3;
int xMS1=4;
int xMS2=5;
int xEnable=6;

int yStep=7;
int yDir=8;
int yMS1=9;
int yMS2=18;
int yEnable=19;

int scalingFactor=5;

void setup() 
{
  //Define stepper pins as digital output pins
  pinMode(xStep,OUTPUT);
  pinMode(xDir,OUTPUT);
  pinMode(xMS1,OUTPUT);
  pinMode(xMS2,OUTPUT);
  pinMode(xEnable,OUTPUT);
  pinMode(yStep,OUTPUT);
  pinMode(yDir,OUTPUT);
  pinMode(yMS1,OUTPUT);
  pinMode(yMS2,OUTPUT);
  pinMode(yEnable,OUTPUT);

  //Set microstepping mode for stepper driver boards.  Using 1.8 deg motor angle (200 steps/rev) NEMA 17 motors (12V)
  
  //X-Axis motor: no micro stepping (MS1 Low, MS2 Low) = 1.8 deg/step (200 steps/rev)
  digitalWrite(xMS1,LOW);
  digitalWrite(xMS2,LOW);
  
  //Y-Axis motor: no micro stepping (MS1 Low, MS2 Low) = 1.8 deg/step (200 steps/rev)
  digitalWrite(yMS1,LOW);
  digitalWrite(yMS2,LOW);

  //Enable motor controllers
  digitalWrite(xEnable,LOW);
  digitalWrite(yEnable,LOW);
    
  //Open serial communications
  Serial.begin(9600);

  //Debug to Serial
  Serial.print("Initializing SD card... ");
  if (!SD.begin(csPin))
  {
    Serial.println("initialization failed!  Aborting!");
	  Serial.println("----------");
    return; //Aborts program if SD card initialization fails
  }
    Serial.println("initialization success!");
    Serial.println("----------");
}

void loop() 
{
  
  //Open the file for reading:
  xyValues = SD.open(filename);
  if (xyValues)
  {
      Serial.print("Opened ");
      Serial.print(filename);
      Serial.println(":");
	    Serial.println("----------");
  }
  else
  {
    //If the file didn't open, print an error and abort
    Serial.print("Error opening ");
    Serial.print(filename);
	  Serial.println("!");
	  Serial.println("Aborting!");
	  Serial.println("----------");
	  return; //Aborts program if file could not be opened
  }
  
  //Read from file until there's nothing else in it:
  while (xyValues.available()) 
  {
    
    //Read next X, Y waypoints

	  //Covert to integers
    noXSteps = xyValues.parseInt();
	  noYSteps = xyValues.parseInt();

    //Scale no of steps based on desired image size
    noXSteps=noXSteps*scalingFactor;
    noYSteps=noYSteps*scalingFactor;

    Serial.print("x: ");
    Serial.print(noXSteps);
    Serial.print(" y: ");
    Serial.println(noYSteps);

    //Rotate motors
	  rotateMotors(noXSteps, noYSteps);
	  delay(200);
  }
	
  // Close the file:
  xyValues.close(); 
   
  // Drawing complete.  Pause for one hour (3600 seconds), i.e. freeze until power off because drawing is done.
  Serial.println("Drawing complete!");
  Serial.println("Pausing for 1 hour.");
  for (int k=0; k<3600; k++)
  {
    delay(1000); 
  }
}

void rotateMotors(int noXSteps, int noYSteps)
{
  //Initialize while loop counter
  int totalSteps=0;
  int stepDelay=10; 

  //Set X-Axis motor rotation direction based on read value
  if (noXSteps<0) //Set X-Axis rotation direction CCW
  {
	  digitalWrite(xDir,LOW); 
  }
  else //Set X-Axis rotation direction to CW
  {
	  digitalWrite(xDir,HIGH);
  }

  //Set Y-Axis motor rotation direction based on read value
  if (noYSteps<0) //Set Y-Axis rotation to CCW
  {
	  digitalWrite(yDir,LOW);
  }
  else
  {
	  digitalWrite(yDir,HIGH);
  }

  //Calculate total number of steps for while loop indexing
  totalSteps=abs(noXSteps)+abs(noYSteps);
  
  //Get absolute value of steps
  noXSteps=abs(noXSteps);
  noYSteps=abs(noYSteps);
  
  //Move motors appropriate number of steps
  while (totalSteps>0)
  {
	  if (noXSteps>0) //Move X-Axis 
	  {
		  //Move X-Axis one step
		  digitalWrite(xStep, LOW); //LOW to HIGH changes creates the "Rising Edge" so that the EasyDriver knows when to step.
		  delay(1);
		  digitalWrite(xStep, HIGH); 
		  delay(1);
		  noXSteps=noXSteps-1; //Decrement remaining number of X-Axis steps
		  totalSteps=totalSteps-1; //Decrement remaining number of total steps
	  }
	
	  if (noYSteps>0) //Move Y-Axis 
	  {
		  //Move Y-Axis one step
		  digitalWrite(yStep, LOW); //LOW to HIGH changes creates the "Rising Edge" so that the EasyDriver knows when to step.
		  delay(1);
		  digitalWrite(yStep, HIGH); 
		  delay(1);
		  noYSteps=noYSteps-1; //Decrement remaining number of Y-Axis steps
		  totalSteps=totalSteps-1; //Decrement remaining number of total steps
	  }

   delay(stepDelay);
   
  }
}
