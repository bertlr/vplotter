# Install #

- install raspian from https://www.raspberrypi.org/downloads/
- install git:


        sudo apt-get install git-core

- install wiringPi from http://wiringpi.com/download-and-install/


        git clone https://github.com/bertlr/vplotter.git
        cd vplotter
        make -f Makefile CONF=Release

# Usage #

run the plotter as root:

    sudo dist/Release/GNU-Linux/vplotter --x0=100 --y0=-650 --baselength=900 --z_up=13 --z_down=8 --steps=40,4


finish the plotter type <kbd>Ctrl</kbd>+<kbd>C</kbd>

vplotter reads g-code files
