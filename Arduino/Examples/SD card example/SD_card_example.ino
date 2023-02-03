/*
  SD card example

  This example shows how to use the STM32 Skipper's SD card capability.

  dependencies - STM32SD library, FATFS library

  for more info, go to https://github.com/Jack-Herron/STM32-Skipper

 */

#include <STM32SD.h> 

#define SD_DETECT_PIN PA15 // PA15 is the pin assigned to the SD detect switch on the Skipper. When an SD card is inserted, it goes LOW.

#define LED PD4 // PD4 is the pin assigned to the onboard LED

File dataFile;

void setup()
{
  pinMode(LED,OUTPUT);
  
  // turn on LED until card is detected (PA15 LOW).
  digitalWrite(LED,HIGH);
  
  while (!SD.begin(SD_DETECT_PIN))
  {
    delay(10);
  }
  delay(100);

  // card detected, turn off LED.
  digitalWrite(LED,LOW);

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  
  dataFile = SD.open("datalog.txt", FILE_WRITE);
  
  // if the file is available, seek to last position
  
  if (dataFile) {
    dataFile.seek(dataFile.size());
  }
}

void loop()
{
  // make a string to write to the card:
  String dataString = "THIS IS A TEST";

  if (dataFile) {
    dataFile.println(dataString);
    dataFile.flush(); // use flush to ensure the data was written
  }
  
  delay(1000); // wait one second, and write again.
}
