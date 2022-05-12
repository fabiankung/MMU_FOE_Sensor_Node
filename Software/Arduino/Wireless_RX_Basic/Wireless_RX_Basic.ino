 // Sample program for interfacing nRF24L01+ radio module with Arduino,
// receiving text string from transmitter.
//
// Author         : Fabian Kung
// Last modified  : 17 December 2021
// Arduino Board  : Pro-micro
// For tutorial on nRF24L01+ module and the supporting Arduino
// library, please visit:
// https://lastminuteengineers.com/nrf24l01-arduino-wireless-communication/

// Include libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define PDEBUG_LED 6
#define PBAT_LEVEL 0    // AN0 - Connected to a 1:2 resistive voltage divider.

//create an RF24 object
RF24 radio(9, 8);  // CE, CSN

//address through which two modules communicate.
const byte address[6] = "00001";
char strRX[16] = {0};             // RX string buffer.

void setup() {
  while (!Serial);
    Serial.begin(9600);
  
  pinMode(PDEBUG_LED,OUTPUT);    // Driver pin for external debug LED.

  radio.begin();
  // Set the channel, carrier frequency = 2400 + [Channel No.] MHz
  radio.setChannel(100); // Use 2500 MHz, to avoid interference with WiFi 2.4GHz system.
  // Set data rate.
  radio.setDataRate(RF24_250KBPS); // Set to lower datarate, this is also most sensitive
                                   // at -94 dBm. Default is RF24_1MBPS with lower 
                                   // sensitivity.
  //set the address
  radio.openReadingPipe(0, address); // Use datapipe 0 for device ID = address.
  //Set module as receiver
  radio.startListening();  
  Serial.println("System ready");
}

void loop() {
  int nIndex;

  //Read the data if available in buffer
  if (radio.available())
  {
    digitalWrite(PDEBUG_LED,HIGH);     // Light up debug LED.
    radio.read(&strRX, sizeof(strRX)); // Send receive text string to display.
    Serial.println(strRX);
    digitalWrite(PDEBUG_LED,LOW);
  }
}
