# Lara Maia <dev@lara.click> 2016

cmake_minimum_required(VERSION 3.0.2)
project(ShyBot)

########## Configuration ##########

set(BOARD uno)
set(PORT /dev/ttyACM0)
set(SKETCH shybot.ino)

# Use arduino-builder >= 3.19
set(BUILDER /home/lara/Develop/arduino-builder/arduino-builder)
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)

set(AVRDUDE /usr/bin/avrdude)
set(AVRDUDE_CONFIG /etc/avrdude.conf)

set(CORE /usr/share/arduino-core)
set(HARDWARE ${CORE}/hardware)
set(TOOLS ${HARDWARE}/tools/avr)
set(LIBRARIES ${CORE}/libraries)

####################################

if(${BOARD} MATCHES "uno")
    set(FQBN arduino:avr:uno)
    set(BAUD 115200)
    set(MCU atmega328p)
elseif(${BOARD} MATCHES "nano")
    set(FQBN arduino:avr:nano:cpu=atmega328)
    set(BAUD 57600)
    set(MCU atmega328p)
endif()

add_custom_target(
        build ALL

            COMMAND ${BUILDER} -hardware ${HARDWARE}
                               -tools ${TOOLS}
                               -libraries ${LIBRARIES}
                               -fqbn ${FQBN}
                               -verbose -warnings all
                               -build-path ${BUILD_DIR}
                               -compile ${SKETCH}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_custom_target(
        upload
        # -p <mcu> | -c <protocol> | -P <serial port> | -b <speed> | -U <mem type>:w:<sketch.hex>:i | -C <config file>
        COMMAND ${AVRDUDE} -F -v -D
                           -p ${MCU}
                           -c arduino
                           -P ${PORT}
                           -b ${BAUD}
                           -U flash:w:${BUILD_DIR}/${SKETCH}.hex:i
                           -C ${AVRDUDE_CONFIG}
        DEPENDS build
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_custom_target(
        distclean
        COMMAND rm -rfv ${BUILD_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
