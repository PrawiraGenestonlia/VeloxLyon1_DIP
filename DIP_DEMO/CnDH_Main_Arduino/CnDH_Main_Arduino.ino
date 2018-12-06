#include <SparkFunBQ27441.h>
#include <BQ27441_Definitions.h>
#include <Wire.h>
#include "AS726X.h"
#include <SPI.h>
#include <QwiicMux.h>
#include <SdFat.h>
#include <MLX90393.h>
#include "SparkFun_I2C_GPS_Arduino_Library.h"

#define PIN_SPI_MOSI 11
#define PIN_SPI_MISO 12
#define PIN_SPI_CLK 13
#define PIN_SD_CS 4
#define PIN_OTHER_DEVICE_CS -1
#define SD_CARD_SPEED SPI_FULL_SPEED
#define BATTERY_CAPACITY 1000


AS726X SpectralSensor;
SdFat SD;
File myFile;
MLX90393 mlx;
MLX90393::txyz data;
I2CGPS myI2CGPS;

String output_string = "";
const byte address[6] = "00001";
char buff[64];

void setup() {
  pinMode(PIN_SPI_MOSI, OUTPUT);
  pinMode(PIN_SPI_MISO, INPUT);
  pinMode(PIN_SPI_CLK, OUTPUT);
  pinMode(PIN_SD_CS, OUTPUT);
  digitalWrite(PIN_SD_CS, HIGH);

#if PIN_OTHER_DEVICE_CS > 0
  pinMode(PIN_OTHER_DEVICE_CS, OUTPUT);
  digitalWrite(PIN_OTHER_DEVICE_CS, HIGH);
#endif //PIN_OTHER_DEVICE_CS > 0

  Serial.begin(115200);
  pinMode(A1, OUTPUT);
  Wire.begin();
  enableMuxPort(1);
  SpectralSensor.begin();
  disableMuxPort(1);
  enableMuxPort(2);
  SpectralSensor.begin();
  disableMuxPort(2);
  enableMuxPort(4);
  mlx.begin();
  disableMuxPort(4);
  setupBQ27441();
  enableMuxPort(5);
  setupGPS;
  disableMuxPort(5);

  if (!SD.begin(PIN_SD_CS, SD_CARD_SPEED))
  {
    Serial.println("SD card begin error");
    return;
  }

  sd_write("this program is initialized \n");
  Serial.println("The program is successfully initialized");
}

void loop() {
  update_output();
  output_string.toCharArray(buff, 64);
//  read_GPS_raw;
  sd_write(buff);
  transmit_RF(buff);
  delay(500);
  
}


void update_output() {
  output_string = "$;";
  output_string += lipo.soc();
  output_string += ";";
  enableMuxPort(1);
  SpectralSensor.takeMeasurements();
  delay(10);
  output_string += SpectralSensor.getTemperature();
  output_string += ";";
  output_string += SpectralSensor.getCalibratedR();
  disableMuxPort(1);
  enableMuxPort(2);
  SpectralSensor.takeMeasurements();
  delay(10);
  output_string += ";";
  output_string += SpectralSensor.getCalibratedR();
  output_string += ";";
  disableMuxPort(2);
  enableMuxPort(4);
  mlx.readData(data);
  output_string += data.x;
  output_string += ";";
  output_string += data.y;
  output_string += ";";
  output_string += data.z;
  output_string += ";";
  output_string += data.t;
  disableMuxPort(4);
  delay(10);
  output_string += ";#";
  output_string += "\n";
}

void sd_write(String info) {
  myFile = SD.open("data.txt", FILE_WRITE);
  if (myFile) {
    digitalWrite(A1, HIGH);
    Serial.print("Writing to data.txt...");
    myFile.print(info);
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
  myFile = SD.open("data.txt");
  if (myFile) {
    Serial.println("data.txt:");

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

void setupBQ27441(void)
{
  // Use lipo.begin() to initialize the BQ27441-G1A and confirm that it's
  // connected and communicating.
  if (!lipo.begin()) // begin() will return true if communication is successful
  {
  // If communication fails, print an error message and loop forever.
    Serial.println("Error: Unable to communicate with BQ27441.");
    Serial.println("  Check wiring and try again.");
    Serial.println("  (Battery must be plugged into Battery Babysitter!)");
    while (1) ;
  }
  Serial.println("Connected to BQ27441!");
  
  // Uset lipo.setCapacity(BATTERY_CAPACITY) to set the design capacity
  // of your battery.
  lipo.setCapacity(BATTERY_CAPACITY);
}

void setupGPS(void){
  if (myI2CGPS.begin() == false)//Checks for succesful initialization of GPS
  {
    Serial.println("Module failed to respond. Please check wiring.");
    while (1); //Freeze!
  }
  Serial.println("GPS module found!");
}

void read_GPS_raw(void){
  enableMuxPort(5);
  while (myI2CGPS.available()) //available() returns the number of new bytes available from the GPS module
  {
    byte incoming = myI2CGPS.read(); //Read the latest byte from Qwiic GPS

    if(incoming == "$") Serial.println(); //Break the sentences onto new lines
    Serial.print(incoming); //Print this character
  }
  disableMuxPort(5);
}
void transmit_RF(String info) {
  Serial.print("Transmitting .... :");
  Serial.println(info);
  enableMuxPort(7);
  Wire.beginTransmission(7); // transmit to device #7
  for (int i = 0; i < 32; i++) {
    Wire.write(char(info[i]));
  }
  Wire.endTransmission();    // stop transmitting
  delay(100);
  Wire.beginTransmission(7); // transmit to device #7
  for (int i = 32; i < 64; i++) {
    Wire.write(char(info[i]));
  }
  Wire.endTransmission();    // stop transmitting
  disableMuxPort(7);
  delay(50);
}
