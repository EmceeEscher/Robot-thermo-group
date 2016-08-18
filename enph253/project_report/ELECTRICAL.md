# ENPH 253 - Group 14 (MURPH) - Project Report
*Jacob Brunette*, *Dilyn Fullerton*, *Ryan Watt*, *Dickson Yao*  
*Summer 2016*

[[previous (*Mechanical*)](./MECHANICAL.md) | [main](./REPORT.md#design) |
[next (*Software*)](./SOFTWARE.md)]

### Electrical Design
---
As an autonomous vehicle, our robot must use many sensors to enable its maneuvering around the roads, and its awareness of the surroundings. For an organized, easy to debug layout we laid all of the PCBs in the electrical systems onto a single plane, enclosed in the chassis.

Four infrared (IR) reflectance sensors were used to enable tape following and intersection detection. IR light that is reflected into the sensor distinguish between a white surface and black tape for tape following. However, due to limit in the number of analog inputs, a comparator circuit was designed to produce a digital input instead.

To avoid conflicts with immediate surroundings, such as other robots and obstructions, switches are used to detect collisions. Pairs of switches were placed in parallel for pin efficiency on the microprocessor.

As per competition rules, IR light (1 kHz) is emitted near passengers to signal their location and availability, and IR (10 kHz) is also emitted at the drop-off point. Thus, IR sensors are necessary and its circuitry require a frequency filter, an amplifier and a peak detector. Two IR sensors were used to search for peripheral signals to follow; then two shielded IR sensors are used to precisely position the robot to pick-up passengers.

Power rail PCBs stretch the length of the robot, and female headers branched off to supply power to the PCBs. Multiple power rails were used to avoid disturbances in the sensor readings from motor noise. Furthermore, decoupling capacitors were placed along the rails to mitigate motor noise.

The use of multiple sensors governed the decision making in our algorithm and led to our success in the competition.

![PCBs](./.images/robo_pictures/PCBs-overhead.jpg "Electrical System's PCB Layout")

[[previous (*Mechanical*)](./MECHANICAL.md) | [main](./REPORT.md#design) |
[next (*Software*)](./SOFTWARE.md)]
