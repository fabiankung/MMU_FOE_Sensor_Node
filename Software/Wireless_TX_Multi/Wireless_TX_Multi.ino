// Sample program to illustrate interfacing NRF24L01+ radio module with
// Arduino and analog IR distance sensor. The sensor output is connected
// to pin A2 of Arduino.
// Transmitting text string to the receiver.
//
// Author        : Fabian Kung
// Last modified : 6 June 2022
// Arduino Board : Pro-micro
// For tutorial on nRF24L01+ module and the supporting Arduino
// library, please visit:
// https://lastminuteengineers.com/nrf24l01-arduino-wireless-communication/

//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define PDEBUG_LED  6
#define PBAT_STAT   7
#define PBAT_LEVEL  0    // AN0 - Connected to a 1:2 resistive voltage divider.

// Create an RF24 object
RF24 radio(9, 8);  // CE, CSN

// Address through which two modules communicate.
uint64_t address[6] = {0x7878787878LL,
                       0xB3B4B5B6F1LL,
                       0xB3B4B5B6CDLL,
                       0xB3B4B5B6A3LL,
                       0xB3B4B5B60FLL,
                       0xB3B4B5B605LL
                      };
#define _SENSOR_ID       'A'
//#define _SENSOR_ID       'B'
char  strTX[16];          // TX string buffer  


void setup()
{
  pinMode(4, OUTPUT);           // Set all unused digital pins to output, and set level to '0'.
  pinMode(5, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);

  pinMode(PDEBUG_LED, OUTPUT);  // Initialize LED driver pin.
  pinMode(PBAT_STAT, INPUT);    // Battery charger status monitoring pin.
    
  radio.begin();
  // Set the channel, carrier frequency = 2400 + [Channel No.] MHz
  radio.setChannel(100); // Use 2500 MHz, to avoid interference with WiFi 2.4GHz system.
  // Set data rate.
  radio.setDataRate(RF24_250KBPS); // Set to lower datarate, this is also most sensitive
                                   // at -94 dBm. Default is RF24_1MBPS, with lower
                                   // sensitivity.
                                   // If we use 2000 kbps baud rate, the channel interval 
                                   // should be 2 MHz to prevent interference.                                 
  
  // Set the address of the pipe. 
  // According to the datasheet, the auto-retry features's delay value should
  // be "skewed" to allow the RX node to receive 1 transmission at a time.
  // So, use varying delay between retry attempts and 15 (at most) retry attempts.
  #define   _DATAPIPE_NUMBER  2     // Valid value: 0 to 5

  #if  _DATAPIPE_NUMBER == 0
    radio.openWritingPipe(address[0]);
    radio.setRetries(0, 15);          // Delay 0 interval, max 15 retries.
  #elif _DATAPIPE_NUMBER == 1
    radio.openWritingPipe(address[1]);
    radio.setRetries(2, 15);          // Delay 2 intervals, max 15 retries.
  #elif _DATAPIPE_NUMBER == 2
    radio.openWritingPipe(address[2]);
    radio.setRetries(4, 15);          // Delay 4 intervals, max 15 retries.
  #elif _DATAPIPE_NUMBER == 3
    radio.openWritingPipe(address[3]);
    radio.setRetries(6, 15);          // Delay 6 intervals, max 15 retries.
  #elif _DATAPIPE_NUMBER == 4
    radio.openWritingPipe(address[4]);
    radio.setRetries(8, 15);          // Delay 8 intervals, max 15 retries.
  #else _DATAPIPE_NUMBER == 5
    radio.openWritingPipe(address[5]);
    radio.setRetries(10, 15);          // Delay 10 intervals, max 15 retries.
  #endif
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
  unSensor = analogRead(2);    // Read value of analog sensor.
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
  // Format:
  // [Device ID]+[Thousand]+[Hundred]+[Ten]+[Digit]
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
  strTX[0] = _SENSOR_ID;          // Sensor ID
  strTX[1] = unThousand + 0x30;   // Convert unThousand to ASCII.
  strTX[2] = unHundred + 0x30;
  strTX[3] = unTen + 0x30;
  strTX[4] = unDigit + 0x30;
  radio.write(&strTX, 5); // Transmit data string to receiver, 5 bytes.
  digitalWrite(PDEBUG_LED,LOW);   // Turn off debug LED.
  delay(250);
}
