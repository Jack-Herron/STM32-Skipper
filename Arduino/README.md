# Skipper + Arduino IDE

---

### Arduino Compatabilty

#### One of the great features of the Skipper is its ability to work with the Arduino IDE. A few downloads are required to get you on your way.

---

### What is the purpose of this branch?

#### This directory includes examples and libraries to get you started using your Skipper with Arduino.

---

### First Steps:

---

1. Open Arduino

2. Go to Arduino > Preferences

3. under the "Additional Boards Manager URLs:" box, paste:

> https://github.com/stm32duino/BoardManagerFiles/raw/main/package_stmicroelectronics_index.json

4. click ok and close preferences

5. go to Tools > Board > Boards manager...

6. scoll through until you find "STM32 MCU based boards"

7. Click "Install"

8. close Boards Manager

9. go to Tools > board > STM32 Boards Groups > select "generic STM32F4 series"

10. go to Tools > board part number > select "generic STM32F407zetx" 

11. Select upload method

    1. to use SWD port via stlink, go to Tools > Upload Method > "STM32CubeProgrammer(SWD)" (note, for this method, you do not need to select the port)

    2. to use micro usb port, go to Tools > Upload Method > click "STM32CubeProgrammer(DFU)", then select the port. For this you must switch the boot switch to boot mode(LED ON), and press reset prior to upload.

    3. to ustilze the serial port, click USB support (if available) > CDC) (note, requires micro USB to be connected)

12. write your code and click "upload"
    

---

#### For instructions regarding specific libraries and functionality, refer to the examples folder.