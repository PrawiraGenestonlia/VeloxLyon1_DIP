/*
  Both the TX and RX ProRF boards will need a wire antenna. We recommend a 3" piece of wire.
  This example is a modified version of the example provided by the Radio Head
  Library which can be found here:
  www.github.com/PaulStoffregen/RadioHeadd
*/
#define GPS 1
#include <SPI.h>
#include <Wire.h>
//Radio Head Library:
#include <RH_RF95.h>
#ifdef GPS 1
#include <QwiicMux0x10.h>
#include "SparkFun_I2C_GPS_Arduino_Library-modified.h"
#include <TinyGPS++.h>
#endif

#ifdef GPS 1
TinyGPSPlus gps; //Declare gps object
I2CGPS myI2CGPS; //Hook object to the library
#endif

// We need to provide the RFM95 module's chip select and interrupt pins to the
// rf95 instance below.On the SparkFun ProRF those pins are 12 and 6 respectively.
RH_RF95 rf95(12, 6);

int LED = 13; //Status LED is on pin 13
String str ="";
char strchar[32]={};
int packetCounter = 0; //Counts the number of packets sent
long timeSinceLastPacket = 0; //Tracks the time stamp of last packet received

// The broadcast frequency is set to 921.2, but the SADM21 ProRf operates
// anywhere in the range of 902-928MHz in the Americas.
// Europe operates in the frequencies 863-870, center frequency at 868MHz.
// This works but it is unknown how well the radio configures to this frequency:
//float frequency = 864.1;
float frequency = 921.2; //Broadcast frequency

String output_string = "";
const byte address[6] = "00001";
char buff[32];
char cr;
int incomingByte = 0;   // for incoming serial data

void setup()
{
  pinMode(LED, OUTPUT);
  Wire.begin(7);
  Wire.onReceive(receiveEvent);
  SerialUSB.begin(115200);
  // It may be difficult to read serial messages on startup. The following line
  // will wait for serial to be ready before continuing. Comment out if not needed.
  while (!SerialUSB);
  SerialUSB.println("RFM Client!");
  
#ifdef GPS 1
  enableMuxPort1(0);
  if (myI2CGPS.begin() == false) {//Checks for succesful initialization of GPS
    SerialUSB.println("Module failed to respond. Please check wiring.");
    while (1); //Freeze!
  }
  disableMuxPort1(0);
#endif
  update_output();
  SerialUSB.print(output_string);
  //Initialize the Radio.
  if (rf95.init() == false) {
    SerialUSB.println("Radio Init Failed - Freezing");
    while (1);
  }
  else {
    //An LED inidicator to let us know radio initialization has completed.
    SerialUSB.println("Transmitter up!");
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
    delay(500);
  }

  // Set frequency
  rf95.setFrequency(frequency);

  // Transmitter power can range from 14-20dbm.
  rf95.setTxPower(14, true);
}


void loop()
{
  SerialUSB.println("Sending message");

  //Send a message to the other radio
  uint8_t toSend[] = "Hi there!";
  //sprintf(toSend, "Hi, my counter is: %d", packetCounter++);
  rf95.send(toSend, sizeof(toSend));
  rf95.waitPacketSent();

  // Now wait for a reply
  byte buf[RH_RF95_MAX_MESSAGE_LEN];
  byte len = sizeof(buf);

  if (rf95.waitAvailableTimeout(2000)) {
    // Should be a reply message for us now
    if (rf95.recv(buf, &len)) {
      SerialUSB.print("Got reply: ");
      SerialUSB.println((char*)buf);
      //SerialUSB.print(" RSSI: ");
      //SerialUSB.print(rf95.lastRssi(), DEC);
    }
    else {
      SerialUSB.println("Receive failed");
    }
  }
  else {
    SerialUSB.println("No reply, is the receiver running?");
  }
  delay(500);
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
#endif
  delay(50);

  output_string += ";#";
  output_string += "\n";
}


void receiveEvent(int howMany)
{
  while(Wire.available()){
    str=char(Wire.read());
    SerialUSB.print(str);
  }
  SerialUSB.println();
  
}
