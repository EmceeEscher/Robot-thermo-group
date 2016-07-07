# Robot-thermo-group
Code for controlling competition robot.

### Dependencies
---
* [TINAH library](http://projectlab.engphys.ubc.ca/wp-content/uploads/TINAH_DocumentsFolder.zip)
* Improved [TINAH phys253 library](./lib/phys253TINAHArduino)
* Arduino 1.6.2 or newer

### Setup
---
The following should be done to prepare for usage:

1. Install the latest Arduino IDE from [www.arduino.cc](https://www.arduino.cc/en/Main/Software)

2. Download the [TINAH library](http://projectlab.engphys.ubc.ca/wp-content/uploads/TINAH_DocumentsFolder.zip)

3. Move `hardware/` and `libraries/` to `~/Documents/Arduino/`.

4. Clone this repository.  

   ```bash
   git clone 'https://github.com/EmceeEscher/Robot-thermo-group.git'
   ```

5. Replace the existing phys253 library at
`~/Documents/Arduino/libraries/phys253TINAHArduino`
with the improved one
[`./lib/phys253TINAHArduino`](./lib/phys253TINAHArduino).

### Examples
---
See [`./examples`](./examples).

### See also
---
* Arduino language: (https://www.arduino.cc/en/Reference/HomePage)
* TINAH board: (http://projectlab.engphys.ubc.ca/enph-253-2016/tinah-2016/)
