// Sample program to illustrate interfacing NRF24L01+ radio module with
// Arduino and analog IR distance sensor.
// Transmitting text string to the receiver.
//
// Author        : Fabian Kung
// Last modified : 17 December 2021
// Arduino Board : Pro-micro
// For tutorial on nRF24L01+ module and the supporting Arduino
// library, please visit:
// https://lastminuteengineers.com/nrf24l01-arduino-wireless-communication/

//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define PDEBUG_LED 6

// Create an RF24 object
RF24 radio(9, 8);  // CE, CSN

// Address through which two modules communicate.
const byte address[6] = "00001";
char  strTX[16];          // TX string buffer  


void setup()
{
  pinMode(PDEBUG_LED, OUTPUT);  // Initialize LED driver pin.
  
  radio.begin();

  // Set the channel, carrier frequency = 2400 + [Channel No.] MHz
  radio.setChannel(100); // Use 2500 MHz, to avoid interference with WiFi 2.4GHz system.
  // Set data rate.
  radio.setDataRate(RF24_250KBPS); // Set to lower datarate, this is also most sensitive
                                   // at -94 dBm. Default is RF24_1MBPS, with lower
                                   // sensitivity.
  //set the address
  radio.openWritingPipe(address);
  
  //Set module as transmitter
  radio.stopListening();
}

void loop()
{
  //Send message to receiver
  unsigned int unSensor;
  unsigned int unDigit;
  unsigned int unTen;
  unsigned int unHundred;
  unsigned int unThousand;
  unsigned int unTemp;
  unsigned int unSensor_mV;
    
  int nIndex;

  digitalWrite(PDEBUG_LED,HIGH);     // Turn on debug LED.
  unSensor = analogRead(2);  // Read value of analog sensor.
                               // NOTE: The built-in ADC in Arduino Uno, Nano and
                               // Micro and Pro-micro are 10 bits. With reference
                               // voltage at 5V, each unit interval is 
                               // 5/1023 = 4.888mV.                           
  unSensor_mV = (unSensor*49)/10;     // We use this trick so that the conversion
                                      // works entirely in integer, and within the
                                      // 16-bits limit of the integer datatype used
                                      // by Arduino.   
  unTemp = unSensor_mV;

  // --- Convert unsigned integer value from analog IR sensor to BCD ---
  unDigit = 0;
  unTen = 0;
  unHundred = 0;
  unThousand = 0;

  while (unTemp > 999) // Update thousand.
  {
    unTemp = unTemp - 1000;
    unThousand++;
  }
  while (unTemp > 99)  // Update hundred.
  {
    unTemp = unTemp - 100;
    unHundred++;
  }
  while (unTemp > 9)  // Update ten.
  {
    unTemp = unTemp - 10;
    unTen++;
  }
  unDigit = unTemp;   // Update digit.
  // ---
  
  strTX[0] = unThousand + 0x30;   // Convert unThousand to ASCII.
  strTX[1] = unHundred + 0x30;
  strTX[2] = unTen + 0x30;
  strTX[3] = unDigit + 0x30;
  strTX[4] = '\n';
  //strTX[0] = '1';
  //strTX[1] = '3';
  //strTX[2] = '\n';
  radio.write(&strTX, sizeof(strTX)); // Transmit data string to receiver.
  digitalWrite(PDEBUG_LED,LOW);   // Turn off debug LED.
  delay(250);
}
