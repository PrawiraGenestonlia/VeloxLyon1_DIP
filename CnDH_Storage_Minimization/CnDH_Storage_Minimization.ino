#include <Wire.h>
#include "AS726X.h"
#include <SPI.h>
#include <QwiicMux.h>
#include <SdFat.h>

AS726X SpectralSensor; //declare

String output_string="";

void setup() {
  Serial.begin(115200);
  pinMode(A1,OUTPUT);
  Wire.begin();
  SpectralSensor.begin();
  Serial.println("The program is successfully initialized");
}

void loop() {
  update_output();
  sd_write(output_string);
}

void update_output(){
  output_string = "#;";
  SpectralSensor.takeMeasurements();
  delay(10);
  output_string += SpectralSensor.getTemperature();
  output_string += ";";
  output_string += SpectralSensor.getCalibratedR();
  output_string += ";#";
}

void sd_write(String info) {
  myFile = SD.open("17102018.txt", FILE_WRITE);
  if (myFile) {
    digitalWrite(A1, HIGH);
    Serial.print("Writing to 17102018.txt...");
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
  myFile = SD.open("17102018.txt");
  if (myFile) {
    Serial.println("17102018.txt:");

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
