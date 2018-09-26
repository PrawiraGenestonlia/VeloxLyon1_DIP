#define DEBUG  1
//#define LOCAL_STORAGE 1

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

#ifdef LOCAL_STORAGE
#include <SD.h>
#endif

#define logging_frequency 5
#define CCS811_ADDR 0x5B //Default I2C Address
#define NUMBER_OF_SENSORS 3
#define GPSMuxPort 3
#define SpectralSensor1MuxPort 1
#define SpectralSensor2MuxPort 2
#define CCS_BME_MuxPort 4
#define MLXMuxPort 5
#define LEN_RAD 32

TinyGPSPlus gps; //Declare gps object
AS726X SpectralSensor; //declare
I2CGPS myI2CGPS; //Hook object to the library
CCS811 myCCS811(CCS811_ADDR);
BME280 myBME280;
MLX90393 mlx;
MLX90393::txyz data; //Create a structure, called data, of four floats (t, x, y, and z)
RF24 radio(7, 8); // CE, CSN

#ifdef LOCAL_STORAGE
File myFile;
#endif

const byte address[6] = "00001";
char buff[LEN_RAD];
char buff2[LEN_RAD];

int i= 0;
String current_string="";
String current_string2="";

void setup() {
  // put your setup code here, to run once:
  pinMode(2,OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(8, OUTPUT);
  Serial.begin(115200);
#ifdef DEBUG
  Serial.println("Initialising the program...");
#endif

  Wire.begin();
  enableMuxPort(GPSMuxPort);
  if (myI2CGPS.begin() == false) {//Checks for succesful initialization of GPS
#ifdef DEBUG
    Serial.println("Module failed to respond. Please check wiring.");
#endif
    while (1); //Freeze!
  }

#ifdef DEBUG
  Serial.println("GPS module found!");
#endif

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
//  const char header[]="MONTH/DATE/YEAR,Latitude,Longitude,SpectralSensor 1,SpectralSensor 2,Temperature,Magnetometer";
//  radio.write(&header,sizeof(header));

#ifdef LOCAL_STORAGE
  open_sd();
  if (myFile) {
    myFile.println(header_CSV());
  }
#endif

#ifdef DEBUG
  Serial.println("The program is successfully initialised...");
#endif
}

void loop() {
  // put your main code here, to run repeatedly:
  i++;
  digitalWrite(2, HIGH);
  enableMuxPort(GPSMuxPort);
  while (myI2CGPS.available()) { //available() returns the number of new bytes available from the GPS module
    gps.encode(myI2CGPS.read()); //Feed the GPS parser
  }
  if (gps.time.isUpdated()) { //Check to see if new GPS info is available
#ifdef DEBUG
    displayInfo();
#endif
  }
  disableMuxPort(GPSMuxPort); //Tell mux to disconnect from this port
  //  update_GPS_raw_data();
  for (byte x = 1 ; x < 3 ; x++) {
    enableMuxPort(x); //Tell mux to connect to this port, and this port only
    SpectralSensor.takeMeasurements();
#ifdef DEBUG
    SpectralSensor.printMeasurements();
#endif
    disableMuxPort(x);
  }

#ifdef DEBUG
  enableMuxPort(CCS_BME_MuxPort);
  update_CCS_BME();
  disableMuxPort(CCS_BME_MuxPort);

  enableMuxPort(MLXMuxPort);
  update_mlx();
  disableMuxPort(MLXMuxPort);
#endif
  digitalWrite(4,LOW);
  digitalWrite(8,HIGH);
//  update_output();
  update_output_str();
  update_output_end();

#ifdef DEBUG
  Serial.print(i);
  Serial.print("x    ");
  Serial.print(current_string);
  Serial.println(current_string2);
  
#endif
  digitalWrite(4,HIGH);
  digitalWrite(8,LOW);
    current_string.toCharArray(buff, LEN_RAD);
    radio.write(&buff, sizeof(buff));

    delay(50);
    
    current_string2.toCharArray(buff2, LEN_RAD);
    radio.write(&buff2, sizeof(buff2));

  //  const char text[] = "Hello World";
  //  radio.write(&text, sizeof(text));

#ifdef LOCAL_STORAGE
  if (myFile) {
    myFile.print(current_string);
    myFile.println(current_string2);
  }
#endif
digitalWrite(2, LOW);
  delay(1000/logging_frequency); //Wait for next reading
}

#ifdef LOCAL_STORAGE
String header_CSV() {
  String this_string;
  this_string = "MONTH/DATE/YEAR,Latitude,Longitude,SpectralSensor 1,SpectralSensor 2,Temperature,Magnetometer";
  return this_string;
}
#endif

//String update_output() {
//  current_string = "STR,"; //start
//  enableMuxPort(GPSMuxPort);
//  gps.encode(myI2CGPS.read());
//  current_string += gps.date.month();
//  current_string += "/";
//  current_string += gps.date.day();
//  current_string += "/";
//  current_string += gps.date.year();
//  current_string += ",";
//  current_string += gps.location.lat(); //GPS latitude
//  current_string += ",";
//  current_string += gps.location.lng(); //GPS longitude
//  disableMuxPort(GPSMuxPort);
//  delay(10);
//  current_string += ",";
//  enableMuxPort(SpectralSensor1MuxPort);
//  SpectralSensor.takeMeasurements();
//  current_string += SpectralSensor.getR();
//  disableMuxPort(SpectralSensor1MuxPort);
//  delay(10);
//  current_string += ",";
//  enableMuxPort(SpectralSensor2MuxPort);
//  SpectralSensor.takeMeasurements();
//  current_string += SpectralSensor.getR();
//  disableMuxPort(SpectralSensor2MuxPort);
//  delay(10);
//  current_string += ",";
//  enableMuxPort(CCS_BME_MuxPort);
//  myCCS811.readAlgorithmResults(); //Read latest from CCS811 and update tVOC and CO2 variables
//  current_string += myBME280.readTempC();
//  disableMuxPort(CCS_BME_MuxPort);
//  delay(10);
//  current_string += ",";
//  enableMuxPort(MLXMuxPort);
//  mlx.readData(data);
//  current_string += data.x;
//  current_string += ",";
//  current_string += data.y;
//  current_string += ",";
//  current_string += data.z;
//  disableMuxPort(MLXMuxPort);
//  delay(10);
//  current_string += "END"; //end
//}

String update_output_str() {
  current_string = "STR,"; //start
  enableMuxPort(GPSMuxPort);
  gps.encode(myI2CGPS.read());
  current_string += gps.date.month();
  current_string += "/";
  current_string += gps.date.day();
  current_string += "/";
  current_string += gps.date.year();
  current_string += ",";
  current_string += gps.location.lat(); //GPS latitude
  current_string += ",";
  current_string += gps.location.lng(); //GPS longitude
  disableMuxPort(GPSMuxPort);
  delay(10);
  current_string += ",";
}

String update_output_end(){
  enableMuxPort(SpectralSensor1MuxPort);
  SpectralSensor.takeMeasurements();
  current_string2 += SpectralSensor.getR();
  disableMuxPort(SpectralSensor1MuxPort);
  delay(10);
  current_string2 += ",";
  enableMuxPort(SpectralSensor2MuxPort);
  SpectralSensor.takeMeasurements();
  current_string2 += SpectralSensor.getR();
  disableMuxPort(SpectralSensor2MuxPort);
  delay(10);
  current_string2 += ",";
  enableMuxPort(CCS_BME_MuxPort);
  myCCS811.readAlgorithmResults(); //Read latest from CCS811 and update tVOC and CO2 variables
  current_string2 += myBME280.readTempC();
  disableMuxPort(CCS_BME_MuxPort);
  delay(10);
  current_string2 += ",";
  enableMuxPort(MLXMuxPort);
  mlx.readData(data);
  current_string2 += data.x;
  current_string2 += ",";
  current_string2 += data.y;
  current_string2 += ",";
  current_string2 += data.z;
  disableMuxPort(MLXMuxPort);
  delay(10);
  current_string2 += "END"; //end  
}

void update_GPS_raw_data() {
  enableMuxPort(GPSMuxPort);
#ifdef DEBUG
  while (myI2CGPS.available()) { //available() returns the number of new bytes available from the GPS module

    byte incoming = myI2CGPS.read(); //Read the latest byte from Qwiic GPS

    if (incoming == true) Serial.println(); //Break the sentences onto new lines
    Serial.write(incoming); //Print this character
  }

  Serial.println("");
  Serial.println("");
#endif
  disableMuxPort(GPSMuxPort);
}
#ifdef DEBUG
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
#endif

void CCS_BME_setup() {
#ifdef DEBUG
  Serial.println("Apply BME280 data to CCS811 for compensation.");
#endif
  //This begins the CCS811 sensor and prints error status of .begin()
  CCS811Core::status returnCode = myCCS811.begin();
  if (returnCode != CCS811Core::SENSOR_SUCCESS)
  {
#ifdef DEBUG
    Serial.println("Problem with CCS811");
    printDriverError(returnCode);
#endif
  }
  else
  {
#ifdef DEBUG
    Serial.println("CCS811 online");
#endif
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
#ifdef DEBUG
    Serial.println("Problem with BME280");
#endif
  }
  else
  {
#ifdef DEBUG
    Serial.println("BME280 online");
#endif
  }
}

void printData()
{
#ifdef DEBUG
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
#endif
}
#ifdef DEBUG
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
#endif

void update_CCS_BME() {
  if (myCCS811.dataAvailable()) //Check to see if CCS811 has new data (it's the slowest sensor)
  {
    myCCS811.readAlgorithmResults(); //Read latest from CCS811 and update tVOC and CO2 variables
//    getWeather(); //Get latest humidity/pressure/temp data from BME280
#ifdef DEBUG
    printData(); //Pretty print all the data
#endif
  }
  else if (myCCS811.checkForStatusError()) //Check to see if CCS811 has thrown an error
  {
#ifdef DEBUG
    Serial.println(myCCS811.getErrorRegister()); //Prints whatever CSS811 error flags are detected
#endif
  }
}

void update_mlx() {
  mlx.readData(data); //Read the values from the sensor
#ifdef DEBUG
  Serial.print("magX[");
  Serial.print(data.x);
  Serial.print("] magY[");
  Serial.print(data.y);
  Serial.print("] magZ[");
  Serial.print(data.z);
  Serial.print("] temperature(C)[");
  Serial.print(data.t);
  Serial.print("]");

  Serial.println();
#endif
}

#ifdef LOCAL_STORAGE
void open_sd() {
  // Open serial communications and wait for port to open:
#ifdef DEBUG
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");
#endif
  if (!SD.begin(4)) {
#ifdef DEBUG
    Serial.println("initialization failed!");
#endif
    while (1);
  }
#ifdef DEBUG
  Serial.println("initialization done.");
#endif
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("DIP.txt", FILE_WRITE);
  /*
    // if the file opened okay, write to it:
    if (myFile) {
      Serial.print("Writing to DIP.txt...");
      myFile.println("testing 1, 2, 3.");
      // close the file:
      myFile.close();
      Serial.println("done.");
    } else {
      // if the file didn't open, print an error:
      Serial.println("error opening test.txt");
    }
      // re-open the file for reading:
      myFile = SD.open("test.txt");
      if (myFile) {
        Serial.println("test.txt:");

        // read from the file until there's nothing else in it:
        while (myFile.available()) {
          Serial.write(myFile.read());
        }
        // close the file:
        myFile.close();
      } else {
        // if the file didn't open, print an error:
        Serial.println("error opening test.txt");
      }
  */
}
#endif
