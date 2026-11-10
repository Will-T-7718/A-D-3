#include <QTRSensors.h>
#include <L298NX2.h>
#include <SD.h>

//SD Card
File myFile;
const int CSPin = 10;

QTRSensors qtr;
const uint8_t SensorCount = 4;
uint16_t sensorValues[SensorCount];
//Defining line sensor pins
const int R1 = 4;
const int R2 = 5;
const int R3 = 6;
const int R4 = 7;

//ultrsonic Sensor Pins
const int trig = 2;
const int echo = 3;

//Colour sensor pins
#define S0 A0
#define S1 A1
#define S2 A3
#define S3 A2
#define sensorOut A5

int frequency = 0;

//Motor
int IN1 = 8;
int IN2 = 9;
int IN3 = 12;
int IN4 = 13;

L298NX2 myMotors(IN1,IN2,IN3,IN4);

long duration;
int distance;

int redMin = 25; // Red minimum value
int redMax = 197; // Red maximum value
int greenMin = 26; // Green minimum value
int greenMax = 205; // Green maximum value
int blueMin = 26; // Blue minimum value
int blueMax = 196; // Blue maximum value
 
// Variables for Color Pulse Width Measurements
int redPW = 0;
int greenPW = 0;
int bluePW = 0;
 
// Variables for final Color values
int redValue;
int greenValue;
int blueValue;

void setup() 
{
  //Colour sensor pins
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  
  // Setting frequency-scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);
  
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  //motor pinmodes
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

   // line sensor array
  qtr.setTypeRC(); 
  qtr.setSensorPins((const uint8_t[]){7, 6, 5, 4}, SensorCount);

  Serial.begin(9600);
  // used for debugging when I was having issues during setup
  Serial.println("go go ga ga");

  for (uint16_t i = 0; i < 400; i++)
  {
    qtr.calibrate();
    Serial.print("Calibrated: ");
    Serial.println(i);
  }
  Serial.print("Initializing SD card...");

  if (!SD.begin(CSPin)) 
  {
    Serial.println("initialization failed");
    return;
  }
    Serial.println("initialization done");
  }


void loop() 
{
  
  qtr.read(sensorValues);

  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(sensorValues[i]);  
    Serial.print('\t');
  }
  Serial.println("<Line Sensor Values");
  Serial.println(); 
  UltrasonicSensor();
  sensorfunc();
  ColourSensor();
  delay(100);
}

void SDCardWrite()
{
  myFile = SD.open("test.txt", FILE_WRITE);

  // opens file and writes to it
  if (myFile) 
  {
    Serial.print("writing to test.txt");
    myFile.println("RightLeftForwardBack");
    myFile.println("End File");
    myFile.close();
  } 
  else 
  // if it cant open file
  {
  Serial.print("Failed To Open");
  }
}



void ColourSensor()
{
  // Read Red value
  redPW = getRedPW();
  // Map to value from 0-255
  redValue = map(redPW, redMin,redMax,255,0);
  // Delay to stabilize sensor
  delay(50);
  
  // Read Green value
  greenPW = getGreenPW();
  // Map to value from 0-255
  greenValue = map(greenPW, greenMin,greenMax,255,0);
  // Delay to stabilize sensor
  delay(50);
  
  // Read Blue value
  bluePW = getBluePW();
  // Map to value from 0-255
  blueValue = map(bluePW, blueMin,blueMax,255,0);
  // Delay to stabilize sensor
  delay(50);
  
  // Print output to Serial Monitor
  Serial.print("Red = ");
  Serial.print(redValue);
  Serial.print(" - Green = ");
  Serial.print(greenValue);
  Serial.print(" - Blue = ");
  Serial.println(blueValue);
}

int getRedPW() 
{
  // Set sensor to read Red only
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;
}
 
// Function to read Green Pulse Widths
int getGreenPW() 
{
  // Set sensor to read Green only
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;
}
 
// Function to read Blue Pulse Widths
int getBluePW() 
{
  // Set sensor to read Blue only
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;
}

void sensorfunc() 
{
  // this code can be updated to use the built in location function in qtr
  uint16_t position = qtr.readLineBlack(sensorValues);
  Serial.print("Position: ");
  Serial.println(position);

  if (position > 1400 && position < 1600)
  {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }
  delay(100);
}
  
void UltrasonicSensor()
{
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(5);
  digitalWrite(trig, LOW);
	
  duration = pulseIn(echo, HIGH);
  
  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  delay(100);
}