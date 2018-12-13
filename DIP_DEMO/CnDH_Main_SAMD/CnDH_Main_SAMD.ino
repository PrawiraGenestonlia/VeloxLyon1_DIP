
#include <SPI.h>
#include <Wire.h>
#include <RH_RF95.h>

#define ARRSIZE 128
#define BUFFER_LENGTH 180

// We need to provide the RFM95 module's chip select and interrupt pins to the
// rf95 instance below.On the SparkFun ProRF those pins are 12 and 6 respectively.
RH_RF95 rf95(12, 6);

int LED = 13; //Status LED is on pin 13
int packetCounter = 0; //Counts the number of packets sent
long timeSinceLastPacket = 0; //Tracks the time stamp of last packet received

// The broadcast frequency is set to 921.2, but the SADM21 ProRf operates
// anywhere in the range of 902-928MHz in the Americas.
// Europe operates in the frequencies 863-870, center frequency at 868MHz.
// This works but it is unknown how well the radio configures to this frequency:
//float frequency = 864.1;
float frequency = 868; //Broadcast frequency (921.2)

const byte address[6] = "00001";
char buff[ARRSIZE];
int incomingByte = 0;   // for incoming serial data


void setup()
{
  delay(1000);
  Wire.begin(7);
  Wire.onReceive(receiveEvent);
  SerialUSB.begin(115200);
  // It may be difficult to read serial messages on startup. The following line
  // will wait for serial to be ready before continuing. Comment out if not needed.
  //  while (!SerialUSB);
  SerialUSB.println("RFM Client!");

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
  rf95.setTxPower(23, false);
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
  transmit(message_received);
}

void transmit(String info){
  SerialUSB.print("Transmitting: ...... ");
  SerialUSB.println(info);
  uint8_t toSend[ARRSIZE];   
  char buff[ARRSIZE];
  info.toCharArray(buff, ARRSIZE);
  
  for (int i = 0; i < ARRSIZE; i++) {
    toSend[i] = buff[i];
  }

  rf95.send(toSend, sizeof(toSend));
  rf95.waitPacketSent();
}