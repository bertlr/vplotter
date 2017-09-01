cmake_minimum_required (VERSION 2.6)
project(vplotter)

find_library(WIRINGPI_LIBRARIES NAMES wiringPi)
find_package(FLEX)

FLEX_TARGET(Gcode gcode.l lex.xx.cc)
set(SRC_FILES  Geometry.cpp Machine.cpp  main.cpp ${FLEX_Gcode_OUTPUTS})

add_definitions( -std=c++0x )

add_executable(vplotter ${SRC_FILES})
target_link_libraries(vplotter ${WIRINGPI_LIBRARIES}) 
install(TARGETS vplotter
        RUNTIME DESTINATION bin)

