# Skipper + Arduino IDE

---

### Arduino Compatabilty

---

#### One of the great features of the Skipper is its ability to work with the Arduino IDE. A download is required to get you on your way.

---

### What is the purpose of this branch?

---

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

    1. to use SWD port via stlink, go to Tools > Upload Method > "STM32CubeProgrammer(SWD)"

    2. to use micro usb port, go to Tools > Upload Method > "STM32CubeProgrammer(DFU)" for this you must switch the boot switch to boot mode(LED OFF) prior to upload.

12. write your code and click "upload"
    (note, for me the tools > port tab does not show any ports, try uploading with no port selected)
