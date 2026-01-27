#include "RoboFlow.h"

Robot::Robot() {
  _hasUltrasonic=false; 
  _hasBluetooth=false; 
  _hasMotors=false;
  _ruleCount=0; 
  _lastBT='\0'; 
  _mode=HYBRID;
}

void Robot::addUltrasonic(int trig,int echo){
  _trigPin=trig; _echoPin=echo;
  pinMode(_trigPin,OUTPUT); 
  pinMode(_echoPin,INPUT);
  _hasUltrasonic=true;
}

void Robot::addBluetooth(Stream &bt){
  _bt=&bt; 
  _hasBluetooth=true;
}

void Robot::addMotors(int m1a,int m1b,int m2a,int m2b){
  _m1a=m1a; _m1b=m1b; _m2a=m2a; _m2b=m2b;
  pinMode(_m1a,OUTPUT); pinMode(_m1b,OUTPUT);
  pinMode(_m2a,OUTPUT); pinMode(_m2b,OUTPUT);
  _hasMotors=true;
}

void Robot::setMode(Mode m){ _mode=m; }
Mode Robot::getMode(){ return _mode; }

void Robot::when(const String &condition, Mode mode, std::function<void()> action){
  if(_ruleCount<MAX_RULES){
    _rules[_ruleCount].condition=condition;
    _rules[_ruleCount].mode=mode;
    _rules[_ruleCount].action=action;
    _ruleCount++;
  }
}

void Robot::run(){
  bool manualOverride=false;
  if(_hasBluetooth && _bt->available()){
    _lastBT=_bt->read();
    if(_mode==HYBRID||_mode==MANUAL) manualOverride=true;
  }

  for(int i=0;i<_ruleCount;i++){
    if(_mode==MANUAL && _rules[i].mode!=MANUAL) continue;
    if(_mode==AUTONOMOUS && _rules[i].mode!=AUTONOMOUS) continue;
    if(_mode==HYBRID && manualOverride && _rules[i].mode==AUTONOMOUS) continue;
    if(evaluateCondition(_rules[i].condition)) _rules[i].action();
  }
}

void Robot::forward(int speed){
  if(!_hasMotors) return;
  analogWrite(_m1a,speed); digitalWrite(_m1b,LOW);
  analogWrite(_m2a,speed); digitalWrite(_m2b,LOW);
}

void Robot::stop(){
  if(!_hasMotors) return;
  digitalWrite(_m1a,LOW); digitalWrite(_m1b,LOW);
  digitalWrite(_m2a,LOW); digitalWrite(_m2b,LOW);
}

void Robot::turnLeft(int speed){
  if(!_hasMotors) return;
  digitalWrite(_m1a,LOW); analogWrite(_m1b,speed);
  analogWrite(_m2a,speed); digitalWrite(_m2b,LOW);
}

void Robot::turnRight(int speed){
  if(!_hasMotors) return;
  analogWrite(_m1a,speed); digitalWrite(_m1b,LOW);
  digitalWrite(_m2a,LOW); analogWrite(_m2b,speed);
}

long Robot::obstacle(){
  if(!_hasUltrasonic) return -1;
  digitalWrite(_trigPin,LOW); delayMicroseconds(2);
  digitalWrite(_trigPin,HIGH); delayMicroseconds(10);
  digitalWrite(_trigPin,LOW);
  long duration=pulseIn(_echoPin,HIGH,30000);
  return duration*0.034/2;
}

char Robot::bluetooth(){ return _lastBT; }

bool Robot::evaluateCondition(const String &cond){
  if(cond.startsWith("obstacle")){
    if(!_hasUltrasonic) return false;
    long d=obstacle();
    int value=cond.substring(cond.lastIndexOf(' ')+1).toInt();
    if(cond.indexOf("<")>0) return d<value;
    if(cond.indexOf(">")>0) return d>value;
  }
  if(cond.startsWith("bluetooth")){
    if(!_hasBluetooth) return false;
    char target=cond.charAt(cond.length()-2);
    return _lastBT==target;
  }
  return false;
}
