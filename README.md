# Pocketqube Command and Data Handling Board

Pocketqube command and data handling board

**Dependencies**

```bash
sudo apt install build-essential cmake gcc git libusb-1.0-0 libusb-1.0-0-dev libgtk-3-dev
cd $HOME/git-repos/cote/scripts
./setup_dependencies.sh $HOME/sw
```

# Useful link

* [libopencm3](http://libopencm3.org/docs/latest/stm32l4/html/modules.html) : Open source firmware link STM

## Directory Contents

* [3d-models](3d-models/README.md): Contains copies of `.stp` 3D model files
* [components](components/README.md): Contains copies of `.lib` component files
* [datasheets](datasheets/README.md): Contains component datasheets
* [footprints](footprints/README.md): Contains copies of `.kicad_mod` footprint
  files
* [gerbers](gerbers/README.md): Contains the most recent set of Gerber files
* [kicad](kicad/README.md): Contains the KiCad project files
* [versions](versions/README.md): Contains `.zip` files of Gerber files
  submitted for manufacture
* [BOM.csv](BOM.csv): Bill of materials
* [LICENSE](LICENSE): License
* [NOTES.md](NOTES.md): Step-by-step instructions for setting up KiCad,
  designing the board, and exporting Gerber files for manufacture
* [README.md](README.md): This document

## License

Written by Bradley Denby  
Other contributors: None

## Update

PQ-CDH Version 0.1.0 : 

1. Transferred design scheamtic and PCB from Kicad5 to Kicad6
2. Mounting holes adjustment same as referred to comm-401 board.
3. Reduced board dimension 43 x 43 mm
4. Removing 5th Pin from upper and lower right

PQ-CDH Version 0.1.1

1. Changed VDD, GND, SWDIO, and SWCLK to lower left to right 4 pins
2. Changed "PQ CDH v0.1.0" to "PQ CDH v0.1.1"
3. Adjusted controller and Conn_02x06_Odd_Even, for mounting hole clearance.
4. Adjusted Conn_02x06_Odd_Even in vertically aligned along with aligned to top and bottom connector holes.
5. Addition of 4 testpads added between oscillator and its capacitor.

PQ-CDH Version 0.2.0

1. I2C3 SDA and SCL to pins 1 and 2 on the bottom header row
2. ADC to pin 3 on the bottom header row
3. unused GPIO to pins 4 and 5 on bottom header row


See the top-level [LICENSE](LICENSE) file for the license.
