---
title: 'Blue Hunters: Bluetooth RSSI Locator Robots'
subtitle: '_ECE 4760, Fall 2017_'
author:
  - Jacob Glueck ([jng55](mailto:jng55@cornell.edu))
  - Jane Du ([zd53](mailto:zd53@cornell.edu))
  - Justin Cray ([jgc232](mailto:jgc232@cornell.edu))

date: December 6, 2017
---


## Introduction
We built 2 small cars which used Bluetooth Received Signal Strength Indicator (RSSI) measurements to navigate towards a stationary base station.
The cars and base station used a Bluetooth Low Energy (BLE) 4.0 module to take the measurements and a PIC32MX250 microcontroller. The cars also used a 3 axis magnetometer as as compass in order to reliably turn, as well as 2 micro 9 g servos to drive.
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

The robots where designed in [OpenSCAD](http://www.openscad.org/), and their source code is available in [our git repository](https://github.com/orangeturtle739/bluehunters/tree/master/cad) and [below](#appendix-b).
There are three files, [`frame.scad`](generated/frame.scad.html), [`drag.scad`](generated/drag.scad.html), and [`wheel.scad`](generated/wheel.scad.html), for each of the three parts. The main modules are defined in [`main.scad`](generated/main.scad.html), and the other files just instantiate them. The following renderings show each part:

![Robot chassis](frame.png){ width=50% }

![Caster wheel on back of robot](drag.png){ width=50% }

![Robot wheel](wheel.png){ width=50% }


The parts were printed in ABS using [Maker Select 3D Printer v2](https://www.monoprice.com/product?c_id=107&cp_id=10724&cs_id=1072403&p_id=13860&seq=1&format=2) printers. All parts were printed
with a layer height of 0.3 mm, as there was no need for a smooth finish or high tolerances. The parts where sliced with [Cura](https://ultimaker.com/en/products/ultimaker-cura-software).

### Pictures

Full system (base station and 2 robots):

![](full_system.jpg){ width=50% }

Multiple views of the robots:

------------------------------ ------------------------------
![Top view](top_view.jpg)      ![Side view](side_view.jpg)
![Front view](front_view.jpg)  ![Inside](inside_view.jpg)
------------------------------ ------------------------------

### Electronics

#### Bluetooth modules

The HM-10 bluetooth modules we bought off Ebay were fakes: they were not made by Jnhuamao [^china], and did not come with genuine Jnhuamao firmware.
However, they did have a real TI CC2541 chip.
We realized they were fakes when they did not behave according to the Jnhuamao data sheet (see [the data sheets section](#data-sheets)).
Luckily the hardware on the fake chips is the same as that of the genuine chips, minus an external crystal, and the genuine firmware checks for the presence of the crystal, and works even without it. [^arduinoforums]
As such, we reprogrammed the chips with the genuine firmware according to an [Arduino forum post](http://forum.arduino.cc/index.php?topic=393655.msg2709528#msg2709528):

1.  We soldered wires to the programming pins on the breakout boards, and connected those pins to an [Arduino Teensy 3.2](https://www.pjrc.com/teensy/teensy31.html).
    We chose a Teensy because it is 3.3 V as opposed to 5, which would damage the 3.3 V CC2541.

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

The robots were programmed in C, using the [MPLAB X IDE](http://www.microchip.com/mplab/mplab-x-ide) v4.0, with the [XC32](http://www.microchip.com/mplab/compilers) v1.4 compiler and the [PIC 32 Legacy Peripheral Library (plib)](http://www.microchip.com/SWLibraryWeb/product.aspx?product=PIC32%20Peripheral%20Library). The full source code is available in [our git repository](https://github.com/orangeturtle739/bluehunters/tree/master/ble.X) and [below](#appendix-b). The code is divided into 5 main units:

*   [`ble.c`](generated/ble.c.html): contains all the functions for interacting with the BLE device over UART.
*   [`imu.c`](generated/imu.c.html): contains all the functions for interacting with the IMU (which contained the magnetometer) over I2C.
*   [`servo.c`](generated/servo.c.html): contains all the functions for interacting with the servos using PWM.
*   [`main.c`](generated/main.c.html): the main code.
*   [`pt_cornell_1_2_2.c`](generated/pt_cornell_1_2_2.c.html): contains the functions which were originally declared in the protothreads header file.
    However, since they were in the header file, if multiple source files included the header file, there would be linking errors due to duplicate definitions of symbols.
    Moving the protothreads definitions to a separate file resolved this issue.

#### BLE

The BLE module used UART, at 9600 baud 8N1. We used `UART1` on the PIC for communicating with the BLE module, and used `UART2` for communicating with the computer (for debugging).
The BLE files define a series of macros and `PT_THREAD` functions for printing characters and reading lines in a non-blocking fashion using protothreads. All of the `PT_THREAD` functions can be spawned with `PT_SPAWN` to run them.

The [`main.c`](generated/main.c.html) file contains a thread which does all the communication with the Bluetooth chip using these functions. The commands it sends to the chip are:

*   `AT+RESET`: resets the chip to ensure it is in a clean state before receiving other commands.
*   `AT+IBEA1`: enables the iBeacon functionality of the chip (sets it to 1; `AT+IBEA0` would disable it by setting it to 0).
    This allows the chip to be found with an RSSI scan.
    After setting the value, we query it with `AT+IBEA?` and send the result over the other serial port to a computer for debugging.
*   `AT+ROLE0` or `AT+ROLE1`: sets the role to either peripheral (0) or central (1).
    The base station is set to peripheral, and the 2 robots are set to central.
    Peripheral means the device will respond in inquiries from a central device.
    This allows it to be discovered during an RSSI scan.
    After setting the value, we query it with `AT+ROLE?` and send the result over the other serial port to a computer for debugging.
*   `AT+IMME0` or `AT+IMME1`: sets the work state of the device to either actively listening for Bluetooth signals (0), or only acting when it receives a serial command (1).
    Once again, the base station is set to 0: it needs to listen for signals and respond.
    The robots are set to 1, as the chips need to initiate scan requests when they receive the command over serial.
    After setting the value, we query it with `AT+IMME?` and send the result over the other serial port to a computer for debugging.
*   `AT+NAME%s`: sets the name of the chip (which is visible when scanning) to `%s` (For example, `AT+NAMEPIRATE` names the chip `PIRATE`).
    We give all the chips unique names to make debugging easier.
    After setting the value, we query it with `AT+NAME?` and send the result over the other serial port to a computer for debugging.
*   `AT+SHOW3`: configures the device to advertise both its name and RSSI when scanning.
*   `AT+ADDR?`: queries the device for its hardware address. We recorded the hardware device of each chip, as when doing RSSI scans, the results are reported by hardware address.
*   `AT+DISI?`: performed only on the robots, causing a discovery scan. The result of the scan is a bunch of lines of the form:
    ```
    OK+DISC:00000000:00000000000000000000000000000000:0000000000:6832A3801EBE:-080
    ```
    The second to last token, `6832A3801EBE`, is the hardware address of the discovered device, and the last token, `-080`, is the measured RSSI.
    The chip will transmit a line for each device it finds ("line" is a misnomer as it does not separate them with any characters), followed by `OK+DISCE`.

One interesting thing to note about the chip is that commands do not have to end with newlines or carriage returns. However, if sent, the chip will ignore them.

#### IMU
The PIC commmunicates with the IMU via I2C. The IMU includes a breakout board for the QFN MPU-9250 module, which itself includes 2 dies. One contains the 3-axis gyroscope and 3-axis accelerometer, which were not used in this project, and the other die is the AK8963 3-axis magnetometer (compass).

It is connected to the rest of the MPU module via an auxillary I2C bus, so it is not connected to the MPU's  main I2C bus by default. While the accelerometer and gyroscope registers can be read after powering up the IMU, the compass also needs pass-through mode to be enabled on the IMU to make it an accessible slave on the I2C bus.

Some useful functions as defined in [`imu.h`](generated/imu.h.html) are described below:

*   `void imu_init()`: Initializes the MPU-9250, including configuring the chip to allow reading the compass.
*   `int imu_get_heading()`  Returns the heading of the robot as a value between -180 and 180.
    The compasses were not completely calibrated to find magnetic north; it only ensures angles are correct relevant to past headings.
*   `void imu_mag_read_data(int * destination)` Fetches compass readings; saves register values into `destination` in the form `[x, y, z]`.
*   `int angle_diff(int source, int target)` Gets the difference between two angles in degrees to account for discontinuity between -180 and 180 degrees.
*   `int degree(int deg)`: Offsets degree values so that they fall within the range -180 to 180 degrees.

Initializing the IMU involves opening the I2C module, and then configuring the IMU.

1.  We open the `I2C2` module; `I2C1` uses pins already used by the connections to the Bluetooth module.
    We open it with the baud rate generator value `BRG = (Fpb / 2 / baudrate) - 2 = 4e7 / 2 / 4e5 - 2 = 48`, as specified for `OpenI2C2()` in the [peripheral libraries](#references).
2.  Pass through is enabled, interrupts for data ready are enabled, the IMU as an I2C master function is disabled, and the sensor is powered up.

This enables the PIC to talk to the AK8963. The AK8963 has several modes of operation, and the chip must be set to power-down mode before switching to other modes. We read the IMU with single measurement mode, as specified below:

![IMU single measurement mode](imu_single_measurement.png)

1.  Set the compass to single measurement mode in 14 bit resolution.
2.  Read the 6 data registers (X low, X high, Y low, Y high, Z low, Z high)
3.  Read the Status 2 register to check for magnetic sensor overflow. Without reading this register, the read is not considered complete and further reads will fail.
4.  Wait; if the IMU is read too frequently, it will not have enough time to take measurements.

Additional helper methods used in I2C were defined in [`imu.c`](generated/imu.c.html). These are largely based on the files from the [self-balancing robot](#code-and-designs-borrowed-from-others).

*   `char i2c_read_device(char device, char address)` Reads the data from a single register at `address`
*   `void i2c_write_byte(char device, char address, char data)` All configurations used in this project involved writing single bytes of data.
*   `void i2c_wait(int cnt)` Writes 2 nops; reads require time to return a value, and calling reads consecutively.

In order to calibrate the compass, the robots spun in place when powered on. They recorded the maximum and minimum values for each axes, and used that data to scale and center the magnetometer readings.

#### Servos

TODO JUSTIN WRITE ABOUT THE SERVOS

#### Protothreads

While the provided protothreads library was very helpful, it did not work correctly when used in multiple compilation units.
The reason for this was that it defined variables and functions in the header file, so if multiple source files included [`pt_cornell_1_2_2.h`](generated/pt_cornell_1_2_2.h.html), linking would fail with duplicate symbol definitions.
Furthermore, if multiple files included [`config_1_2_2.h`](generated/config_1_2_2.h.html), linking would fail with the obscure error:
```
section `.config_BFC00BF4' will not fit in region `config2'
```
This happened because if multiple compilation units included `#pragma config` directives (which they did by including a `#include "config_1_2_2.h"` directive), then the linker would try to assign too many symbols to the configuration region.

The fix to this was twofold:
1.  We modified protothreads to have both a header and source file, with definitions only in the source file.
1.  We only included [`config_1_2_2.h`](generated/config_1_2_2.h.html) in the main C file, and moved all macro definitions to the protothreads header.

The updated versions of protothreads can be found in [Appendix B](#appendix-b).

#### Gradient Descent

The algorithm for deciding what path to follow is a basic version of gradient descent. The following image represents the decision-making fsm, where the starting state is **Measure rssi twice, take average**.

![](grad_desc.png)

We also implemented and tested the following improved version that allows for correction; a car that has just moved forward and detected a weakened signal does not know whether the beacon to its left or right. If after turning, the signal is still weaker, it has picked the wrong turn. This decision process corrects this:

![](turn_correction.png)

It did not prove much more accurate than randomized gradient descent, largely due to noisy readings from IMU and Bluetooth signal strength.

### Testing

#### Bluetooth

Once we flashed the Bluetooth chips with the proper firmware, we used the [LightBlue](https://itunes.apple.com/us/app/lightblue-explorer/id557428110?mt=8) app to test communications.
Using the app, we could discover the Bluetooth chips, and send and receive data.
By initially using a the iPhone app, which was known to work, we could debug the Bluetooth chips in isolation.
Once that worked, we had the chips communicate with each other.
Finally, we had the chips take RSSI measurements.

We wrote code that parsed the chip output for the RSSI and saved the raw data. We then plotted and tried to derive an RSSI distance correlation which proved to be unsuccessful. See the graph in the Results section. We were able to use this data however to derive the appropriate signal strength for our cutoff threshold. This also validated the effectiveness of RSSI as an approximate metric for distance measurement.


#### IMU
- Basic I2C: Basic testing was required to ensure I2C communication with the chip was working. This included testing the correctness of the I2C read and write functions (including the order of starts, restarts, and idles in accordance with the I2C protocol and the chip datasheets), as well basic parameters like the address of the MPU-9250 and its registers.
- Compass I2C: This was more complex, and mostly involved testing the usage of the modes of both the MPU-9250 (IMU module) and the AK8963 (the compass itself). Performance testing was also important here, as the compass would not allow rapid reads in quick succession; we inserted nops in the `imu_mag_read_data(int * destination)` for this reason.
- Compass calibration: We tested incrementally, first using hard-coded maximum and minimum observed values outputted from the IMU, and then with values taken from a self-calibration in which the robot would spin in a circle, and the PIC would record the largest and smallest values it saw on the X- and Y-axes.

#### Whole system
- Some subsystem testing was required for the servo with the IMUs, for turning/driving straight with feedback. We debugged by reading in IMU values via UART and observing the robot's turns.
- We tested extensively with the beacon and two hunters in different environments. Most of our initial testing was in a large, open space with a few metal tables on the periphery. We later tested in the lab in the Digital Lab (Phillips 238), and in the hallway where our demo took place. The confined space of the inside of the lab and hallway, as well as the presence of more reflective bodies (metal doors, tables, people) negatively impacted the performance of our hunters, and it was necessary to recalibrate the hunters to work better in the demo environment.

#### Servos
We tested the functionality of servo code by testing individual functionality on both the hardware and software side. We tested the servo code in stages, first implementing control at a fixed speed and then implementing directional motion and lastly turning. This allowed us to verify our parts of the code and the functionality of the servos.

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
Furthermore, when we tried to reproduce the measurements, we could not do so accurately -- it seemed to even depend on where our feet where! See [Appendix B](#rssi-data) for the source data for the table.

Despite how noisy the RSSI measurements where, the robots were still able to perform a reasonably accurate gradient descent. In most cases, at least one of the 2 robots would find the beacon in a matter of minutes.

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

This assignment was an interesting exploration into short-range distance determination using Bluetooth, a generally unconventional approach. We were warned beforehand about the difficulties in using Bluetooth RSSI, a measurement particularly susceptible to multipath interference, as a distance sensor -- both in the papers and reference materials researched before implementation, as well as by course staff in the early stages of our project. We worked with the course staff to simplify and stratify our project into workable milestones. Our final project is a reduced version of our original plan.

Our hunting bots worked reliably when they stayed within roughly 1 m of the beacon. After this, they entered the land of shallow gradients: the signal strength from the beacon (already noisy) would not change very much, and often only due to noise. The hunting bots normally could never recover from this. Our bots also worked better in larger, more open spaces, preferably without metal structures like tables or doors nearby. Both of these are reasonable given the operating range of BLE 4.0.

We reused some of the code from the [Self-Balancing Robot](https://people.ece.cornell.edu/land/courses/ece4760/FinalProjects/f2015/dc686_nn233_hz263/final_project_webpage_v2/dc686_nn233_hz263/index.html) as [cited](#code-and-designs-borrowed-from-others) in  our references section, and explained in [IMU](#imu). The rest of the design is ours, and is not an infringement on intellectual property. We also used the Bluetooth firmware flashing technique we discovered on the [Arduino forums](http://forum.arduino.cc/index.php?topic=393655.msg2709528#msg2709528).

We did not reverse-engineer any technology, and did not encounter any issues with trademarks or patents. We did not have to sign non-disclosure to get a sample part. We hope to further polish our findings and report, and will aim to publish them in a print magazine, ideally as a part of ECE 4920.

There are many potential avenues for improvements or further development. In addition to more circumstantial difficulties encountered during the project work period (such as extremely uneven servos), two areas of potential improvement are as follows:

- *Communication between the two robots*. The strengths of Bluetooth are certainly not distance measurement by RSSI, but the reliable communication between modules. It would be straightforward for one hunting robot to inform the other whether it believes it is approaching the beacon or not. In the simplest case, a hunting robot that is approaching, or already at, the beacon can provide a second point of reference for a currently hunting robot.
- *More complete usage of IMU*. A lot of development time was spent simply on getting the PIC to compass communication running, and not much time was spent on calibrating the sensor or processing the data. Given more time, we could continue to work towards dead reckoning of the mobile robots. This, paired with inter-swarm communication, would make for a much more sophisticated and likely much more efficient system. (Of course, this does not resolve the shallow gradients problem -- but it would allow the approach to the beacon to be much faster.)



### IEEE Code of Ethics

Our project, as well as the design and creation project thereof,  adheres to the [IEEE Code of Ethics](https://www.ieee.org/about/ethics.html). The project itself is relatively small and physically self-contained; it did not have any immediate impact on the environment or safety and health of potential users, we the creators, or any others. We chose the project's topic out of interest and for a challenge, without any conflict of interest. Our report is honest and realistic; none of our data is fabricated, and our conclusions reflect our hard findings. We did not accept any form of bribery.

The project investigated the use of an emerging and popular technology (BLE) in a new application (distance sensing). We challenged ourselves while not attempting tasks we were not qualified to do. We consistently received and built upon advice from our TAs and professor Bruce Land, and cite the work we used in the basis of our project.

Additionally, neither we nor our project presented any act of discrimination, and our project does not injure others in any way. We assisted our colleagues (both fellow group members and other classmates) when the occasion arose, such as by offering help debugging or lending a spare module before a demo.

Overall, our project complies with the IEEE Policies, 7.8 Code of Ethics.
<!-- in conclusion, we aren't jerks -->

## Appendices

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

### Appendix A: Legal Considerations

*   The group approves this report for inclusion on the course website.
*   The group approves the video for inclusion on the course youtube channel.
*   The TI CC2541 meets FCC CFR47 Part 15 requirements. [^tidatasheet]

[^tidatasheet]: See the top of page 8 of <http://www.ti.com/lit/ds/symlink/cc2541.pdf> for compliance information.


### Appendix B: Source Listing
Our source code can be found [on Github](https://github.com/orangeturtle739/bluehunters).
Each file can also be found below:

#### Code

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

#### CAD

##### Sources

*   [`main.scad`](generated/main.scad.html)
*   [`frame.scad`](generated/frame.scad.html)
*   [`wheel.scad`](generated/wheel.scad.html)
*   [`drag.scad`](generated/drag.scad.html)
*   [`concept.scad`](generated/concept.scad.html)

##### STL

*   [`frame.stl`](stl/frame.stl)
*   [`wheel.stl`](stl/wheel.stl)
*   [`drag.stl`](stl/drag.stl)

#### RSSI Data

| Distance (feet) | RSSI A | RSSI B | RSSI C | RSSI Average | Expected RSSI |
| --------------- | ------ | ------ | ------ | ------------ | ------------- |
| 1               | -48    | -48    | -48    | -48.00       | -48.00        |
| 2               | -56    | -58    | -52    | -55.33       | -57.03        |
| 3               | -66    | -66    | -68    | -66.67       | -62.31        |
| 4               | -64    | -68    | -65    | -65.67       | -66.06        |
| 5               | -60    | -70    | -64    | -64.67       | -68.97        |
| 6               | -80    | -80    | -76    | -78.67       | -71.34        |
| 7               | -80    | -79    | -65    | -74.67       | -73.35        |
| 8               | -68    | -66    | -72    | -68.67       | -75.09        |

### Appendix C: Schematics

![](schematic.png)

### Appendix D: Bill Of Materials

| Name                     | Manufacturer Part Number | Vendor          | Vendor Part Number          | Quantity          | Unit Cost | Total Cost |
|------------------------| ------------------------| ---------------|---------------------------|-----------------|---------|----------|
| BLE 4.0 Module (TI CC2541)   | HM-10   | [Ebay](https://www.ebay.com/itm/AT-09-BLE-Bluetooth-4-0-Uart-Transceiver-Module-CC2541-Central-Switching-HM-10/142425748901?ssPageName=STRK%3AMEBIDX%3AIT&_trksid=p2057872.m2749.l2649)  | 142425748901  | 3 | $3.99  | $11.97  |
| FEETECH FS90R  (pack of 2) Continuous Rotation Robotic Servo | FS90R  | [Amazon](https://www.amazon.com/gp/product/B074BFQC3Q/ref=oh_aui_detailpage_o06_s00?ie=UTF8&psc=1) | B074BFQC3Q  | 2  | $12.39  | $24.78  |
| HiLetgo 9-Axis 9 DOF 16 Bit Gyroscope Acceleration Magnetic Sensor | MPU-9250  | [Amazon](https://www.amazon.com/HiLetgo-Gyroscope-Acceleration-Accelerator-Magnetometer/dp/B01I1J0Z7Y/ref=redir_mobile_desktop?_encoding=UTF8&dpID=51nl2fcMh6L&dpPl=1&keywords=mpu%209250&pi=AC_SX236_SY340_QL65&qid=1512564044&ref=plSrch&ref_=mp_s_a_1_3&sr=8-3)  | B01I1J0Z7Y | 2 | $8.49  | $16.98 |
| Small board | -- | Lab rental | -- | 3 | $5.00 | $15.00 |
| PIC32MX250F128B | PIC32MX250F128B | Lab rental | -- | 3 | $5.00 | $15.00 |
| 6-inch Protoboard | -- | Lab rental | -- | 3 | $2.50 | $7.50 |
| Male header pins | -- | Lab | -- | 129 | $0.05 | $6.45 |

**Total:** $97.68

### Appendix E: Work Distribution

*   **Jacob:**
    *   Bluetooth firmware hacking
    *   Bluetooth UART code
    *   CAD design & 3D printing
*   **Jane:**
    *   I2C IMU code
    *   Robot motion code and gradient descent algorithm
*   **Justin:**
    *   Servo control code
    *   Board assembly (3 small boards and our 3 of our protoboards)
    *   Schematic

### References

#### Data sheets

*   [MPU-9250](https://www.invensense.com/wp-content/uploads/2015/02/PS-MPU-9250A-01-v1.1.pdf) and [its register map](https://cdn.sparkfun.com/assets/learn_tutorials/5/5/0/MPU-9250-Register-Map.pdf).
*   [3-axis Electronic Compass on IMU](https://www.akm.com/akm/en/file/datasheet/AK8963C.pdf).
*   [HM-10 (Bluetooth breakout module & firmware)](http://www.jnhuamao.cn/bluetooth40_en.zip). The ZIP contains the original data sheet, as well as other documentation. [MIT has a PDF](http://fab.cba.mit.edu/classes/863.15/doc/tutorials/programming/bluetooth/bluetooth40_en.pdf) available, which may me out of date, but is easier to get to.
*   [PIC32MX250](http://ww1.microchip.com/downloads/en/DeviceDoc/60001168J.pdf).

#### Code and Designs borrowed from others

*   [Self-Balancing Robot](https://people.ece.cornell.edu/land/courses/ece4760/FinalProjects/f2015/dc686_nn233_hz263/final_project_webpage_v2/dc686_nn233_hz263/index.html) by Desmond Caulley (dc686@cornell.edu), Nadav Nehoran (nn233@cornell.edu), Sherry Zhao (hz263@cornell.edu). In particular, we borrowed extensively from their [i2c_helper.h](https://people.ece.cornell.edu/land/courses/ece4760/FinalProjects/f2015/dc686_nn233_hz263/final_project_webpage_v2/dc686_nn233_hz263/dc686_nn233_hz263/i2c_helper.h).

#### Other

Website template: <https://github.com/tajmone/pandoc-goodies/tree/master/templates/html5/github>.
