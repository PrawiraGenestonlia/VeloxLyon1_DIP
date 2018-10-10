#include <Wire.h>
#include "AS726X.h"
#include "SparkFun_I2C_GPS_Arduino_Library.h"
#include <QwiicMux.h>
#include <TinyGPS++.h>
#include <MLX90393.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SdFat.h>

#define logging_frequency 5
#define CCS811_ADDR 0x5B //Default I2C Address
#define NUMBER_OF_SENSORS 3
#define GPSMuxPort 3
#define SpectralSensor1MuxPort 1
#define SpectralSensor2MuxPort 2
#define MLXMuxPort 5
#define LEN_RAD 32

TinyGPSPlus gps; //Declare gps object
AS726X SpectralSensor; //declare
I2CGPS myI2CGPS; //Hook object to the library
SdFat SD;
#define SD_CS_PIN 10
File myFile;
//MLX90393 mlx;
//MLX90393::txyz data; //Create a structure, called data, of four floats (t, x, y, and z)
//RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";
char buff[32];

int i = 0;
String current_string = "";

void setup() {
  // put your setup code here, to run once:
  pinMode(A1,OUTPUT);
  Serial.begin(115200);
  Wire.begin();
  enableMuxPort(GPSMuxPort);
  if (myI2CGPS.begin() == false) {//Checks for succesful initialization of GPS
    Serial.println("Module failed to respond. Please check wiring.");
    while (1); //Freeze!
  }
  disableMuxPort(GPSMuxPort);

  enableMuxPort(SpectralSensor1MuxPort);
  SpectralSensor.begin();
  disableMuxPort(SpectralSensor1MuxPort);

  enableMuxPort(SpectralSensor2MuxPort);
  SpectralSensor.begin();
  disableMuxPort(SpectralSensor2MuxPort);


  Serial.print("Initializing SD card...");

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("10102018.txt", FILE_WRITE);
  //  enableMuxPort(MLXMuxPort);
  //  mlx.begin();
  //  disableMuxPort(MLXMuxPort);

  //  radio.begin();
  //  radio.openWritingPipe(address);
  //  radio.setPALevel(RF24_PA_MIN);
  //  radio.stopListening();



}

void loop() {
  // put your main code here, to run repeatedly:
  update_output();
  Serial.print(current_string);
  delay(1000 / logging_frequency); //Wait for next reading
  i++;
  
  myFile = SD.open("10102018.txt", FILE_WRITE);
  if (myFile) {
    digitalWrite(A1,HIGH);
    Serial.print("Writing to 10102018.txt...");
    myFile.print(current_string);
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  digitalWrite(A1,LOW);
}

void update_output() {
  current_string = "STR;"; //start
  enableMuxPort(GPSMuxPort);
  while (myI2CGPS.available()) { //available() returns the number of new bytes available from the GPS module
    gps.encode(myI2CGPS.read()); //Feed the GPS parser
  }
  current_string += gps.date.month();
  current_string += "/";
  current_string += gps.date.day();
  current_string += "/";
  current_string += gps.date.year();
  current_string += ";";
  current_string += gps.location.lat(); //GPS latitude
  current_string += ";";
  current_string += gps.location.lng(); //GPS longitude
  disableMuxPort(GPSMuxPort);
  delay(50);
  current_string += ";";
  enableMuxPort(SpectralSensor1MuxPort);
  SpectralSensor.takeMeasurements();
  current_string += SpectralSensor.getTemperature();
  current_string += ";";
  current_string += SpectralSensor.getCalibratedR();
  disableMuxPort(SpectralSensor1MuxPort);
  delay(50);
  current_string += ";";
  enableMuxPort(SpectralSensor2MuxPort);
  SpectralSensor.takeMeasurements();
  current_string += SpectralSensor.getCalibratedR();
  disableMuxPort(SpectralSensor2MuxPort);
  delay(50);
//  current_string += ";";
//  enableMuxPort(MLXMuxPort);
//  mlx.readData(data);
//  current_string += data.x;
//  current_string += ";";
//  current_string += data.y;
//  current_string += ";";
//  current_string += data.z;
//  current_string += ";";
//  current_string += data.t;
//  disableMuxPort(MLXMuxPort);
//  delay(10);
  current_string += "; END \n"; //end
  delay(50);
}
