// UPLOAD ONLY ONCE TO PICO THEN FORGET!!!

// Install Libraries for LCD
#include <Wire.h> // Allows Communication
#include <LiquidCrystal_I2C.h> // Control Screen

// Pins (don't modify)
const int TRIG = 17; // Send signal
const int ECHO = 16; // Recieves signal

// Creates new lcd
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// Chosen Settings 
int MIN_CM = 2;
int MAX_CM = 100;
int TOO_CLOSE_CM = 40;

void setup() {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // Used for I²C communication.
  Wire.setSDA(4); // GP 4
  Wire.setSCL(5); // GP 5
  Wire.begin();

  // LCD Initialization
  lcd.init(); 
  lcd.backlight();
}

// Function to get a distance reading
float getDistance() {

  // Sends a 10 microsec pulse 
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2); 

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  
  // Measures how long echo stays HIGH 
  float time = pulseIn(ECHO, HIGH);

  // Error Handling (signal not returned)
  if (time == 0) { 
    return -1; // Not possible
  }

  // d = vt, v = 0.0343 cm/us
  float distance = time * 0.0343 / 2;
  return distance;
}

// Function to get average distance
int getAverageDistance() {

  float total = 0;
  int count = 0;

  for (int i = 0; i < 3; i++) {
    float d = getDistance();

    // Only accept valid values (filtering noise, etc)
    if (d >= MIN_CM && d <= MAX_CM) {
      total = total + d;
      count++;
    }

    delay(30);
  }

  // If all readings were bad
  if (count == 0) {
    return -1;
  }

  // Returns average dis (as an int)
  return (int)(total / count);
}

void loop() {

  int distance = getAverageDistance();

  lcd.clear();

  if (distance == -1) {
    lcd.setCursor(0, 0);
    lcd.print("Invalid range");
  }
  else if (distance <= TOO_CLOSE_CM) {
    lcd.setCursor(0, 0);
    lcd.print("   Too Close!");
    lcd.setCursor(0, 1);
    lcd.print("Sit Up Straight!");
  }
  else {
    lcd.setCursor(0, 0);
    lcd.print("Your Distance:");
    lcd.setCursor(0, 1);
    lcd.print(distance);
    lcd.print(" cm");
  }

  delay(300);
}