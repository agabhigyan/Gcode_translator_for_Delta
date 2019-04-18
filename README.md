# Gcode_translator_for_Delta
Code that translates a G-code targeted for a cartesian CNC machine to a G-code that can be run in a Delta CNC machine with a firmware for cartesian machine installed.

ore translating the G-Codes the file format needs to be changed from ".g" or ".gcode" or ".gco" to ".txt". In the same directory where the main.cpp file is stored, there must be at least two more files - "settings.txt" and "targets.txt". Save the G-Code files in the same directory
The file "settings.txt" contains the following parameters of the Delta robot


arm_length_in_mm ___\
height _____\
radius_of_platform  _____\
radius_of_base_plate ____\
offset  ____\


Measure above quantities from your machine and update the file.
The file "targets.txt" lists the G-Code files needed to be translated


Hardware:
Upload the cartesian firmware Sprinter/Marlin to your CNC machine. Connect the motor controlling the movement of the carriages arbitrarily to the X Y and Z motor point of the controlling board. Connect the corresponding limit switches to the minimum points as well.


Calibration:
 Install interface software like pronterface/Repitier-Host in your PC. connect your CNC machine to your PC through any of the interface software. Increment the x coordinate (as denoted in the interfacing software) by 10 unit. Then the carriage being connected with the X-motor must move downward by 10 mm. If it moves in the opposite direction, invert ht econnection.If it has not moved by 10mm, let's say it has moved by x1. Now go to "congiguration.h" file in the firmware. There must be a line like-\
#define _AXIS_STEP_PER_UNIT {x2, y2, z2,e2} \
Now replace x2 by (x1/x2)*10. \
Repeat the same steps for Y-motor and Z-motor as well.
If the machine is used for 3D printing perform the same steps for extruder as well.
