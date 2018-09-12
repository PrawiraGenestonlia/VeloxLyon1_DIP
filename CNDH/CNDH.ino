#include <Wire.h>
#include "AS726X.h"
#include "SparkFun_I2C_GPS_Arduino_Library.h"
#include <QwiicMux.h>
#include <TinyGPS++.h>
#include <SparkFunBME280.h>
#include <SparkFunCCS811.h>
#include <MLX90393.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CCS811_ADDR 0x5B //Default I2C Address
#define NUMBER_OF_SENSORS 3
#define GPSMuxPort 3
#define SpectralSensor1MuxPort 1
#define SpectralSensor2MuxPort 2
#define CCS_BME_MuxPort 4
#define MLXMuxPort 5

TinyGPSPlus gps; //Declare gps object
AS726X SpectralSensor; //declare
I2CGPS myI2CGPS; //Hook object to the library
CCS811 myCCS811(CCS811_ADDR);
BME280 myBME280;
MLX90393 mlx;
MLX90393::txyz data; //Create a structure, called data, of four floats (t, x, y, and z)
RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";


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

  enableMuxPort(CCS_BME_MuxPort);
  CCS_BME_setup();
  disableMuxPort(CCS_BME_MuxPort);

  enableMuxPort(MLXMuxPort);
  mlx.begin();
  disableMuxPort(MLXMuxPort);

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  Serial.println("The program is successfully initialised...");
}

void loop() {
  // put your main code here, to run repeatedly:

  for (byte x = 1 ; x < 3 ; x++) {
    enableMuxPort(x); //Tell mux to connect to this port, and this port only
    SpectralSensor.takeMeasurements();
    SpectralSensor.printMeasurements();
    disableMuxPort(x);
  }

  enableMuxPort(GPSMuxPort);
  while (myI2CGPS.available()){ //available() returns the number of new bytes available from the GPS module
    gps.encode(myI2CGPS.read()); //Feed the GPS parser
  }

  if (gps.time.isUpdated()){ //Check to see if new GPS info is available
    displayInfo();
  }
  disableMuxPort(GPSMuxPort); //Tell mux to disconnect from this port

//  update_GPS_raw_data();


  enableMuxPort(CCS_BME_MuxPort);
  update_CCS_BME();
  disableMuxPort(CCS_BME_MuxPort);

  const char text[] = "Hello World";
  radio.write(&text, sizeof(text));
  
  delay(500); //Wait for next reading



}

void update_GPS_raw_data() {
  enableMuxPort(GPSMuxPort);
  while (myI2CGPS.available()) { //available() returns the number of new bytes available from the GPS module

    byte incoming = myI2CGPS.read(); //Read the latest byte from Qwiic GPS

    if (incoming == true) Serial.println(); //Break the sentences onto new lines
    Serial.write(incoming); //Print this character
  }
  Serial.println("");
  Serial.println("");
  disableMuxPort(GPSMuxPort);
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

void CCS_BME_setup() {
  Serial.println("Apply BME280 data to CCS811 for compensation.");

  //This begins the CCS811 sensor and prints error status of .begin()
  CCS811Core::status returnCode = myCCS811.begin();
  if (returnCode != CCS811Core::SENSOR_SUCCESS)
  {
    Serial.println("Problem with CCS811");
    printDriverError(returnCode);
  }
  else
  {
    Serial.println("CCS811 online");
  }

  //Initialize BME280
  //For I2C, enable the following and disable the SPI section
  myBME280.settings.commInterface = I2C_MODE;
  myBME280.settings.I2CAddress = 0x77;
  myBME280.settings.runMode = 3; //Normal mode
  myBME280.settings.tStandby = 0;
  myBME280.settings.filter = 4;
  myBME280.settings.tempOverSample = 5;
  myBME280.settings.pressOverSample = 5;
  myBME280.settings.humidOverSample = 5;

  //Calling .begin() causes the settings to be loaded
  delay(10);  //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.
  byte id = myBME280.begin(); //Returns ID of 0x60 if successful
  if (id != 0x60)
  {
    Serial.println("Problem with BME280");
  }
  else
  {
    Serial.println("BME280 online");
  }
}

void printData()
{
  Serial.print(" CO2[");
  Serial.print(myCCS811.getCO2());
  Serial.print("]ppm");

  Serial.print(" TVOC[");
  Serial.print(myCCS811.getTVOC());
  Serial.print("]ppb");

  Serial.print(" temp[");
  Serial.print(myBME280.readTempC(), 1);
  Serial.print("]C");

  //Serial.print(" temp[");
  //Serial.print(myBME280.readTempF(), 1);
  //Serial.print("]F");

  Serial.print(" pressure[");
  Serial.print(myBME280.readFloatPressure(), 2);
  Serial.print("]Pa");

  //Serial.print(" pressure[");
  //Serial.print((myBME280.readFloatPressure() * 0.0002953), 2);
  //Serial.print("]InHg");

  //Serial.print("altitude[");
  //Serial.print(myBME280.readFloatAltitudeMeters(), 2);
  //Serial.print("]m");

  //Serial.print("altitude[");
  //Serial.print(myBME280.readFloatAltitudeFeet(), 2);
  //Serial.print("]ft");

  Serial.print(" humidity[");
  Serial.print(myBME280.readFloatHumidity(), 0);
  Serial.print("]%");

  Serial.println();
}

void printDriverError( CCS811Core::status errorCode )
{
  switch ( errorCode )
  {
    case CCS811Core::SENSOR_SUCCESS:
      Serial.print("SUCCESS");
      break;
    case CCS811Core::SENSOR_ID_ERROR:
      Serial.print("ID_ERROR");
      break;
    case CCS811Core::SENSOR_I2C_ERROR:
      Serial.print("I2C_ERROR");
      break;
    case CCS811Core::SENSOR_INTERNAL_ERROR:
      Serial.print("INTERNAL_ERROR");
      break;
    case CCS811Core::SENSOR_GENERIC_ERROR:
      Serial.print("GENERIC_ERROR");
      break;
    default:
      Serial.print("Unspecified error.");
  }
}

void update_CCS_BME() {
  if (myCCS811.dataAvailable()) //Check to see if CCS811 has new data (it's the slowest sensor)
  {
    myCCS811.readAlgorithmResults(); //Read latest from CCS811 and update tVOC and CO2 variables
    //getWeather(); //Get latest humidity/pressure/temp data from BME280
    printData(); //Pretty print all the data
  }
  else if (myCCS811.checkForStatusError()) //Check to see if CCS811 has thrown an error
  {
    Serial.println(myCCS811.getErrorRegister()); //Prints whatever CSS811 error flags are detected
  }
}


