# ENPH 253 - Group 14 (MURPH) - Project Report
*Jacob Brunette*, *Dilyn Fullerton*, *Ryan Watt*, *Dickson Yao*  
*Summer 2016*

[[top](./REPORT.md#design) | [next (*Electrical*)](./ELECTRICAL.md)]

### Mechanical Design
---
Our design for the chassis was fairly simplistic. The chassis was divided into three parts: the bottom held the motors, wheels, and other elements of the drive system; the middle provided a mount for all the circuit boards necessary for our robot, and, when attached to the bottom, enclosed the PCBs and drive system; and the top provided a place to mount the arm and TINAH.

![The robot][full robot]

The design was intended to be modular, so that each component could be worked on separately during the prototyping process. This worked well at the beginning, when we had one person each working on drive system, circuitry, arm design, and software, and each could work independently. Near the end of the problem, however, it caused significant issues. Once all three parts were put together, it became a hassle to take the robot apart again every time we needed to debug an issue. Something as simple as a wire that had become unplugged on the inside required us to separate all three parts (removing all the fasteners and the wires that crossed over between sections), and flip the robot over to access the circuit boards, adding a significant time delay to our debugging process. We wanted to redesign the chassis to avoid these problems, but because it was our first time prototyping something of this complexity, it was a slow process, and we didn't realize the full extent of the flaws inherent in our design until we had run out of time to do a full redesign.

[full robot]: ./.images/robo\ pictures/full\ robo\ -\ side.jpg
