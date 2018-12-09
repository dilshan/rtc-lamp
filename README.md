# RTC based automatic LED lamp

This is real-time clock based automatic LED lamp which we originally designed to use as night light. This lamp can programmed to turn on and off at the specific time of the day. For example, it can program to turn on at 6 PM on each day and to turn off at 4 AM next day.

The core component of this project is PIC16F883 MCU and it's firmware is developed using *MikroC Pro for PIC*. We select this MCU because of it's 7 KB flash memory, I2C, UART, E2PROM and built-in 8-bit and 16-bit timers. In this system we use DS1307 RTC because of it's availability in the market and lower external component count.

### LED module

This lamp is designed to work with commonly available 7W LED panels. In our prototype design we use 7W 24V warm-white LED module to test this system. To drive other LED modules change value of the R5 resistor of the current limiter circuit.

LED modules, heat-sink and lamp enclosures are now commonly available in the market but make sure to use good quality 24V - 28V LED module with this circuit. During our prototype we found lot of defective LED modules in the market.

### Battery backup

This unit is designed to work with 24V - 3A power supply unit. 3V button cell is required as back power source of DS1307 RTC. Supplied PCB is designed to work with *CR2025* or *CR2032* type button cells.

### Circuit assembly

For this project we use commonly available, less expensive electronic components and PCB is designed for "through-hole" type components. 

Both Q1 (STP55N06) and U4 (LM7805) required heatsinks. If heatsink is shared between both devices make sure to use proper isolation in either Q1 or U4 (or both).

J3 to RS232 pin mapping for DB-9 connector is listed in below:

| J3    | DB9   |
|-------|-------|
| Pin 1 | Pin 2 |
| Pin 2 | Pin 3 |
| Pin 3 | Pin 5 |

### Firmware

PIC16F883 firmware is developed using *MikroC Pro for PIC*. Compiled HEX file is available at release section of this repository.

### Programming the system

This system is designed to program using RS232 serial port. User can modify system time, light start and end times by connecting this system to any computer through RS232 serial port. To program and view system information user can use any serial terminal application like *minicom*, *putty*, etc. with baud rate of 9600.

Once system is connected to the host PC it can switch to programming mode by sending *Enter* or *Space* key stroke through the serial terminal.

If host PC is not equipped with serial port, use USB to RS232 converter cable / module. During our tests we use couple of USB to RS232 converters (such as *FT232* and *CH340*) without any issues.

### Bill of materials

| Id               |   Component                                  | Qty |
|------------------|----------------------------------------------|-----|
| U1               | PIC16F883-IP                                 | 1   |
| U2               | DS1307                                       | 1   |
| U3               | MAX232                                       | 1   |
| U4               | LM7805                                       | 1   |
| Q1               | STP55NF06                                    | 1   |
| Q2               | 2SC945                                       | 1   |
| D1               | 6A10                                         | 1   |
| Y1               | 32.768 Crystal                               | 1   |
| BT1              | CR2025 or CR2032 battery holder with battery | 1   |
| R1, R2, R3       | 4.7K                                         | 3   |
| R4, R6           | 10 K                                         | 2   |
| R5               | 2.2R - 3W                                    | 1   |
| C1               | 4.7 MFD / 16V                                | 1   |
| C2, C3, C4, C5   | 10MFD / 16V                                  | 4   |
| C6               | 0.33MFD / 63V                                | 1   |
| C7, C9, C10, C11 | 0.1 MFD / 63V                                | 4   |
| C8               | 470 MFD / 35V                                | 1   |
| F1               | 12V 1A Fuse                                  | 1   |
| L1               | VK200 - 3 turns                              | 1   |
| BZ1              | KXG1203C or 5V DC PCB mount buzzer           | 1   |
| J1               | 2.54mm - 2 Way, 1 Row PCB header             | 1   |
| J2               | 2.54mm - 3 Way, 1 Row PCB header             | 1   |
| J3, J4           | 5.08mm - 2 Way PCB terminal block            | 2   |
|                  | 7W 24V/28V LED module and heatsink           | 1   |
|                  | 3mm Red LED                                  | 1   |
|                  | DC 50V Rocker switch                         | 1   |
|                  | DB9 Female Connector                         | 1   |
|                  | Panel Mount DC Power Socket                  | 1   |
|                  | TO-220 heatsink                              | 2   |

### Datasheets

[PIC16F883 - 28-Pin Enhanced Flash-Based, 8-Bit. CMOS Microcontrollers with. nanoWatt Technology.](https://octopart.com/pic16f883-e/sp-microchip-484880?r=sp&s=gf3fl0DDQqu7zfi6Mu1mTg)
[DS1307 - Serial Real-Time Clock](https://octopart.com/ds1307+-maxim+integrated-998915?r=sp&s=hPqOpBiTSiaPCVOwAzWNwA)
[MAX232 - 5V-Powered, Multichannel RS-232 Drivers/Receivers](https://octopart.com/max232cpe-maxim+integrated-55450068?r=sp&s=dPfpbiRnRZ2jJNLkmzmRFQ)
[LM7805 - 3-Terminal, 1A Positive Voltage Regulator.](https://octopart.com/lm7805ct/nopb-texas+instruments-23919374?r=sp&s=__mIdAy-R1qIqddPHdqk4g)
[STP55NF06 - N-CHANNEL 60V - 50A Power MOSFET](https://octopart.com/stp55nf06-stmicroelectronics-335317?r=sp&s=E7hGFTBFSqqGf4v1k2f-rA)
[2SC945 - NPN Silicon Transistor](https://octopart.com/2sc945a-a-renesas-66523934?r=sp&s=pYvJG4XtT0ekf6kEQFJn-A)
[6A10 - 1000 V - 6 A High Surge Current Silicon Rectifier](https://octopart.com/6a10-t-diodes+inc.-55426548?r=sp&s=7SjnoxDITDCXdY6NPBmLIg)

### License

This is an open hardware project and all it's source code are released under the terms of [MIT license](https://opensource.org/licenses/MIT). Schematics, PCB designs and other resources are released under the terms of [Creative Commons Attribution 4.0 international](https://creativecommons.org/licenses/by/4.0/) license.

