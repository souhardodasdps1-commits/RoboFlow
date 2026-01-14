# RoboFlow
Declarative robotics framework for Arduino

**RoboFlow** is a rule-based robotics framework for Arduino. It allows developers to create **manual, autonomous, and hybrid robot behaviors** using simple human-readable rules.

## Features

- Control motors: `forward`, `stop`, `turnLeft`, `turnRight`
- Ultrasonic obstacle detection support
- Bluetooth control support
- Hybrid mode: combines manual and autonomous behavior
- Simple rule-based syntax for easy programming

## Installation

1. Download the Arduino-ready ZIP from this repository
2. Open Arduino IDE → Sketch → Include Library → Add .ZIP Library
3. The library will appear in **Sketch → Include Library → RoboFlow**

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
```

## `Usage`

```cpp
// 1. Define your robot’s motors, sensors, and mode in setup()
bot.addUltrasonic(trigPin, echoPin);
bot.addBluetooth(Serial);
bot.addMotors(m1a, m1b, m2a, m2b);
bot.setMode(HYBRID);

// 2. Define rules using when(condition, mode, action)
bot.when("obstacle < 20", AUTONOMOUS, [](){
    bot.stop();
    bot.turnLeft();
});

// 3. Call bot.run() inside loop()
void loop() {
    bot.run();
}
```

## License

This project is licensed under the **MIT License** — see the `LICENSE` file.
