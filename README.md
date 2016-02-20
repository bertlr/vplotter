# Install #

- install raspian
- install git:


    sudo apt-get install git-core

- install wiringPi from http://wiringpi.com/download-and-install/


    git clone https://github.com/bertlr/vplotter.git
    cd vplotter
    make -f Makefile CONF=Release

run the plotter as root:

    sudo dist/Release/GNU-Linux/vplotter

finish the plotter type Strg + C
    

# Usage #

vplotter reads g-code files
