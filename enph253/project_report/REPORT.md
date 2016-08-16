# <a name="top"></a> ENPH 253 - Group 14 (MURPH) - Project Report
*Jacob Brunette*, *Dilyn Fullerton*, *Ryan Watt*, *Dickson Yao*  
*Summer 2016*

* [About the project](#about)
* [Our team](#team)
* [Design](#design)
* [Manufacturing and debugging](#manufacturing)
* [Competition](#competition)
* [Conclusion](#conclusion)
* [Links](#links)

**TODO**: Picture of MURPH here

### <a name="about"></a> About the Project
---
For the final exam of our ENPH 253 class, we had to design and build a
completely autonomous robot that would compete in an “Uber-bot”
competition against the robots created by our classmates. The premise
of the “Uber-bot” competition was to create miniaturized
versions of the self-driving cars that are currently under development
by companies such as Uber and Google. A miniature city was created,
with foam buildings and electrical tape roads, and stuffed animals
scattered around the city as passengers. The robot vehicles were
required to be completely autonomous; that is, there could be no
remote control of the robots by the team members. The robots had to
detect both the IR beacons and the tape they were driving on, and all
decisions had to be made using nothing more than the robots’
internal software and the data received through its sensors.

The rules of the competition were fairly simple. Robots had two
minutes per heat to drive around the city, identify and pick up
passengers, and transport them to a set dropoff location. The
passengers were marked by small ground-level IR beacons, emitting at
a frequency of 1 KHz. The dropoff zone was marked by a beacon about 10
inches off of the ground, emitting infrared light at a frequency of 10
KHz. Two teams’ robots would be on the competition surface at once,
and were required to detect collisions with other robots, and back up
and turn around when one happened. 

[Official competition overview and rules](http://projectlab.engphys.ubc.ca/enph-253-2016/competition-2016/)

[[top](#top)]

### <a name="team"></a> Our team
---
**TODO**: Brief information about our team

[[top](#top)]

### <a name="design"></a> Design
---
Our [Formal Design Proposal](./Formal\ Design\ Document.pdf) 
describes in detail the design as we originally envisioned it.

The following are highlights of our final design, including its
mechanical, electrical, and software features.

* [_**Mechanical**_][mech]: Information about the
chassis and drive system

* [_**Electrical**_][elec]: Information about the
circuit and sensor design

* [_**Software**_][soft]: Information about the
software system.

[[top](#top)]

### <a name="manufacturing"></a> Manufacturing and debugging
---
The mechanical components of the robot were designed in Solidworks, and manufactured using a laser cutter or OMAX waterjet cutter for the most part. The metal components of the chassis were cut with the waterjet cutter, and then bent into shape using a bender. For some metal parts, further alterations were made using hand tools, such as snips or a nibbler. Most of the smaller parts (anything made out of wood or acrylic) were cut using the laser cutter.

Debugging the software took a modular approach, similar to the one we used for designing the chassis. Separate modes (tape following, grabbing and dropping, etc.) were tested seperately, before being integrated into the larger program for further debugging.

The circuits in the electrical systems were made by manually soldering components onto PCBs. We strived to make neat and compact circuits, and used adjustable potentiometers for better adaptability.

[[top](#top)]

### <a name="competition"></a> Competition
---
Our robot placed in 4th place overall (in a field of 15). Murph's best
runs came during the second heat of the group stage and during the
quarterfinal. A malfunction in the first match (caused due to the
swinging of the arm) caused Murph to drive in circles after picking up
a passenger, but we were able to change the software to accommodate
this in between heats. Unfortunately, a chip on the TINAH blew during
the semifinal, causing Murph's collision sensors to fail, and while we
knew what the problem was, there wasn't enough time between the
semifinal and the bronze-medal match to replace the chip, so the
problem persisted. Due to the sportsmanship rules of the competition,
we had to reset after every collision in the last two matches.

[Group stage: heat 1](https://www.youtube.com/watch?v=PrXCJz5u2yc#t=43m55s)

[Group stage: heat 2](https://www.youtube.com/watch?v=PrXCJz5u2yc#t=60m45s)

[Quarterfinal](https://www.youtube.com/watch?v=PrXCJz5u2yc#t=94m20s)

[Semifinal](https://www.youtube.com/watch?v=PrXCJz5u2yc#t=102m)

[Bronze-medal match](https://www.youtube.com/watch?v=PrXCJz5u2yc#t=105m50s)

[[top](#top)]

### <a name="conclusion"></a> Conclusion
---

In this project, we designed and built a fully autonomous robot out of
little more than scrap material and a TINAH board. In the process, we
experienced the full prototyping process, including designing for a
specific task, manufacturing parts, constructing strategic software,
testing, debugging, and meeting a final deadline with a functional
product.

**TODO**: Lessons learned

[[top](#top)]

### <a name="links"></a> Links
---

*Competition overview and rules:*  
http://projectlab.engphys.ubc.ca/enph-253-2016/competition-2016/

*Full competition video:*  
https://www.youtube.com/watch?v=PrXCJz5u2yc

*UBC Engineering Physics program homepage:*  
http://www.engphys.ubc.ca/

*Arduino language:*  
https://www.arduino.cc/en/Reference/HomePage

*TINAH board:*  
http://projectlab.engphys.ubc.ca/enph-253-2016/tinah-2016/

*Standard C++ library for Arduino:*  
https://github.com/maniacbug/StandardCplusplus

*Arduino Makefile setup:*  
https://github.com/sudar/Arduino-Makefile

*Atmega128 bootloader setup:*  
http://arduinoexplained.blogspot.ca/2012/03/how-arduino-download-programs-to-board.html

*Information about Arduino board memory:*  
https://learn.adafruit.com/memories-of-an-arduino/

[[top](#top)]

[mech]: ./MECHANICAL.md
[elec]: ./ELECTRICAL.md
[soft]: ./SOFTWARE.md
