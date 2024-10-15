
     #include <Keypad.h>

// Define keypad size
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns

// Define symbols on the buttons of the keypad
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Connect the row and column pinouts of the keypad
byte rowPins[ROWS] = {9, 8, 7, 6};   // Row pins
byte colPins[COLS] = {5, 4, 3, 2};   // Column pins

// Create an instance of the Keypad class
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Pin assignments for LEDs and buzzer
const int redLED = 13;
const int yellowLED = 11;
const int greenLED = 12;
const int buzzer = 10;

// Variables for timing and state management
unsigned long lastMillis = 0;
unsigned long flashLastMillis = 0; // Used for flashing interval
int redDuration = 24;   // Default red light duration (seconds)
int greenDuration = 20; // Default green light duration (seconds)
int yellowDuration = 3; // Fixed yellow light duration (seconds)
int currentDuration = 0;
char currentState = 'R'; // 'R' for red, 'G' for green, 'Y' for yellow
bool systemRunning = false; // To start the sequence after pressing '*'
bool flashing = false; // To manage the flashing and buzzer behavior

// Function to flash lights and sound buzzer for 0.5 seconds
void flashLightAndBuzzer(int ledPin) {
  unsigned long currentMillis = millis();
  if ((currentMillis - flashLastMillis) >= 500) {
    digitalWrite(ledPin, !digitalRead(ledPin)); // Toggle the light
    digitalWrite(buzzer, !digitalRead(buzzer)); // Toggle the buzzer
    flashLastMillis = currentMillis;
  }
}

// Setup function
void setup() {
  // Set pins as outputs for LEDs and buzzer
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  // Initialize serial communication
  Serial.begin(9600);
}

// Main loop function
void loop() {
  char key = customKeypad.getKey(); // Read key from keypad
  
  // Handle keypad input
  if (key) {
    Serial.println(key);
    if (key == '*') {
      systemRunning = true; // Start the traffic light sequence
      currentDuration = 0; // Reset the timer
      lastMillis = millis(); // Reset timer to manage delay
    } else if (key == 'A') {
      // Example: A-2-4-# sets the red light to 24 seconds
      redDuration = getNumberFromKeypad();
    } else if (key == 'B') {
      // Example: B-2-0-# sets the green light to 20 seconds
      greenDuration = getNumberFromKeypad();
    }
  }

  // Traffic light control system
  if (systemRunning) {
    if (millis() - lastMillis >= 1000) { // Increment currentDuration every 1 second
      currentDuration++;
      lastMillis = millis();
    }

    if (currentState == 'R') {
      if (currentDuration == 0) {
        digitalWrite(buzzer, HIGH);  // Turn on buzzer
        delay(3000);                 // Buzzer on for 3 seconds
        digitalWrite(buzzer, LOW);   // Turn off buzzer
        digitalWrite(redLED, HIGH);  // Turn on Red light
      }
      if (currentDuration >= redDuration - 3 && currentDuration < redDuration) {
        // Start flashing Red light and sound buzzer during the last 3 seconds
        flashLightAndBuzzer(redLED);
      } else if (currentDuration >= redDuration) {
        digitalWrite(redLED, LOW);   // Turn off Red light
        digitalWrite(buzzer, LOW);   // Ensure buzzer is off
        changeState('G');            // Change to green light
      }
    } else if (currentState == 'G') {
            if (currentDuration == 0) {
        digitalWrite(buzzer, HIGH);  // Turn on buzzer
        delay(3000);                 // Buzzer on for 3 seconds
        digitalWrite(buzzer, LOW);   // Turn off buzzer
        digitalWrite(greenLED, HIGH);  // Turn on Green light
      }
      if (currentDuration >= greenDuration - 3 && currentDuration < greenDuration) {
        // Start flashing Green light and sound buzzer during the last 3 seconds
        flashLightAndBuzzer(greenLED);
      } else if (currentDuration >= greenDuration) {
        digitalWrite(greenLED, LOW);   // Turn off Green light
        digitalWrite(buzzer, LOW);     // Ensure buzzer is off
        changeState('Y');              // Change to yellow light
      }
    } else if (currentState == 'Y') {
      if (currentDuration == 0) {
        digitalWrite(buzzer, HIGH);  // Turn on buzzer
        delay(3000);                 // Buzzer on for 3 seconds
        digitalWrite(buzzer, LOW);   // Turn off buzzer
        digitalWrite(yellowLED, HIGH); // Turn on Yellow light
      }
      if (currentDuration >= yellowDuration) {
        digitalWrite(yellowLED, LOW);  // Turn off Yellow light
        digitalWrite(buzzer, LOW);     // Ensure buzzer is off
        changeState('R');              // Change back to red light
      }
    }
  }
}

// Helper function to get number input from the keypad (e.g., 24 for 24 seconds)
int getNumberFromKeypad() {
  int number = 0;
  char key;
  while ((key = customKeypad.getKey()) != '#') {
    if (key) {
      number = number * 10 + (key - '0');
      Serial.print(key);  // Display the key on the serial monitor
    }
  }
  Serial.println();
  return number;
}

// Change the state of the traffic light system
void changeState(char nextState) {
  // Ensure all LEDs are turned off before switching state
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(yellowLED, LOW);
  digitalWrite(buzzer, LOW);
  
  // Reset duration and flashing state
  currentDuration = 0;
  flashing = false;
  
  // Update the current state to the next one
  currentState = nextState;
  
  // Reset the timer for flashing
  lastMillis = millis();
  flashLastMillis = millis();
}
