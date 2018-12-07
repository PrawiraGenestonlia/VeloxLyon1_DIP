#include <SdFat.h>
#include <Wire.h>

SdFat SD;
File myFile;

void setup()
{
  
  #define PIN_SPI_MOSI 11
  #define PIN_SPI_MISO 12
  #define PIN_SPI_CLK 13
  #define PIN_SD_CS 4
  #define PIN_OTHER_DEVICE_CS -1
  #define SD_CARD_SPEED SPI_FULL_SPEED
  
  Serial.begin(115200);
  Wire.begin(9);                // join i2c bus with address #8
  
  Wire.onReceive(receiveEvent);

    if (!SD.begin(PIN_SD_CS, SD_CARD_SPEED))
    {
      //    Serial.println("SD card begin error");
      return;
    }
  
    sd_write("this program is initialized \n");
}

void loop()
{
delay(100);

}

void receiveEvent(int howMany)
{
  String message_received ="";
  while (Wire.available()) {
    message_received += char(Wire.read());
  }
//  SerialUSB.println(message_received);
  sd_write(message_received);
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