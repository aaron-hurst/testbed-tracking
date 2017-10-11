# Computer vision for a Traffic and Autonomous Vehicle Testbed

Code developed as part of my final year research project for my Master of Professional Engineering degree at The University of Western Australia. 

Supervisors: Dr Tim French & Dr Ghulam Mubashar Hassan

## Overview

This software is designed to track multiple ZenWheels cars within the bounds of the testbed area. Two algorithms have been developed for this, either of which (but not both simultaneously) can be run using this code. Both algorithms are __detection__ algorithms. That is, both algorithms track the cars by detecting them in individual frames -- information from previous frames is not considered.

The first algorithm -- hue-based detection -- can be applied to uniformly coloured cars and detects these by matching image pixels to the car's known colour. The resulting mask is then examined for regions of the expected size. Each car must have a different colour for this to work. Currently it is calibrated for six cars: orange, red, blue, pink, yellow and green.

The second algorithm -- histogram-based detection -- can be applied to either uniformly coloured cars or, and this is the reason it was designed, cars with colour markers attached to them. In particular, some cars have been fitted with two different coloured markers. This causes them to have a unique colour __histogram__ (in HSI colour space). Cars are detected by calculating a colour (hue) histogram over their area and comparing this to a library of prototype histograms. That with the strongest match as associated with the car - a positive detection.

Currently this approach is not as reliable as hue-based detection. However, further optimisation of the prototype histograms may be able to address this.

## Hardware requirements:

* Raspberry Pi
* Raspberry Pi Camera V2

## Software requirements

* OpenCV -- tested using version 3.3.0 (http://opencv.org/releases.html)
* RaspiCam -- used for interfacing with the Raspberry Pi camera, tested using version 0.1.6 (http://www.uco.es/investiga/grupos/ava/node/40)
* mmal (installed with Raspian by default)

## Using the software

### Compiling

A makefile is provided for compiling the system. Simply store all source files in the same directory and call `make` to compile the program.

You may need to change the include directories stored in the `CFLAGS` variable in the makefile, depending on your installation.

A future development is to migrate compilation over to CMake. This will (hopefully) check the presence and version of the above software requirements.

### Configuration

Most parameters that are likely to be modified during use are stored in a config file `config.txt`. This file is structures into three parts.

1. Detection/tracking mode: select 0 for hue-based detection, 1 for histogram-based detection.
2. Global parameters: this section includes parameters common to both detection methods. Of particular interest to the user are:

   min_sat -- minimum pixel saturation value to be used in detection
   min_val -- minimum pixel intensity value to be used in detection
   origin_x -- x pixel location of coordinate system origin
   origin_y -- y pixel location of coordinate system origin
   scale -- a factor for converting from image to real-world position (pixel to mm)

   The last three must be determined when setting up the system. Take a sample image, determine where the origin position is, and measure the scale factor based on known real-world distances (e.g. place a ruler or tape measure on the testbed).

3. Car parameters: an arbitrary number of cars can be added to the config file. Each car has four parameters: name, MAC address, hue and delta. 'Hue' and 'delta' specify the range of pixel hue values to be considered candidates for being part of the car. The range is `hue + delta` to `hue - delta`. Each car's entry in the config file must begin with a line that says `Car   [Y/N]` and end with a line that contains only `END`. `[Y/N]` should be replaced with Y or N based on whether the user wishes to include the specified car in the program (Y) or not (N).

### Run

The compilation process builds an executable called `tracker`. This should be run as follows:

`./tracker [frames] [mode] [delay]`

* `frames` specifies the number of frames for which the program is to be run.
* `mode` specifies the output mode to use from the following options:

   | Mode   | Type   | Description                                                               |
   | :----: |:------:| :-------------------------------------------------------------------------|
   | 0      | LIVE   | JSON string sent to server                                                |
   | 1      | LIVE   | JSON string sent to server, output printed to console                     |
   | 2      | LIVE   | JSON string sent to server, output printed to console & saved to log.csv  |
   | 3      | TEST   | Output printed to console & saved to log.csv                              |
   | 4      | DEBUG  | Output printed to console, saved to log.csv and all debug outputs enabled |

* `delay` specifies a time in milliseconds for which the program sleeps between processing one video frame and getting the next.