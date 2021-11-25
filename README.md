# Information about this fork #

Thank you to Herbert Roider for this simple but powerful v-plotter control, and for the excellent instructions on installation and use.

I will use this fork to add a hardware PWM-control for a BLDC-motor. This drone-motor and propeller is used for pushing the plotter head against the paper.

!!!This is a work in progress. Do not use the code from this repository! I am a beginner programmer, so this might take some time, but this readme will be updated when, and if, I am able to add the functionality to Herberts code!!!

This version:
Got command line argument for bldc_dutycycle to work, but are not using the value for anything yet.
Previous version:
In Machine.cpp - Moving Z_SERVO from wiringPi pin 1 to pin 2, freeing pin 1 for the bldc motor.
wiringPi only supports hardware pwm on pin 1.
No other changes in original code.

# About #

Vplotter is a vertical plotter for g-code. It runs on a [Raspberry Pi](https://www.raspberrypi.org/). 
Works with the [gcodetools](https://github.com/cnc-club/gcodetools) extension for [Inkscape](https://inkscape.org).

v-plotter in action on youtube:

[![raspberry pi v-plotter in action](http://img.youtube.com/vi/LhT2oX4Yt_c/0.jpg)](http://www.youtube.com/watch?v=LhT2oX4Yt_c "raspberry pi v-plotter in action")


# Install #

- install raspian: [www.raspberrypi.org/downloads/](https://www.raspberrypi.org/downloads/)

- install necessary packages: git, cmake, flex, [wiringPi](http://wiringpi.com)

        sudo apt-get install git-core cmake flex wiringpi

- download and build vplotter:

        git clone https://github.com/bertlr/vplotter.git
        cd vplotter
        cmake .
        make
        sudo make install        

# Usage #

The vplotter reads g-code files.

![Geometry](vplotter_geometry.png)

The plotter needs some commandline arguments:

- x0      :    horizontal distance from the left stepper motor to the zero point of the canvas in mm. see the image 
- y0      :    vertical distance from the left stepper motor to the zero point of the canvas in mm. see the image
- baselength : the distance between the stepper motors in mm, see the image.
- z_up    :    a value for the servo motor to lift the pen (1 - 100)
- z_down  :    a value for the servo motro to move down the pen (1 - 100)
- steps   :    the count of steps to move the cord at 1 mm:
 
    1600 / circumference of the pulley
    
    for microsteps


run the plotter as root:

    sudo vplotter --x0=170 --y0=-720 --baselength=685 --z_up=11 --z_down=7 --steps=40.1

provide a g-code file test.ngc:

    sudo vplotter --x0=170 --y0=-720 --baselength=685 --z_up=11 --z_down=7 --steps=40.1 < test.ngc


finish the plotter type <kbd>Ctrl</kbd>+<kbd>C</kbd> or type in the konsole:
    
    M30

The Z-axis can only lift or drop the pen. A value >0 lift the pen, and <=0 drop the pen.

    G0 Z0 (move the pen down)
    G0 Z1 (raise the pen up)

It is possible to set a feed, but this is not exact.
    
    G1 X100 F200

It supports G2 and G3 for circles with radius R:
    
    G2 X10 Y20 R20

or with relative center point coordinates:

    G2 X10 Y0 I5 J10

Change units to inches:

    G20

Change to millimetre:

    G21

# Wiring #

The plotter uses EasyDriver for the stepper motors:
[www.schmalzhaus.com/EasyDriver/](http://www.schmalzhaus.com/EasyDriver/)

![wiring](vplotter_wiring.png "wiring")

