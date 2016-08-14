# ENPH 253 - Group 14 (MURPH) - Project Report
*Jacob Brunette*, *Dilyn Fullerton*, *Ryan Watt*, *Dickson Yao*  
*Summer 2016*

[[previous (*Electrical*)](./ELECTRICAL.md) | [top](./REPORT.md#design)]

### Software Design
---

![Software][head]

The following details our software design, including our original
vision, the problems encountered with it, and the final resolution.

#### Original design: Modularity and abstraction

![Software major modes][modes]

Our original software design strategy was to separate major objectives
into separate modes, implementing a ```MajorMode``` abstract class.
At any single point in time the robot's state would consist of a
single active major mode, whose ```loop()``` function would be called
each iteration.

Minor functions such as following tape, operating the arm, or tracking
the destination beacon would be handled by one or more
simultaneously operating modes, implementing a ```MinorMode``` abstract
class. The active major mode would mitigate communication between
concurrently active minor modes and control their activation and
deactivation. 

All major and minor modes would be controlled and owned by a single
```RobotState``` object, which would serve to control the flow through
the various major modes.

#### Problems: Dynamic allocations and stack-heap collisions

![Arduino stack-heap collisions][stack]

**TODO**

#### Final design: Simplicity wins
**TODO**

[head]: ./.images/software.png
[modes]: ./.images/modes.jpg
[stack]: ./.images/arduinostack.gif
