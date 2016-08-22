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
#define ULTRASONIC_SERVO 5

#define POWER_BUTTON 7
#define POWER_ON_LED 8
#define POWER_OFF_LED 9

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

Servo eye_servo;
static int eye_position = 0;
static int crash_distance;
static bool reverse = false;

void safeDelay(int delay_) {
    for(int i = 0; i <= delay_; i++) {
        if(isRunning()) {
            delay(1);
        } else {
            break;
        }
    }
}

void move(byte direction, unsigned char speed=255) {
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
        case 'S':
            Serial.println("Stoping");
            speed = 0;
            break;
    }

    if(speed != 0) {
        Serial.print("(speed: ");
        Serial.print(speed);
        Serial.println(')');
    }

    analogWrite(motorsPins[2], speed);
    analogWrite(motorsPins[3], speed);
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
            safeDelay(1000);
        } else {
            safeDelay(2000);
        }

        move('F');
    }
}

bool isRunning() {
    if(digitalRead(powerPins[0]) == HIGH) {
        if(digitalRead(powerPins[1]) == HIGH) {
            Serial.println("Turning off...");
            digitalWrite(powerPins[1], LOW);
            digitalWrite(powerPins[2], HIGH);
        } else {
            Serial.println("Turning on...");
            digitalWrite(powerPins[1], HIGH);
            digitalWrite(powerPins[2], LOW);
        }
        delay(500);
    }

    if(digitalRead(powerPins[1]) == HIGH) {
        return true;
    } else {
        eye_servo.write(90);
        move('N', 0);
        return false;
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
    if(isRunning()) {
        move('F');
    } else {
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
