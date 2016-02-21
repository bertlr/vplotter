# Install #

- install raspian from https://www.raspberrypi.org/downloads/
- install git:


        sudo apt-get install git-core

- install wiringPi from http://wiringpi.com/download-and-install/


        git clone https://github.com/bertlr/vplotter.git
        cd vplotter
        make -f Makefile CONF=Release

# Wiring #

![wiring](vplotter_wiring.png "wiring")

# Usage #

run the plotter as root:

    sudo dist/Release/GNU-Linux/vplotter  --x0=170 --y0=-720 --baselength=685 --z_up=13 --z_down=8 --steps=40.1

or provide a g-code file:

    sudo dist/Release/GNU-Linux/vplotter  --x0=170 --y0=-720 --baselength=685 --z_up=13 --z_down=8 --steps=40.1 < test.ngc


finish the plotter type <kbd>Ctrl</kbd>+<kbd>C</kbd> or type in the konsole:
    
    M30

vplotter reads g-code files
