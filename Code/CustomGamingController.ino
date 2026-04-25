#include "Adafruit_TinyUSB.h"

// Declare the USB device (PICO) to act as a keyboard 
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD()
};

// Create USB HID object (keyboard)
Adafruit_USBD_HID usb_hid(
  desc_hid_report,
  sizeof(desc_hid_report),
  HID_ITF_PROTOCOL_KEYBOARD,
  2,
  false
);

// Pins

// Joystick analog pins
const int xPin = 26;       // GP26 = horizontal movement
const int yPin = 27;       // GP27 = vertical movement
const int swPin = 6;       // GP6 = joystick press (F)

// Button pins
const int greenButtonPin = 2;    // Space
const int yellowButtonPin = 1;   // C
const int redButtonPin = 15;     // R
const int blackButtonPin = 0;   // G

// Calibration values

int centerX = 0;   // joystick resting X value
int centerY = 0;   // joystick resting Y value

// Variables (State)

// Movement states
bool upPressed = false;
bool downPressed = false;
bool leftPressed = false;
bool rightPressed = false;

// Button states
bool greenPressed = false;
bool yellowPressed = false;
bool redPressed = false;
bool blackPressed = false;
bool swPressed = false;

// Helper Functions

// Clears all pressed keys (important to prevent stuck keys)
void clearKeys() {
  uint8_t keycode[6] = {0};
  usb_hid.keyboardReport(0, 0, keycode);
}

// Calibrates joystick by averaging values when it is not being touched
void calibrateJoystick() {
  long sumX = 0;
  long sumY = 0;
  const int samples = 60;

  for (int i = 0; i < samples; i++) {
    sumX += analogRead(xPin);
    sumY += analogRead(yPin);
    delay(5);
  }
  
  centerX = sumX / samples;
  centerY = sumY / samples;
}

// Sends keyboard inputs to the computer
void sendKeyboardReport() {
  uint8_t keycode[6] = {0}; // max 6 keys at once
  int idx = 0;

  // Movement keys (WASD)
  if (upPressed)    keycode[idx++] = HID_KEY_W;
  if (leftPressed)  keycode[idx++] = HID_KEY_A;
  if (downPressed)  keycode[idx++] = HID_KEY_S;
  if (rightPressed) keycode[idx++] = HID_KEY_D;

// Button mappings
if (greenPressed)  keycode[idx++] = HID_KEY_SPACE;
if (yellowPressed) keycode[idx++] = HID_KEY_C;
if (blackPressed)   keycode[idx++] = HID_KEY_G;  
if (redPressed)    keycode[idx++] = HID_KEY_R;
if (swPressed)     keycode[idx++] = HID_KEY_F;

  // Send report to PC
  usb_hid.keyboardReport(0, 0, keycode);
} 

// Setup

void setup() {
  analogReadResolution(12);  // higher precision for joystick

  // Set button pins as input with pull-up resistors
  pinMode(swPin, INPUT_PULLUP);
  pinMode(greenButtonPin, INPUT_PULLUP);
  pinMode(yellowButtonPin, INPUT_PULLUP);
  pinMode(redButtonPin, INPUT_PULLUP);
  pinMode(blackButtonPin, INPUT_PULLUP);

  usb_hid.begin();   // start USB keyboard
  delay(2000);       // wait for USB connection

  calibrateJoystick(); // find center position
  clearKeys();         // ensure no keys are stuck
}

// Main Loop

void loop() {
  // Wait until USB is ready
  if (!usb_hid.ready()) {
    delay(10);
    return;
  }

  // Read joystick values
  int x = analogRead(xPin);
  int y = analogRead(yPin);

  // Calculate how far joystick moved from center
  int dx = x - centerX;
  int dy = y - centerY;

  int deadzone = 1000; // ignore small movements

  // Reset movement states
  upPressed = false;
  downPressed = false;
  leftPressed = false;
  rightPressed = false;

  // Joystick mapping (rotated 90° counter-clockwise)
  if (dx < -deadzone) upPressed = true;      // left -> W
  if (dx > deadzone)  downPressed = true;    // right -> S
  if (dy < -deadzone) rightPressed = true;   // up -> D
  if (dy > deadzone)  leftPressed = true;    // down -> A

  // Read button states (LOW = pressed)
  greenPressed  = (digitalRead(greenButtonPin) == LOW);
  yellowPressed = (digitalRead(yellowButtonPin) == LOW);
  redPressed    = (digitalRead(redButtonPin) == LOW);
  blackPressed  = (digitalRead(blackButtonPin) == LOW);
  swPressed     = (digitalRead(swPin) == LOW);

  // Send all inputs to PC
  sendKeyboardReport();

  delay(20); // small delay for stability
}