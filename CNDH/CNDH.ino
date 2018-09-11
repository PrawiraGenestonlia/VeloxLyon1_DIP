#include <Wire.h>
#include "AS726X.h"
#include "SparkFun_I2C_GPS_Arduino_Library.h"

#define NUMBER_OF_SENSORS 3

AS726X SpectralSensor; //declare
I2CGPS myI2CGPS; //Hook object to the library

void setup() {
  // put your setup code here, to run once:
  Serial.println("Initialising the program...");
  Wire.begin();
  Serial.begin(9600);
  if (myI2CGPS.begin() == false) {//Checks for succesful initialization of GPS
    Serial.println("Module failed to respond. Please check wiring.");
    while (1); //Freeze!
  }
  Serial.println("GPS module found!");
  for (byte x = 0 ; x < NUMBER_OF_SENSORS ; x++) { //Initialize all the sensors
    enableMuxPort(x); //Tell mux to connect to port X
    SpectralSensor.begin(); //Init the sensor connected to this port
    disableMuxPort(x);
  }
  Serial.println("The program is successfully initialised...");
}

void loop() {
  // put your main code here, to run repeatedly:
//  sensor.takeMeasurements();
//  sensor.printMeasurements();//Prints out all measurements (calibrated)
//
 // while (myI2CGPS.available()) { //available() returns the number of new bytes available from the GPS module
 //
 //   byte incoming = myI2CGPS.read(); //Read the latest byte from Qwiic GPS
 //
 //   if (incoming == true) Serial.println(); //Break the sentences onto new lines
 //   Serial.write(incoming); //Print this character
 // }
  for (byte x = 0 ; x < NUMBER_OF_SENSORS ; x++) {
    enableMuxPort(x); //Tell mux to connect to this port, and this port only
    if (SpectralSensor.available()) {
      SpectralSensor.takeMeasurements();
      SpectralSensor.printMeasurements();
    }
    while (myI2CGPS.available()) { //available() returns the number of new bytes available from the GPS module

      byte incoming = myI2CGPS.read(); //Read the latest byte from Qwiic GPS

      if (incoming == true) Serial.println(); //Break the sentences onto new lines
      Serial.write(incoming); //Print this character
    }
    disableMuxPort(x); //Tell mux to disconnect from this port
  }
  delay(1); //Wait for next reading



}
