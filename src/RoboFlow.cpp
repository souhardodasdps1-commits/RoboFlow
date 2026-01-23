#include "RoboFlow.h"

// Constructor
Robot::Robot() {
  _hasUltrasonic = false;
  _hasBluetooth = false;
  _hasMotors = false;
  _ruleCount = 0;
  _lastBT = '\0';
  _mode = HYBRID;
}

// === Setup ===
void Robot::addUltrasonic(int trig, int echo) {
  _trigPin = trig;
  _echoPin = echo;
  pinMode(_trigPin, OUTPUT);
  pinMode(_echoPin, INPUT);
  _hasUltrasonic = true;
}

void Robot::addBluetooth(Stream &bt) {
  _bt = &bt;
  _hasBluetooth = true;
}

void Robot::addMotors(int m1a, int m1b, int m2a, int m2b) {
  _m1a = m1a; _m1b = m1b;
  _m2a = m2a; _m2b = m2b;
  pinMode(_m1a, OUTPUT); pinMode(_m1b, OUTPUT);
  pinMode(_m2a, OUTPUT); pinMode(_m2b, OUTPUT);
  _hasMotors = true;
}

void Robot::setMode(Mode m) { _mode = m; }
Mode Robot::getMode() { return _mode; }

// === Rules ===
void Robot::when(const String &condition, Mode mode, std::function<void()> action) {
  if (_ruleCount >= MAX_RULES) return;
  _rules[_ruleCount] = { condition, mode, action };
  _ruleCount++;
}

// === Main loop ===
void Robot::run() {
  bool manualOverride = false;

  if (_hasBluetooth && _bt->available()) {
    _lastBT = _bt->read();
    if (_mode == HYBRID || _mode == MANUAL) manualOverride = true;
  }

  for (int i = 0; i < _ruleCount; i++) {
    if (_mode == MANUAL && _rules[i].mode != MANUAL) continue;
    if (_mode == AUTONOMOUS && _rules[i].mode != AUTONOMOUS) continue;
    if (_mode == HYBRID && manualOverride && _rules[i].mode == AUTONOMOUS) continue;

    if (evaluateCondition(_rules[i].condition)) {
      _rules[i].action();
    }
  }
}

// === Motors ===
void Robot::forward(int speed) {
  if (!_hasMotors) return;
  analogWrite(_m1a, speed); digitalWrite(_m1b, LOW);
  analogWrite(_m2a, speed); digitalWrite(_m2b, LOW);
}

void Robot::stop() {
  if (!_hasMotors) return;
  digitalWrite(_m1a, LOW); digitalWrite(_m1b, LOW);
  digitalWrite(_m2a, LOW); digitalWrite(_m2b, LOW);
}

void Robot::turnLeft(int speed) {
  if (!_hasMotors) return;
  digitalWrite(_m1a, LOW); analogWrite(_m1b, speed);
  analogWrite(_m2a, speed); digitalWrite(_m2b, LOW);
}

void Robot::turnRight(int speed) {
  if (!_hasMotors) return;
  analogWrite(_m1a, speed); digitalWrite(_m1b, LOW);
  digitalWrite(_m2a, LOW); analogWrite(_m2b, speed);
}

// === Sensors ===
long Robot::obstacle() {
  if (!_hasUltrasonic) return -1;
  digitalWrite(_trigPin, LOW); delayMicroseconds(2);
  digitalWrite(_trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(_trigPin, LOW);
  long duration = pulseIn(_echoPin, HIGH, 30000);
  return duration > 0 ? duration * 0.034 / 2 : 999;
}

char Robot::bluetooth() {
  return _lastBT;
}

// === Condition evaluation ===
bool Robot::evaluateCondition(const String &cond) {
  String c = cond;
  c.trim();

  if (c.startsWith("obstacle")) {
    if (!_hasUltrasonic) return false;
    long d = obstacle();

    if (c.indexOf('<') >= 0) {
      int v = c.substring(c.indexOf('<') + 1).toInt();
      return d < v;
    }
    if (c.indexOf('>') >= 0) {
      int v = c.substring(c.indexOf('>') + 1).toInt();
      return d > v;
    }
  }

  if (c.startsWith("bluetooth")) {
    if (!_hasBluetooth) return false;
    int sp = c.lastIndexOf(' ');
    if (sp < 0) return false;
    return _lastBT == c.charAt(sp + 1);
  }

  return false;
}
