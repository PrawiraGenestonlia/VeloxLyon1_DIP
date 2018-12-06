#include <SparkFunBQ27441.h>
#include <BQ27441_Definitions.h>
#include <Wire.h>
#include "AS726X.h"
#include <SPI.h>
#include <QwiicMux.h>
#include <QwiicMux0x10.h>
#include <SdFat.h>
#include <MLX90393.h>


#define PIN_SPI_MOSI 11
#define PIN_SPI_MISO 12
#define PIN_SPI_CLK 13
#define PIN_SD_CS 4
#define PIN_OTHER_DEVICE_CS -1
#define SD_CARD_SPEED SPI_FULL_SPEED

AS726X SpectralSensor;
SdFat SD;
File myFile;
MLX90393 mlx;
MLX90393::txyz data;


String output_string = "";
const byte address[6] = "00001";
char buff[64];
const unsigned int BATTERY_CAPACITY = 850; // e.g. 850mAh battery

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
//  enableMuxPort(6);
//  setupBQ27441();
//  disableMuxPort(6);
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
  Serial.print(buff);

  delay(250);
  sd_write(buff);
  transmit_to_SAMD(buff);
}


void update_output() {
  output_string = "$;";
//  enableMuxPort(6);
//  output_string += String(lipo.soc());
//  disableMuxPort(6);
  output_string += "60";
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

void sd_read_lastline() {
  myFile = SD.open("data.txt");
  //  Serial.print("This is the last line: ");
  while (true) {
    char cr = myFile.read();
    if ((cr == '\n') && ("LAST LINE?"))
      break;

    Serial.print(cr);
  }
  Serial.println("");
  myFile.close();
}

//void setupBQ27441(void)
//{
//  // Use lipo.begin() to initialize the BQ27441-G1A and confirm that it's
//  // connected and communicating.
//  if (!lipo.begin()) // begin() will return true if communication is successful
//  {
//  // If communication fails, print an error message and loop forever.
//    Serial.println("Error: Unable to communicate with BQ27441.");
//    Serial.println("  Check wiring and try again.");
//    Serial.println("  (Battery must be plugged into Battery Babysitter!)");
//    while (1) ;
//  }
//  Serial.println("Connected to BQ27441!");
//  
//  // Uset lipo.setCapacity(BATTERY_CAPACITY) to set the design capacity
//  // of your battery.
//  lipo.setCapacity(BATTERY_CAPACITY);
//}

void transmit_to_SAMD(String info) {
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
  delay(100);
}
