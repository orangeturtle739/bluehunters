---
title: 'Blue Hunters: Bluetooth RSSI Locator Robots'
author:
  - Jacob Glueck (jng55)
  - Jane Du (zd53)
  - Justin Cray (jgc232)

date: December 6, 2017
---


## Introduction
<!-- Give a short explanation of what was done. -->
We built 2 small cars which used Bluetooth Received Signal Strength Indicator (RSSI) measurements to navigate towards a stationary base station.
The cars and base station used a Bluetooth Low Energy (BLE) 4.0 module to take the measurements and a PIC32MX250 microcontroller. The cars also used a 3 axis magnetometer as as compass in order to reliably turn, as well as 2 micro 9 g servos to drive them.
Each unit was powered with 3 AA batteries.
Finally, the chassis and wheels of each car were 3D printed.


![bob](http://www.asciify.net/ascii/thumb/3585/minon.png)

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

### Hardware

#### Robot Design

![Robot chassis](report/frame.png){ width=50% }

![Caster wheel on back of robot](report/drag.png){ width=50% }

![Robot wheel](report/wheel.png){ width=50% }

### Software

```cpp
template <typename T>
void fun(const T& bar) {
  bar.foo();
}
```

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

<!-- Legal considerations. For instance, if you use a transmitter, you must discuss the appropriate FCC legal restrictions. -->

_The group approves this report for inclusion on the course website._

_The group approves the video for inclusion on the course youtube channel._

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

### Appendix A

The group approves this report for inclusion on the course website.

The group approves the video for inclusion on the course youtube channel.

### Appendix B
with commented program listing

### Appendix C
with schematics (you can download free software from expresspcb.com to draw schematics)
A block diagram is not a schematic.

### Appendix D
with cost details with all part numbers, vendors, and their price. This cost will include components as described in the Budget Considerations section.

| Name                     | Manufacturer Part Number | Vendor          | Vendor Part Number          | Quantity          | Unit Cost | Total Cost |
| ------------------------ | ------------------------ | --------------- | --------------------------- | ----------------- | --------- | ---------- |
| BLE 4.0 Module (TI CC2541)   | HM-10   | [Ebay](https://www.ebay.com/itm/AT-09-BLE-Bluetooth-4-0-Uart-Transceiver-Module-CC2541-Central-Switching-HM-10/142425748901?ssPageName=STRK%3AMEBIDX%3AIT&_trksid=p2057872.m2749.l2649)  | 142425748901  | 3 | $3.99  | $11.97  |
| FEETECH FS90R  (pack of 2) Continuous Rotation Robotic Servo | FS90R  | Amazon | B074BFQC3Q  | 2  | $12.39  | $24.78  |

### Appendix E
Appendix with a list of the specific tasks in the project carried out by each team member.

### References

Data sheets

*   [MPU-9250](https://www.invensense.com/wp-content/uploads/2015/02/PS-MPU-9250A-01-v1.1.pdf) and [its register map](https://cdn.sparkfun.com/assets/learn_tutorials/5/5/0/MPU-9250-Register-Map.pdf)
*   [3-axis Electronic Compass on IMU](https://www.akm.com/akm/en/file/datasheet/AK8963C.pdf)


Vendor sites


Code/designs borrowed from others

*   [Self-Balancing Robot](https://people.ece.cornell.edu/land/courses/ece4760/FinalProjects/f2015/dc686_nn233_hz263/final_project_webpage_v2/dc686_nn233_hz263/index.html) by Desmond Caulley (dc686@cornell.edu), Nadav Nehoran (nn233@cornell.edu), Sherry Zhao (hz263@cornell.edu). In particular, we borrowed extensively from their [i2c_helper.h](https://people.ece.cornell.edu/land/courses/ece4760/FinalProjects/f2015/dc686_nn233_hz263/final_project_webpage_v2/dc686_nn233_hz263/dc686_nn233_hz263/i2c_helper.h).

Background sites/paper


Template: <https://github.com/tajmone/pandoc-goodies/tree/af492fc217a95485ff73fc834ff2cd4fb7bd7148/templates/html5/github>
