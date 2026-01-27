#include <RoboFlow.h>

// Main RoboFlow controller object
RoboFlow robot;

void setup() {
  // Start serial communication for debugging
  Serial.begin(9600);
  while (!Serial) { ; } // Wait for Serial on boards like Leonardo/Micro

  // Initialize RoboFlow library
  robot.begin();

  // Example rules
  robot.setManualRule("Move forward when button pressed");        // Manual mode trigger
  robot.setAutonomousRule("Avoid obstacles using ultrasonic sensor"); // Autonomous behavior

  // Inform user that the library is ready
  Serial.println("RoboFlow example started!");
  Serial.println("Manual: press button to move forward");
  Serial.println("Autonomous: robot will avoid obstacles automatically");
}

void loop() {
  // Execute RoboFlow rules continuously
  robot.run();

  // Small delay to make Serial output readable
  delay(500);
}
