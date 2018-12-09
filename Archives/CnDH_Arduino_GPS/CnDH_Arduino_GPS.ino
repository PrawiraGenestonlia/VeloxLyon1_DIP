#include <SparkFun_I2C_GPS_Arduino_Library.h> 
#include <TinyGPS++.h>

TinyGPSPlus gps;
I2CGPS myI2CGPS; 
String current_string;

void setup()
{
  Serial.begin(115200);
  Wire.begin(15);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  Serial.println("GTOP Read Example");

  if (myI2CGPS.begin() == false)
  {
    Serial.println("Module failed to respond. Please check wiring.");
    while (1); //Freeze!
  }
  Serial.println("GPS module found!");
}

void loop()
{
  updateInfo();
  delay(100);
}

void requestEvent() {
  char buff2[32];
  current_string.toCharArray(buff2,32);
  Serial.print("Sending this....");
  Serial.println(buff2);
  Wire.write(buff2);
}

void updateInfo(){
  current_string="";
  while (myI2CGPS.available()) //available() returns the number of new bytes available from the GPS module
  {
    gps.encode(myI2CGPS.read()); //Feed the GPS parser
  }

  if (gps.time.isUpdated()) //Check to see if new GPS info is available
  {
    current_string += gps.date.day();
    current_string += "/";
    current_string += gps.date.month();
    current_string += "/";
    current_string += gps.date.year();
    current_string += ";";
    current_string += gps.location.lat(); //GPS latitude
    current_string += ";";
    current_string += gps.location.lng(); //GPS longitude
  }
}

//Display new GPS info
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
