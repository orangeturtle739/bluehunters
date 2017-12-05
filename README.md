---
title: 'Blue Hunters: Bluetooth RSSI Locator Robots'
author:
  - Jacob Glueck ([jng55](mailto:jng55@cornell.edu))
  - Jane Du ([zd53](mailto:zd53@cornell.edu))
  - Justin Cray ([jgc232](mailto:jgc232@cornell.edu))

date: December 6, 2017
---


## Introduction
We built 2 small cars which used Bluetooth Received Signal Strength Indicator (RSSI) measurements to navigate towards a stationary base station.
The cars and base station used a Bluetooth Low Energy (BLE) 4.0 module to take the measurements and a PIC32MX250 microcontroller. The cars also used a 3 axis magnetometer as as compass in order to reliably turn, as well as 2 micro 9 g servos to drive them.
Each unit was powered with 3 AA batteries.
Finally, the chassis and wheels of each car were 3D printed.

### Video

<iframe width="560" height="315" src="https://www.youtube.com/embed/g5H4cLJBA_Q?rel=0" frameborder="0" gesture="media" allow="encrypted-media" allowfullscreen></iframe>


## Design
<!-- Explain the approach you used for both software and hardware aspects of the assignment.
Be sure to include the design of tests whose outcome are convincing to the reader
(or to the instructor in the lab) that the requirements of the assignment have been met. -->
<!-- High level design:
rationale and sources of your project idea
background math
logical structure
hardware/software tradeoffs
Relationship of your design to available IEEE, ISO, ANSI, DIN, and other standards.
Discuss existing patents, copyrights, and trademarks which are relevant to your project.
Program/hardware design:
program details. Could someone else build this based on what you have written?
hardware details. Could someone else build this based on what you have written?
Be sure to specifically reference any design or code you used from someone else.
Things you tried which did not work -->

### Chassis

The robots are made from 4 3D printed pieces: 2 wheels, the frame, and the caster in the back.
The servos, a 3 AA battery holder, and a perfboard containing all the circuitry are mounted directly to the frame.

