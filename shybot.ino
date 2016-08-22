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

static bool running = false;

Servo eye_servo;

void setup() {
    pinMode(powerPins[0], INPUT);
    pinMode(powerPins[1], OUTPUT);
    pinMode(powerPins[2], OUTPUT);
    pinMode(eyePins[0], OUTPUT);
    pinMode(eyePins[1], INPUT);

    digitalWrite(powerPins[1], LOW);
    digitalWrite(powerPins[2], HIGH);

    eye_servo.attach(eyePins[2], 700, 2200);
    eye_servo.write(90);

    Serial.begin(9600);
}

void loop() {
    moveAndCheck(15, 2000);

    for (int p = 15; p < 165; p += 15) {
        moveAndCheck(p, 100);
    }

    safeDelay(4000);
    moveAndCheck(90, 500);
    moveAndCheck(120, 500);
    moveAndCheck(60, 5000);
    moveAndCheck(130, 1000);
    moveAndCheck(80, 5000);
    moveAndCheck(120, 2000);

    for (int p = 120; p > 10; p -= 10) {
        moveAndCheck(p, 250);
    }

    safeDelay(4000);
    moveAndCheck(60, 500);
    moveAndCheck(80, 500);
    moveAndCheck(60, 5000);
    moveAndCheck(130, 1000);
    moveAndCheck(80, 5000);

    for (int p = 15; p < 165; p += 15) {
        moveAndCheck(p, 300);
    }

    for (int p = 150; p > 30; p -= 15) {
        moveAndCheck(p, 300);
    }
}

void powerCheck() {
    if(digitalRead(powerPins[0]) == 1) {
        if(running == true) {
            Serial.println("Turning off...");
            running = false;
            digitalWrite(powerPins[1], LOW);
            digitalWrite(powerPins[2], HIGH);
        } else {
            Serial.println("Turning on...");
            running = true;
            digitalWrite(powerPins[1], HIGH);
            digitalWrite(powerPins[2], LOW);
        }
        delay(500);
        eye_servo.write(90);
    }
}

void safeDelay(int delay_) {
    for(int i = 0; i <= delay_; i++) {
        powerCheck();
        if(running == true) {
            delay(1);
        } else {
            break;
        }
    }
}

void moveAndCheck(int position, int delay_) {
    powerCheck();

    if(running == false) {
        return;
    }

    eye_servo.write(position);
    safeDelay(100);

    Serial.print("Distance: ");
    Serial.println(sonar(eyePins[0],eyePins[1]));

    safeDelay(delay_);
}
