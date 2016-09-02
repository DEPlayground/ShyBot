/*
 *  Lara Maia <dev@lara.click> 2016
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Servo.h>
#include "ultrasonic.h"

#define ULTRASONIC_TRIGGER A1
#define ULTRASONIC_ECHO A0
#define ULTRASONIC_SERVO 9

#define POWER_BUTTON 7
#define POWER_ON_LED A2
#define POWER_OFF_LED A3

#define MOTOR1 12
#define MOTOR1_SPEED 3

#define MOTOR2 13
#define MOTOR2_SPEED 11

static const byte eyePins[3] = {
    ULTRASONIC_TRIGGER,
    ULTRASONIC_ECHO,
    ULTRASONIC_SERVO,
};

static const byte powerPins[3] = {
    POWER_BUTTON,
    POWER_ON_LED,
    POWER_OFF_LED,
};

static const byte motorsPins[4] = {
    MOTOR1,
    MOTOR2,
    MOTOR1_SPEED,
    MOTOR2_SPEED,
};

// FIXME: Use just 1 pin for front_light, please
static const byte front_light1 = 5;
static const byte front_light2 = 6;
static float front_light_voltage = map(2.8, 0, 3.3, 0, 255);
static int eye_position = 0;
static int crash_distance;
static bool reverse = false;
static unsigned char speed = 0;

Servo eye_servo;

void move(byte direction) {
    switch(direction) {
        case 'F':
            Serial.print("Moving to forward ");
            digitalWrite(motorsPins[0], LOW);
            digitalWrite(motorsPins[1], HIGH);
            break;
        case 'B':
            Serial.print("Moving to back ");
            digitalWrite(motorsPins[0], HIGH);
            digitalWrite(motorsPins[1], LOW);
            break;
        case 'L':
            Serial.print("Moving to Left ");
            digitalWrite(motorsPins[0], HIGH);
            digitalWrite(motorsPins[1], HIGH);
            break;
        case 'R':
            Serial.print("Moving to Right ");
            digitalWrite(motorsPins[0], LOW);
            digitalWrite(motorsPins[1], LOW);
            break;
    }

    if(direction == 'S') {
        Serial.println("Stoping");
        analogWrite(motorsPins[2], 0);
        analogWrite(motorsPins[3], 0);
    } else {
        Serial.print("(speed: ");
        Serial.print(speed);
        Serial.println(')');

        analogWrite(motorsPins[2], speed);
        analogWrite(motorsPins[3], speed);
    }
}

void changeDirection() {
    Serial.print("Eye position: ");
    Serial.println(eye_position);
    Serial.print("Crash Distance: ");
    Serial.println(crash_distance);

    if(crash_distance > 0 && crash_distance <= 20) {
        if(eye_position < 90) {
            move('L');
        } else if(eye_position >= 90) {
            move('R');
        }

        if(eye_position <= 40 || eye_position >= 130) {
            safeDelay(500);
        } else {
            safeDelay(1000);
        }

        move('F');
    }
}

bool checkSerialCommand(byte command) {
    if(Serial.peek() == command) {
        // Flush serial buffer
        Serial.read();
        return true;
    } else {
        return false;
    }
}

void autoModeOn() {
    Serial.println("Turning autoMode on...");
    digitalWrite(powerPins[1], HIGH);
    digitalWrite(powerPins[2], LOW);
    speed = 255;
    delay(500);
}

void autoModeOff() {
    Serial.println("Turning autoMode off...");
    analogWrite(motorsPins[2], 0);
    analogWrite(motorsPins[3], 0);
    digitalWrite(powerPins[1], LOW);
    digitalWrite(powerPins[2], HIGH);
    delay(500);
}

bool autoModeSwitch() {
    if(digitalRead(powerPins[0]) == HIGH) {
        if (digitalRead(powerPins[1]) == HIGH) {
            Serial.println("autoMode disabled by button");
            autoModeOff();
        } else {
            Serial.println("autoMode enabled by button");
            autoModeOn();
        }
    }

    while(Serial.available()) {
        if (checkSerialCommand('x')) {
            Serial.println("autoMode disabled by serial");
            autoModeOff();
            continue;
        }

        if(checkSerialCommand('X')) {
            Serial.println("autoMode enabled by serial");
            autoModeOn();
            continue;
        }

        break;
    }

    if(digitalRead(powerPins[1]) == HIGH) {
        return true;
    } else {
        eye_servo.write(90);
        return false;
    }
}

void safeDelay(int delay_) {
    for(int i = 0; i <= delay_; i++) {
        if(autoModeSwitch()) {
            delay(1);
        } else {
            break;
        }
    }
}

void setup() {
    pinMode(powerPins[0], INPUT);
    pinMode(powerPins[1], OUTPUT);
    pinMode(powerPins[2], OUTPUT);
    pinMode(eyePins[0], OUTPUT);
    pinMode(eyePins[1], INPUT);

    for(int i = 0; i <= 3; i++) {
        pinMode(motorsPins[i], OUTPUT);
    }

    digitalWrite(powerPins[1], LOW);
    digitalWrite(powerPins[2], HIGH);

    eye_servo.attach(eyePins[2], 700, 2200);
    eye_servo.write(90);

    Serial.begin(9600);
}

void loop() {
    while(Serial.available()) {
        if(checkSerialCommand('w')) {
            Serial.println("Front light Off");
            analogWrite(front_light1, 0);
            analogWrite(front_light2, 0);
            continue;
        }

        if(checkSerialCommand('W')) {
            Serial.println("Front light On");
            // FIXME: Replace by a resistor, please
            analogWrite(front_light1, front_light_voltage);
            analogWrite(front_light2, front_light_voltage);
            continue;
        }

        break;
    }

    if(autoModeSwitch()) {
        move('F');
    } else {
        while(Serial.available()) {
            if (checkSerialCommand('F')) {
                move('F');
                continue;
            }

            if (checkSerialCommand('B')) {
                move('B');
                continue;
            }

            if (checkSerialCommand('L')) {
                move('L');
                continue;
            }

            if (checkSerialCommand('R')) {
                move('R');
                continue;
            }

            if(checkSerialCommand('S')) {
                move('S');
                continue;
            }

            for(unsigned char i=0; i <= 9; i++) {
                if (checkSerialCommand(i+'0')) {
                    speed = map(i, 0, 10, 0, 255);
                }
            }

            if(checkSerialCommand('q')) {
                speed = 255;
            }

            // Flush serial buffer for the last check
            // this prevents infinite loop if receives
            // an incorrect input on serial
            Serial.read();
        }

        delay(100);
        return;
    }

    crash_distance = sonar(eyePins);

    if(crash_distance == 0 || crash_distance > 200) {
        eye_servo.write(eye_position);
        safeDelay(500);

        if(reverse) {
            eye_position -= 90;
        } else {
            eye_position += 90;
        }

        if(eye_position >= 180) {
            reverse = true;
        } else if(eye_position <= 0) {
            reverse = false;
        }
    } else if(crash_distance > 0 && crash_distance <= 200) {
        eye_servo.write(eye_position);
        safeDelay(100);

        if(reverse) {
            eye_position -= 30;
        } else {
            eye_position += 30;
        }

        if(eye_position >= 180) {
            reverse = true;
        } else if(eye_position <= 0) {
            reverse = false;
        }

        changeDirection();
    }
}
