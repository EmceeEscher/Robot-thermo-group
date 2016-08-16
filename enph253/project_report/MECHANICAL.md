# ENPH 253 - Group 14 (MURPH) - Project Report
*Jacob Brunette*, *Dilyn Fullerton*, *Ryan Watt*, *Dickson Yao*  
*Summer 2016*

[[main](./REPORT.md#design) | [next (*Electrical*)](./ELECTRICAL.md)]

### Mechanical Design
---
Our design for the chassis was fairly simplistic. The chassis was divided into three parts: the bottom held the motors, wheels, and other elements of the drive system; the middle provided a mount for all the circuit boards necessary for our robot, and, when attached to the bottom, enclosed the PCBs and drive system; and the top provided a place to mount the arm and TINAH.

![The robot][full_robot]

The design was intended to be modular, so that each component could be
worked on separately during the prototyping process. This worked well
at the beginning, when we had one person each working on drive system,
circuitry, arm design, and software, and each could work
independently. Near the end of the problem, however, it caused
significant issues. Once all three parts were put together, it became
a hassle to take the robot apart again every time we needed to debug
an issue. Something as simple as a wire that had become unplugged on
the inside required us to separate all three parts (removing all the
fasteners and the wires that crossed over between sections), and flip
the robot over to access the circuit boards, adding a significant time
delay to our debugging process. We wanted to redesign the chassis to
avoid these problems, but because it was our first time prototyping
something of this complexity, it was a slow process, and we didn't
realize the full extent of the flaws inherent in our design until we
had run out of time to do a full redesign.

#### Arm Design

![The arm][the_arm]

The arm was designed with simplicity in mind. It is composed of a 
simple and intuitive double jointed system where the arm may be 
controlled at its base and centre joints dynamically. This was done 
using a Servo motor at the centre joint for simple interfacing and 
a DC motor geared down significantly at the base along with a potentiometer 
for determining angle. The base had to be controlled using a simple PID 
implementation which ran nearly constantly to ensure the arm was never 
over-corrected in such a way which would damage the robot.

![The claw][the_claw]

The claw at the end of the arm had a single mounted DC motor for gripping 
as well as several internal switches. The claw would close or open for specified 
periods of time unless specific switches were triggered. The claw should also 
be noted for having adjustable angles on the claw itself, allowing for easy tuning 
right up until the competition The switches were for sensing whether:

  * The arm collided with an animal while reaching and should not reach further
  * The claw had caught an animal and should begin to lift it
  * The claw had closed on itself and should note that no animal was caught
  
Issues faced with the arm design included the difficulties of using PID 
control, in particular the integral partition, which would occasionally 
cause the arm to jolt suddenly and destroy its gears in early stages, 
halting development. Future systems will be developed with better fail-safe 
apparatus. Additionally, as we saw with other teams arms, the second degree 
of freedom of our arm was completely inessential and so we learned to properly 
assess what a technological solution needs to be able to do without overcomplicating 
things.

#### Drive System Design

![Chassis Underside][chassis_underside]

The drive system was intended to give our robot maximum maneuverability. 
It is composed of centrally placed wheels rotating with separate axles to provide 
a differential speed steering system, in which different speeds of the wheels 
cause turning motion. The robot was then made balanced by placing plastic 
semi-spheres under the front-centre and back-centre of the robot which were smooth 
enough to slide along the competition surface. These semi-spheres were actually given 
suspension so as to promote stability in the robot while preventing either of the primary 
driving wheels from ever being raised off the surface. The driving wheels were 
powered by their internally mounted gears which interfaced with DC motors protruding 
from the sides of the chassis and geared the motors up for increases in speed.

Problems arose in the design process and implementation of our drive system frequently 
throughout all stages of development. This was done caused by instability in the axles 
which supported the wheels, causing the gears between the DC motors and drive wheels to 
frequently grind and function poorly, if at all. This problem was caused by several factors.
One key factor was our limited ability to create precision bushings to hold the axle in 
place while the wheel spun, due partially to inexperience in machining precision pieces 
as well the flexure of our sheet metal chassis misaligning the bushings intended to hold 
the axle straight. Furthermore, the problem was worsened by the difficulty in holding the 
motors themselves parallel to the bushings and perfectly still. An effective solution to 
this was not realized until the second iteration of our chassis was produced and we had 
had significant time to experiment. In future designs we will find better and more secure 
ways to affix the key drive system components in the initial design to circumvent these
problems entirely.

[[main](./REPORT.md#design) | [next (*Electrical*)](./ELECTRICAL.md)]

[full_robot]: ./.images/robo_pictures/full_robo-side.jpg
[the_arm]: ./.images/robo_pictures/arm_fixed.jpg
[the_claw]: ./.images/robo_pictures/claw.jpg
[chassis_underside]: ./.images/robo_pictures/chassis-bottom.jpg
