//#define GPS 1

#include <Wire.h>
#include "AS726X.h"
#include <SPI.h>
#ifdef GPS 1
#include "SparkFun_I2C_GPS_Arduino_Library.h"
#include <TinyGPS++.h>
#endif
#include <QwiicMux0x49.h>
#include <QwiicMux0x10.h>
#include <SdFat.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <CSVFile.h>

#define PIN_SPI_MOSI 11
#define PIN_SPI_MISO 12
#define PIN_SPI_CLK 13
#define PIN_SD_CS 4
#define PIN_OTHER_DEVICE_CS 8
#define SD_CARD_SPEED SPI_FULL_SPEED
#define FILENAME "DEMO.csv"

AS726X SpectralSensor;
SdFat SD;
File myFile;
CSVFile csv;
RF24 radio(7, 8); // CE, CSN
#ifdef GPS 1
TinyGPSPlus gps; //Declare gps object
I2CGPS myI2CGPS; //Hook object to the library
#endif

String output_string = "";
const byte address[6] = "00001";
char buff[32];
char cr;

void setup() {
  pinMode(PIN_SPI_MOSI, OUTPUT);
  pinMode(PIN_SPI_MISO, INPUT);
  pinMode(PIN_SPI_CLK, OUTPUT);
  //Disable SPI devices
  pinMode(PIN_SD_CS, OUTPUT);
  digitalWrite(PIN_SD_CS, HIGH);

#if PIN_OTHER_DEVICE_CS > 0
  pinMode(PIN_OTHER_DEVICE_CS, OUTPUT);
  digitalWrite(PIN_OTHER_DEVICE_CS, HIGH);
#endif //PIN_OTHER_DEVICE_CS > 0

  Serial.begin(115200);
  pinMode(A1, OUTPUT);
  Wire.begin();
  enableMuxPort4(9);
  SpectralSensor.begin();
  disableMuxPort4(9);
#ifdef GPS 1
  enableMuxPort1(0);
  if (myI2CGPS.begin() == false) {//Checks for succesful initialization of GPS
    Serial.println("Module failed to respond. Please check wiring.");
    while (1); //Freeze!
  }
  disableMuxPort1(0);
#endif
  if (!SD.begin(PIN_SD_CS, SD_CARD_SPEED))
  {
    Serial.println("SD card begin error");
    return;
  }
  csv.open(FILENAME, O_RDWR | O_CREAT);
  csv.close();
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  Serial.println("The program is successfully initialized");
}

void loop() {
  initSdFile();
  update_output();
//  Serial.print(output_string);

  //  sd_write(output_string);
  //  sd_read_lastline();
  output_string.toCharArray(buff, 32);
  radio.write(&buff, sizeof(buff));
  Serial.print(buff);
  csv.gotoBeginOfLine();
  csv.addField(buff);
  csv.addLine();
  csv.close();
  delay(200);
}

void initSdFile() {
  if (SD.exists(FILENAME) && !SD.remove(FILENAME))
  {
    Serial.println("Failed init remove file");
    return;
  }
  if (!csv.open(FILENAME, O_RDWR | O_CREAT)) {
    Serial.println("Failed open file");
  }
}

void update_output() {
  output_string = "$;";
#ifdef GPS 1
  enableMuxPort1(0);
  while (myI2CGPS.available()) { //available() returns the number of new bytes available from the GPS module
    gps.encode(myI2CGPS.read()); //Feed the GPS parser
  }
  output_string += gps.date.day();
  output_string += "/";
  output_string += gps.date.month();
  output_string += "/";
  output_string += gps.date.year();
  output_string += ";";
  output_string += gps.location.lat(); //GPS latitude
  output_string += ";";
  output_string += gps.location.lng(); //GPS longitude
  disableMuxPort1(0);
  delay(50);
  output_string += ";";
#endif
  enableMuxPort4(9);
  SpectralSensor.takeMeasurements();
  delay(10);
  output_string += SpectralSensor.getTemperature();
  output_string += ";";
  output_string += SpectralSensor.getCalibratedR();
  disableMuxPort4(9);
  output_string += ";#";
  output_string += "\n";
}

void sd_write(String info) {
  myFile = SD.open("24102018.txt", FILE_WRITE);
  if (myFile) {
    digitalWrite(A1, HIGH);
    Serial.print("Writing to 24102018.txt...");
    myFile.println(info);
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening..");
  }
  digitalWrite(A1, LOW);
}

void sd_read() {
  myFile = SD.open("24102018.txt");
  if (myFile) {
    Serial.println("24102018.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening..");
  }
}

void sd_read_lastline() {
  myFile = SD.open("24102018.txt");
  Serial.print("This is the last line: ");
  while (true) {
    cr = myFile.read();
    if ((cr == '\n') && ("LAST LINE?"))
      break;

    Serial.print(cr);
  }
  Serial.println("");
  myFile.close();
}

