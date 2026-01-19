# RoboFlow

Declarative robotics framework for Arduino.

RoboFlow is a rule-based robotics framework for Arduino. It allows developers to create manual, autonomous, and hybrid robot behaviors using simple, human-readable rules.

## Features
- Motor control: forward, stop, turnLeft, turnRight
- Ultrasonic obstacle detection support
- Bluetooth control support
- Hybrid mode combining manual and autonomous behavior
- Simple rule-based syntax

## Installation
1. Open Arduino IDE
2. Go to Sketch → Include Library → Manage Libraries
3. Search for "RoboFlow"
4. Click Install

## Example

```cpp
#include <RoboFlow.h>

Robot bot;

void setup() {
  Serial.begin(9600);

  bot.addUltrasonic(8, 9);
  bot.addBluetooth(Serial);
  bot.addMotors(5, 6, 10, 11);

  bot.setMode(HYBRID);

  bot.when("obstacle < 20", AUTONOMOUS, [](){
    bot.stop();
    bot.turnLeft();
  });

  bot.when("bluetooth == 'F'", MANUAL, [](){
    bot.forward(180);
  });

  bot.when("bluetooth == 'B'", MANUAL, [](){
    bot.turnRight();
  });

  bot.when("bluetooth == 'S'", MANUAL, [](){
    bot.stop();
  });
}

void loop() {
  bot.run();
}
Usage
Define motors, sensors, and operating mode in setup()

Add behavior rules using when(condition, mode, action)

Call bot.run() inside loop()

License
MIT License. See the LICENSE file for details.
