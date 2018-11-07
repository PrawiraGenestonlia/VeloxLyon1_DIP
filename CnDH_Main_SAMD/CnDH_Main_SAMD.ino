/*
  Both the TX and RX ProRF boards will need a wire antenna. We recommend a 3" piece of wire.
  This example is a modified version of the example provided by the Radio Head
  Library which can be found here:
  www.github.com/PaulStoffregen/RadioHeadd
*/
//#define GPS 1
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

#define ARRSIZE 128
#define BUFFER_LENGTH 180

// We need to provide the RFM95 module's chip select and interrupt pins to the
// rf95 instance below.On the SparkFun ProRF those pins are 12 and 6 respectively.
RH_RF95 rf95(12, 6);

int LED = 13; //Status LED is on pin 13
String str = "";
int packetCounter = 0; //Counts the number of packets sent
long timeSinceLastPacket = 0; //Tracks the time stamp of last packet received
String from_main = "";
String from_main2 = "";
String first_part = "" , second_part = "";

// The broadcast frequency is set to 921.2, but the SADM21 ProRf operates
// anywhere in the range of 902-928MHz in the Americas.
// Europe operates in the frequencies 863-870, center frequency at 868MHz.
// This works but it is unknown how well the radio configures to this frequency:
//float frequency = 864.1;
float frequency = 921.2; //Broadcast frequency
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 5000;           // interval at which to blink (milliseconds)
String output_string = "";
const byte address[6] = "00001";
char buff[ARRSIZE];
char cr;
int incomingByte = 0;   // for incoming serial data
int state = 1;
int received_state = 1;
String *first_part2;

void setup()
{
  pinMode(LED, OUTPUT);
  Wire.begin(7);
  Wire.onReceive(receiveEvent2);
  SerialUSB.begin(115200);
  // It may be difficult to read serial messages on startup. The following line
  // will wait for serial to be ready before continuing. Comment out if not needed.
  //  while (!SerialUSB);
  SerialUSB.println("RFM Client!");

#ifdef GPS 1
  enableMuxPort1(0);
  if (myI2CGPS.begin() == false) {//Checks for succesful initialization of GPS
    SerialUSB.println("Module failed to respond. Please check wiring.");
    while (1); //Freeze!
  }
  disableMuxPort1(0);
#endif
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
  //
    SerialUSB.print("This is from void loop: ");
    SerialUSB.println(*first_part2);
  if (rf95.available()) {
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    int received_message;
    if (rf95.recv(buf, &len)) {
      timeSinceLastPacket = millis(); //Timestamp this packet
      received_message = atoi((char*)buf);
      SerialUSB.print("Got message: ");
      SerialUSB.print(received_message);
      if (received_message == 11) {
        state = 1;
      }
      if (received_message == 22) {
        state = 2;
      }
      if (received_message == 33) {
        state = 3;
      }
    }
  }
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

//    output_string = second_part;

    
    output_string.toCharArray(buff, ARRSIZE);


    if (state == 1) {
      SerialUSB.print("Sending message: ");
      SerialUSB.print(buff);
      SerialUSB.println();
      uint8_t toSend[ARRSIZE] = {};
      for (int i = 0; i < ARRSIZE; i++) {
        toSend[i] = buff[i];
      }
      rf95.send(toSend, sizeof(toSend));
      rf95.waitPacketSent();
    }

    if (state == 2) {

    }

    if (state == 3) {

    }
    delay(200);
  }
  delay(50);
}

void update_output() {
  //  output_string = "$;";
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
  output_string += first_part;
  output_string += second_part;
  //  output_string += ";#";
  output_string += "\n";
}


void receiveEvent(int howMany)
{
  from_main = "";
  while (Wire.available()) {
    str = char(Wire.read());
    from_main += str;
    SerialUSB.print(str);
  }

  SerialUSB.println();

}

void receiveEvent2(int howMany) {
  if (received_state == 1) {
    from_main = "";
    while (Wire.available()) {
      str = char(Wire.read());
      from_main += str;
      //      SerialUSB.print(str);
    }
    first_part = from_main;
//    SerialUSB.print(first_part);`
    received_state = 2;

  }
  if (received_state == 2) {
    from_main2 = "";
    while (Wire.available()) {
      str = char(Wire.read());
      from_main2 += str;
      //      SerialUSB.print(str);
    }
    second_part = from_main2;
//    SerialUSB.print(second_part);
    received_state = 1;
    output_string = first_part+second_part;
    first_part2= &output_string;
    SerialUSB.print(output_string);
  }

}
