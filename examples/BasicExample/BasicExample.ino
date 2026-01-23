#include <RoboFlow.h>

// Create RoboFlow object
RoboFlow robot;

void setup() {
  Serial.begin(9600);

  // Initialize RoboFlow
  robot.begin();

  // Example rules
  robot.setManualRule("Move forward when button pressed");
  robot.setAutonomousRule("Avoid obstacles using ultrasonic sensor");

  Serial.println("RoboFlow example started!");
}

void loop() {
  // Run the RoboFlow rules
  robot.run();

  // Slow down loop for demonstration
  delay(500);
}
