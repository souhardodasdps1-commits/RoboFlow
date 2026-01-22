#ifndef ROBOFLOW_H
#define ROBOFLOW_H

#include <Arduino.h>
#include <functional>

#define MAX_RULES 12

// Modes of operation
enum Mode { MANUAL, AUTONOMOUS, HYBRID };

// Rule structure
struct Rule {
  String condition;
  Mode mode;
  std::function<void()> action;
};

class Robot {
public:
  Robot() {
    _hasUltrasonic = false;
    _hasBluetooth = false;
    _hasMotors = false;
    _ruleCount = 0;
    _lastBT = '\0';
    _mode = HYBRID;
  }

  // === Setup functions ===
  void addUltrasonic(int trig, int echo) {
    _trigPin = trig;
    _echoPin = echo;
    pinMode(_trigPin, OUTPUT);
    pinMode(_echoPin, INPUT);
    _hasUltrasonic = true;
  }

  void addBluetooth(Stream &bt) {
    _bt = &bt;
    _hasBluetooth = true;
  }

  void addMotors(int m1a, int m1b, int m2a, int m2b) {
    _m1a = m1a; _m1b = m1b;
    _m2a = m2a; _m2b = m2b;
    pinMode(_m1a, OUTPUT); pinMode(_m1b, OUTPUT);
    pinMode(_m2a, OUTPUT); pinMode(_m2b, OUTPUT);
    _hasMotors = true;
  }

  void setMode(Mode m) { _mode = m; }
  Mode getMode() { return _mode; }

  // === Rule registration ===
  void when(const String &condition, Mode mode, std::function<void()> action) {
    if (_ruleCount >= MAX_RULES) return;
    _rules[_ruleCount].condition = condition;
    _rules[_ruleCount].mode = mode;
    _rules[_ruleCount].action = action;
    _ruleCount++;
  }

  // === Main loop function ===
  void run() {
    bool manualOverride = false;

    // Read Bluetooth input if available
    if (_hasBluetooth && _bt->available()) {
      _lastBT = _bt->read();
      if (_mode == HYBRID || _mode == MANUAL) manualOverride = true;
    }

    // Evaluate all rules
    for (int i = 0; i < _ruleCount; i++) {
      // Skip rules not matching current mode
      if (_mode == MANUAL && _rules[i].mode != MANUAL) continue;
      if (_mode == AUTONOMOUS && _rules[i].mode != AUTONOMOUS) continue;
      if (_mode == HYBRID && manualOverride && _rules[i].mode == AUTONOMOUS) continue;

      if (evaluateCondition(_rules[i].condition)) {
        _rules[i].action();
      }
    }
  }

  // === Motor controls ===
  void forward(int speed = 150) {
    if (!_hasMotors) return;
    analogWrite(_m1a, speed); digitalWrite(_m1b, LOW);
    analogWrite(_m2a, speed); digitalWrite(_m2b, LOW);
  }

  void stop() {
    if (!_hasMotors) return;
    digitalWrite(_m1a, LOW); digitalWrite(_m1b, LOW);
    digitalWrite(_m2a, LOW); digitalWrite(_m2b, LOW);
  }

  void turnLeft(int speed = 150) {
    if (!_hasMotors) return;
    digitalWrite(_m1a, LOW); analogWrite(_m1b, speed);
    analogWrite(_m2a, speed); digitalWrite(_m2b, LOW);
  }

  void turnRight(int speed = 150) {
    if (!_hasMotors) return;
    analogWrite(_m1a, speed); digitalWrite(_m1b, LOW);
    digitalWrite(_m2a, LOW); analogWrite(_m2b, speed);
  }

  // === Sensors ===
  long obstacle() {
    if (!_hasUltrasonic) return -1;
    digitalWrite(_trigPin, LOW); delayMicroseconds(2);
    digitalWrite(_trigPin, HIGH); delayMicroseconds(10);
    digitalWrite(_trigPin, LOW);
    long duration = pulseIn(_echoPin, HIGH, 30000); // 30ms timeout
    return duration > 0 ? duration * 0.034 / 2 : 999; // Return 999 if no echo
  }

  char bluetooth() { return _lastBT; }

private:
  int _trigPin, _echoPin; bool _hasUltrasonic;
  Stream* _bt; bool _hasBluetooth; char _lastBT;
  int _m1a, _m1b, _m2a, _m2b; bool _hasMotors;
  Rule _rules[MAX_RULES]; int _ruleCount;
  Mode _mode;

  // === Condition evaluation ===
  bool evaluateCondition(const String &cond) {
    String c = cond;
    c.trim();

    // Obstacle conditions: "obstacle < 20" or "obstacle>15"
    if (c.startsWith("obstacle")) {
      if (!_hasUltrasonic) return false;
      long d = obstacle();
      int value = 0;

      if (c.indexOf('<') >= 0) {
        value = c.substring(c.indexOf('<') + 1).toInt();
        return d < value;
      } else if (c.indexOf('>') >= 0) {
        value = c.substring(c.indexOf('>') + 1).toInt();
        return d > value;
      } else {
        return false;
      }
    }

    // Bluetooth conditions: "bluetooth A"
    if (c.startsWith("bluetooth")) {
      if (!_hasBluetooth) return false;
      int lastSpace = c.lastIndexOf(' ');
      if (lastSpace < 0) return false;
      char target = c.charAt(lastSpace + 1);
      return _lastBT == target;
    }

    return false;
  }
};

#endif