The robots where designed in [OpenSCAD](http://www.openscad.org/), and their source code is available in [our git repository](https://github.com/orangeturtle739/bluehunters/tree/master/cad).
There are three files, `frame.scad`, `drag.scad`, and `wheel.scad`, for each of the three parts. The following renderings show each part:

![Robot chassis](report/frame.png){ width=50% }

![Caster wheel on back of robot](report/drag.png){ width=50% }

![Robot wheel](report/wheel.png){ width=50% }


The parts were printed in ABS using [Maker Select 3D Printer v2](https://www.monoprice.com/product?c_id=107&cp_id=10724&cs_id=1072403&p_id=13860&seq=1&format=2) printers. All parts were printed
with a layer height of 0.3 mm, as there was no need for a smooth finish or high tolerances. The parts where sliced with [Cura](https://ultimaker.com/en/products/ultimaker-cura-software).

### Electronics

#### Bluetooth modules

The HM-10 bluetooth modules we bought off Ebay were fakes: they were not made by Jnhuamao [^china], and did not come with genuine Jnhuamao firmware.
Initially, we tried to use these chips, but quickly discovered that they did not behave according to the Jnhuamao data sheet (see [the data sheets section](#data-sheets)).
As the hardware on the fake chips is the same as that of the genuine chips, minus an external crystal.
However, the genuine firmware checks for the presence of the crystal, and works even without it. [^arduinoforums]
As such, we reprogrammed the chips with the genuine firmware according to an [Arduino forum post](http://forum.arduino.cc/index.php?topic=393655.msg2709528#msg2709528):

1.  We soldered wires to the programming pins on the breakout boards, and connected those pins to an [Arduino Teensy 3.2](https://www.pjrc.com/teensy/teensy31.html).
    We chose a Teensy because it is 3.3 V as opposed to 5, which would damage the CC2541.

    The pins were connected as follows:

      | Name          | CC2541 Pin | Arduino Pin |
      | ------------- | ---        | ----------- |
      | `DEBUG_CLOCK` | 7          | 5           |
      | `DEBUG_DATA`  | 8          | 6           |
      | `RESET_N`     | 11         | 4           |

    The layout of the HM-10 board is:

    ![](hm10_pins.png)
2.  We uploaded the [`CCLoader.ino` sketch](https://github.com/RedBearLab/CCLoader/blob/master/Arduino/CCLoader/CCLoader.ino) to the Arduino.
3.  Finally, we ran (in a Windows virtual machine) [`CCLoader.exe`](https://github.com/RedBearLab/CCLoader/tree/master/Windows).
    This program takes 3 arguments:
    ```bash
    CCLoader.exe <COM Port> <Firmware.bin> 0
    ```
    The firmware file came from the same Arduino form post, and can be found [here](http://forum.arduino.cc/index.php?action=dlattach;topic=393655.0;attach=183702).

There is also an excellent [YouTube video](https://www.youtube.com/watch?v=ez3491-v8Og), which explains the firmware flashing process.


After flashing genuine firmware onto the chip, the next step was to update the firmware to the latest version.
The firmware flashed onto the board was version 540, but Jnhuamao had (at the time we did this project) [released version 603](http://www.jnhuamao.cn/rom/HMSoft-10-2541-V603.zip). [^firmware]
They also provide [instructions on how to upgrade the firmware](http://www.jnhuamao.cn/HowToUpgradeFirmware_en.zip). The basic process is:

1.  Connect the HM-10 module to a computer using a 3.3 V FTDI to USB adapter.
    Then, use PUTTY to establish a serial session (9600 baud, 8N1).
    Send the chip `AT`; if it is connected properly, it will respond with `OK`.
2.  Send the chip `AT+SBLUP` to put it in firmware update mode.
    It will respond with `OK+SBLUP`. Terminate the PUTTY session.
3.  Run the `HMSoft.exe` program distributed in the firmware update download.
    We ran it in a Windows virtual machine because we did not trust it.
4.  Select the proper port and firmware file using the software, and hit "Load Image".
    The software should handle the rest!
5.  To make sure it worked, establish a serial connection again using PUTTY.
    Send `AT+VERS?` to query the chip for version information.

[^china]: Jnhuamao is the company which makes the HM-10 module, along with other Bluetooth modules. Their English website is: <http://www.jnhuamao.cn/bluetooth.asp?id=1>.
[^arduinoforums]: <http://forum.arduino.cc/index.php?topic=393655.msg2709528#msg2709528>.
[^firmware]: Jnhuamao's firmware download page <http://www.jnhuamao.cn/download_rom_en.asp?id=1#>.


### Software

The robots were programmed in C, using the [MPLAB X IDE](http://www.microchip.com/mplab/mplab-x-ide) v4.0, with the [XC32](http://www.microchip.com/mplab/compilers) v1.4 compiler and the [PIC 32 Legacy Peripheral Library (plib)](http://www.microchip.com/SWLibraryWeb/product.aspx?product=PIC32%20Peripheral%20Library). The full source code is available in [our git repository](https://github.com/orangeturtle739/bluehunters/tree/master/ble.X). The code is divided into 4 main units:

*   [`ble.c`](https://github.com/orangeturtle739/bluehunters/blob/master/ble.X/ble.c): contains all the functions for interacting with the BLE device over UART.
*   [`imu.c`](https://github.com/orangeturtle739/bluehunters/blob/master/ble.X/imu.c): contains all the functions for interacting with the IMU (which contained the magnetometer) over I2C.
*   [`servo.c`](https://github.com/orangeturtle739/bluehunters/blob/master/ble.X/servo.c): contains all the functions for interacting with the servos using PWM.
*   [`pt_cornell_1_2_2.c`](https://github.com/orangeturtle739/bluehunters/blob/master/ble.X/pt_cornell_1_2_2.c): contains the functions which were originally declared in the protothreads header file.
    However, since they were in the header file, if multiple source files included the header file, there would be linking errors due to duplicate definitions of symbols.
    Moving the protothreads functions to a separate file resolved this issue.

## Documentation
<!-- Include here drawings and program listings, together with any explanatory comments needed. -->

## Results
<!-- How fast was it? How accurate was it? What were the error ranges? -->
<!--
Results of the design:
This is the section which is most often lacking in student projects. Be specific!
If you took calibration data, then show it!
If you have images or video, include it.
speed of execution (hesitation, filcker, interactiveness, concurrency)
Be quantitative. Show graphs!
accuracy (numeric, music frequencies, video signal timing, etc)
Be quantitative. Show graphs!
how you enforced safety in the design.
interference with other people's designs (e.g. cpu noise, RF interference).
usability by you and other people (perhaps with special needs). -->

In most cases, at least 1 of the 2 robots successfully making it to the base station. However, it was not as reliable as we initially hoped. One of the main reasons for this was the noise in RSSI measurements.

We expected that RSSI would vary with distance according to the following relation:

$$\text{RSSI} = A - 10 n \log(d)$$

where $A$ and $n$ are RF propagation parameters in dBm, $d$ is distance in meters, and RSSI is the measured RSSI in dBm. [^signalstrength]
We experimented with RSSI measurements to determine how well they worked by taking 2 Bluetooth modules, and measuring the RSSI while changing the distance between them.
One remained stationary on the floor, and the other we moved away from it, 1 floor tile (each floor tile is a 1 foot square) at a time.
At each point, we took 3 RSSI measurements and averaged them. The graph below displays the results:

![](rssi-chart.png)

While the chip reported RSSI in units proportional to dBm, and we measured distances in feet, not meters, we could still use the above formula without worrying about unit conversions.
The constants $A$ and $n$, provided we determined them empirically based on the data, would encode the conversions.
As such, we fit the data using the above formula with $A=-48$ and $n=3$, resulting in the blue curve above.
While the general shape of the curve matches, there is significant noise in the averaged RSSI data.
Furthermore, when we tried to reproduce the measurements, we could not do so accurately -- it seemed to even depend on where our feet where!

[^signalstrength]: L. Peneda, A. Azenha and A. Carvalho, "Trilateration for indoors positioning within the framework of wireless communications," 2009 35th Annual Conference of IEEE Industrial Electronics, Porto, 2009, pp. 2732-2737.

    doi: 10.1109/IECON.2009.5415423

    URL: <http://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=5415423&isnumber=5414636>

## Conclusions
<!-- Useability, what you might have done differently, etc. Any comments concerning the assignment, including suggestions for improvement, excuses, and complaints. -->
<!-- Conclusions:
Analyse your design in terms of how the results met your expectations. What might you do differently next time?
How did your design conform to the applicable standards?
Intellectual property considerations.
Did you reuse code or someone else's design?
Did you use code in the public domain?
Are you reverse-engineering a design? How did you deal with patent/trademark issues.
Did you have to sign non-disclosure to get a sample part?
Are there patent opportunites for your project?
Are there publishing opportunities for your project?
Ethical considerations. Refering to the IEEE Code of Ethics, specifically explain how decisions you made or actions you took in this project were consistent with this Code of Ethics. I expect at least 200 words on this topic. A bulleted list will not be acceptable. -->


## Appendix

TOOD LINK TO PUBLIC GITHUB

<!-- Appendix with commented program listing
Appendix with schematics (you can download free software from expresspcb.com to draw schematics)
A block diagram is not a schematic.
Appendix with cost details with all part numbers, vendors, and their price. This cost will include components as described in the Budget Considerations section.
Appendix with a list of the specific tasks in the project carried out by each team member.
References you used:
Data sheets
Vendor sites
Code/designs borrowed from others
Background sites/paper -->

## Appendices

### Appendix A: Legal Considerations

*   The group approves this report for inclusion on the course website.
*   The group approves the video for inclusion on the course youtube channel.
*   The TI CC2541 meets FCC CFR47 the Part 15 requirements. [^tidatasheet]

[^tidatasheet]: See the top of page 8 of <http://www.ti.com/lit/ds/symlink/cc2541.pdf> for compliance information.


### Appendix B: Source Listing
Our source code can be found [on Github](https://github.com/orangeturtle739/bluehunters).
Each file can also be found below:

*   [`ble.c`](generated/ble.c.html)
*   [`ble.h`](generated/ble.h.html)
*   [`imu.c`](generated/imu.c.html)
*   [`imu.h`](generated/imu.h.html)
*   [`servo.c`](generated/servo.c.html)
*   [`servo.h`](generated/servo.h.html)
*   [`pt_cornell_1_2_2.c`](generated/pt_cornell_1_2_2.c.html)
*   [`pt_cornell_1_2_2.h`](generated/pt_cornell_1_2_2.h.html)
*   [`main.c`](generated/main.c.html)
*   [`main.h`](generated/main.h.html)
*   [`config_1_2_2.h`](generated/config_1_2_2.h.html)


### Appendix C: Schematics
with schematics (you can download free software from expresspcb.com to draw schematics)
A block diagram is not a schematic.

### Appendix D: Bill Of Materials

| Name                     | Manufacturer Part Number | Vendor          | Vendor Part Number          | Quantity          | Unit Cost | Total Cost |
|------------------------| ------------------------| ---------------|---------------------------|-----------------|---------|----------|
| BLE 4.0 Module (TI CC2541)   | HM-10   | [Ebay](https://www.ebay.com/itm/AT-09-BLE-Bluetooth-4-0-Uart-Transceiver-Module-CC2541-Central-Switching-HM-10/142425748901?ssPageName=STRK%3AMEBIDX%3AIT&_trksid=p2057872.m2749.l2649)  | 142425748901  | 3 | $3.99  | $11.97  |
| FEETECH FS90R  (pack of 2) Continuous Rotation Robotic Servo | FS90R  | Amazon | B074BFQC3Q  | 2  | $12.39  | $24.78  |
| Small board | -- | Lab rental | -- | 3 | $5.00 | $15.00 |
| PIC32MX250F128B | PIC32MX250F128B | Lab rental | -- | 3 | $5.00 | $15.00 |
| 6-inch Protoboard | -- | Lab rental | -- | 3 | $2.50 | $7.50 |

### Appendix E: Work Distribution

*   **Jacob:**
    *    hi
*   **Jane:**
    *    hi
*   **Justin:**
    *    hi

### References

#### Data sheets

*   [MPU-9250](https://www.invensense.com/wp-content/uploads/2015/02/PS-MPU-9250A-01-v1.1.pdf) and [its register map](https://cdn.sparkfun.com/assets/learn_tutorials/5/5/0/MPU-9250-Register-Map.pdf).
*   [3-axis Electronic Compass on IMU](https://www.akm.com/akm/en/file/datasheet/AK8963C.pdf).
*   [HM-10 (Bluetooth breakout module & firmware)](http://www.jnhuamao.cn/bluetooth40_en.zip). The ZIP contains the original data sheet, as well as other documentation. [MIT has a PDF](http://fab.cba.mit.edu/classes/863.15/doc/tutorials/programming/bluetooth/bluetooth40_en.pdf) available, which may me out of date, but is easier to get to.
*   [PIC32MX250](http://ww1.microchip.com/downloads/en/DeviceDoc/60001168J.pdf).


#### Vendor sites


#### Code/designs borrowed from others

*   [Self-Balancing Robot](https://people.ece.cornell.edu/land/courses/ece4760/FinalProjects/f2015/dc686_nn233_hz263/final_project_webpage_v2/dc686_nn233_hz263/index.html) by Desmond Caulley (dc686@cornell.edu), Nadav Nehoran (nn233@cornell.edu), Sherry Zhao (hz263@cornell.edu). In particular, we borrowed extensively from their [i2c_helper.h](https://people.ece.cornell.edu/land/courses/ece4760/FinalProjects/f2015/dc686_nn233_hz263/final_project_webpage_v2/dc686_nn233_hz263/dc686_nn233_hz263/i2c_helper.h).

#### Background sites/paper

#### Other

Template: <https://github.com/tajmone/pandoc-goodies/tree/af492fc217a95485ff73fc834ff2cd4fb7bd7148/templates/html5/github>
