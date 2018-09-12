#include <Wire.h>
#include "AS726X.h"
#include "SparkFun_I2C_GPS_Arduino_Library.h"

#define NUMBER_OF_SENSORS 3
#define GPSMuxPort 3
#define SpectralSensor1MuxPort 1
#define SpectralSensor2MuxPort 2


#include <TinyGPS++.h>
TinyGPSPlus gps; //Declare gps object

AS726X SpectralSensor; //declare
I2CGPS myI2CGPS; //Hook object to the library

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Initialising the program...");
  Wire.begin();
  enableMuxPort(GPSMuxPort);
  if (myI2CGPS.begin() == false) {//Checks for succesful initialization of GPS
    Serial.println("Module failed to respond. Please check wiring.");
    while (1); //Freeze!
  }
  Serial.println("GPS module found!");
  disableMuxPort(GPSMuxPort);

  enableMuxPort(SpectralSensor1MuxPort);
  SpectralSensor.begin();
  disableMuxPort(SpectralSensor1MuxPort);

  enableMuxPort(SpectralSensor2MuxPort);
  SpectralSensor.begin();
  disableMuxPort(SpectralSensor2MuxPort);

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

  for (byte x = 1 ; x < 3 ; x++) {
    enableMuxPort(x); //Tell mux to connect to this port, and this port only
//    if (SpectralSensor.dataAvailable()) {
      SpectralSensor.takeMeasurements();
      SpectralSensor.printMeasurements();
//    }
    disableMuxPort(x);
  }

  enableMuxPort(GPSMuxPort);
  while (myI2CGPS.available()) //available() returns the number of new bytes available from the GPS module
  {
    gps.encode(myI2CGPS.read()); //Feed the GPS parser
  }

  if (gps.time.isUpdated()) //Check to see if new GPS info is available
  {
    displayInfo();
  }

  disableMuxPort(GPSMuxPort); //Tell mux to disconnect from this port


  //  while (myI2CGPS.available()) { //available() returns the number of new bytes available from the GPS module
  //
  //    byte incoming = myI2CGPS.read(); //Read the latest byte from Qwiic GPS
  //
  //    if (incoming == true) Serial.println(); //Break the sentences onto new lines
  //    Serial.write(incoming); //Print this character
  //    Serial.println("");
  //  }

  Serial.println("");
    Serial.println("");
  delay(500); //Wait for next reading



}

void displayInfo()
{
  //We have new GPS data to deal with!
  Serial.println();

  if (gps.time.isValid())
  {
    Serial.print(F("Date: "));
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());

    Serial.print((" Time: "));
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());

    Serial.println(); //Done printing time
  }
  else
  {
    Serial.println(F("Time not yet valid"));
  }

  if (gps.location.isValid())
  {
    Serial.print("Location: ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(", "));
    Serial.print(gps.location.lng(), 6);
    Serial.println();
  }
  else
  {
    Serial.println(F("Location not yet valid"));
  }
}
