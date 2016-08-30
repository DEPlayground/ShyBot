# Lara Maia <dev@lara.click> 2016

cmake_minimum_required(VERSION 3.0.2)
project(ShyBot)

set(BOARD arduino:avr:uno)
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

add_custom_target(
        build ALL

            COMMAND ${BUILDER} -hardware ${HARDWARE}
                               -tools ${TOOLS}
                               -libraries ${LIBRARIES}
                               -fqbn ${BOARD}
                               -verbose -warnings all
                               -build-path ${BUILD_DIR}
                               -compile ${SKETCH}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_custom_target(
        upload_uno
        # -p <mcu> | -c <protocol> | -P <serial port> | -b <speed> | -U <mem type>:w:<sketch.hex>:i | -C <config file>
        COMMAND ${AVRDUDE} -F -v -D
                           -p atmega328p
                           -c arduino
                           -P ${PORT}
                           -b 115200
                           -U flash:w:${BUILD_DIR}/${SKETCH}.hex:i
                           -C ${AVRDUDE_CONFIG}
        DEPENDS build
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_custom_target(
        upload_nano
        # -p <mcu> | -c <protocol> | -P <serial port> | -b <speed> | -U <mem type>:w:<sketch.hex>:i | -C <config file>
        COMMAND ${AVRDUDE} -F -v -D
                           -p atmega328p
                           -c arduino
                           -P ${PORT}
                           -b 57600
                           -U flash:w:${BUILD_DIR}/${SKETCH}.hex:i
                           -C ${AVRDUDE_CONFIG}
        DEPENDS build
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_custom_target(
        distclean
        COMMAND rm -rfv ${BUILD_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
