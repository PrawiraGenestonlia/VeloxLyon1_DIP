#include <SPI.h>  
#include "RF24.h"
#include <SD.h>
/* SCK -> 13
  MOSI -> 11
  MOSO -> 12
  CS -> 10 */
  File myFile;

int CS = 4;

RF24 myRadio (7, 8);
byte addresses[][6] = {"0"};

struct package
{
 int id=1;
 float temperature = 18.3;
 char  text[100] = "Test";
};


typedef struct package Package;
Package data;


void setup()
{
 Serial.begin(115200);
pinMode(CS, OUTPUT);
if(!SD.begin(CS)){
Serial.println("Problem");
return;
}else{
Serial.println("Everything ok");
}
 myRadio.begin();  
 myRadio.setChannel(100); 
 myRadio.setPALevel(RF24_PA_MAX);
 myRadio.setDataRate( RF24_250KBPS ) ; 
 myRadio.openWritingPipe( addresses[0]);


}

void loop()
{

 myRadio.write(&data, sizeof(data)); 

 Serial.print("\nPackage:");
 Serial.print(data.id);
 Serial.print("\n");
 Serial.println(data.temperature);
 Serial.println(data.text);
 data.id = data.id + 1;
 data.temperature = data.temperature+0.1;


 myFile = SD.open("test.txt", FILE_WRITE);
 if (myFile) {
   myFile.println("t");
   myFile.close(); 
  Serial.println("Writing...");
 }

  delay(500);
 }
