// --- Configuration ---
// Button Matrix Pins: 5 rows x 6 columns
const int buttonRowPins[5] = {2, 3, 4, 5, 6};     // Rows driven as outputs
const int buttonColPins[6] = {7, 8, 9, 10, 11, 12}; // Columns as inputs with pullup

// LED Matrix Pins: 5 rows x 6 columns
// (Assuming a common-cathode LED matrix: 
//  to light an LED, set its row HIGH and its column LOW.)
const int ledRowPins[5] = {22, 23, 24, 25, 26};    // LED rows driven as outputs
const int ledColPins[6] = {27, 28, 29, 30, 31, 32}; // LED columns driven as outputs

// Variables to store the last button pressed (matrix indices)
// (-1 means “no button has been pressed yet”)
int lastButtonRow = -1;
int lastButtonCol = -1;

// --- Setup ---
void setup() {
  Serial.begin(115200);
  
  // --- Set up button matrix pins ---
  // For scanning, set row pins as outputs (start HIGH) and column pins as inputs with pullups.
  for (int r = 0; r < 5; r++) {
    pinMode(buttonRowPins[r], OUTPUT);
    digitalWrite(buttonRowPins[r], HIGH);  // Default state: HIGH (not driving)
  }
  for (int c = 0; c < 6; c++) {
    pinMode(buttonColPins[c], INPUT_PULLUP);
  }
  
  // --- Set up LED matrix pins ---
  // Configure LED row and column pins as outputs.
  // For our common-cathode arrangement, default:
  //   LED rows LOW (off) and LED columns HIGH.
  for (int r = 0; r < 5; r++) {
    pinMode(ledRowPins[r], OUTPUT);
    digitalWrite(ledRowPins[r], LOW);
  }
  for (int c = 0; c < 6; c++) {
    pinMode(ledColPins[c], OUTPUT);
    digitalWrite(ledColPins[c], HIGH);
  }
}

// --- Main Loop ---
void loop() {
  // --- 1. Button Matrix Scan ---
  int pressedRow = -1;
  int pressedCol = -1;
  
  // Scan each row: drive it LOW then check each column.
  for (int r = 0; r < 5; r++) {
    // Drive current row LOW for scanning.
    digitalWrite(buttonRowPins[r], LOW);
    delayMicroseconds(5);  // small delay for signal stabilization

    // Check all 6 columns: if a button is pressed, the column will read LOW.
    for (int c = 0; c < 6; c++) {
      if (digitalRead(buttonColPins[c]) == LOW) {
        pressedRow = r;
        pressedCol = c;
      }
    }
    // Return the row pin to HIGH before moving on.
    digitalWrite(buttonRowPins[r], HIGH);
  }
  
  // If a button was detected, update our “last pressed” record.
  if (pressedRow != -1 && pressedCol != -1) {
    lastButtonRow = pressedRow;
    lastButtonCol = pressedCol;
    
    Serial.print("Button pressed at row: ");
    Serial.print(pressedRow);
    Serial.print("  col: ");
    Serial.println(pressedCol);
    
    // Simple debounce: wait until the pressed button is released.
    while (digitalRead(buttonColPins[pressedCol]) == LOW) {
      delay(10);
    }
  }
  
  // --- 2. LED Matrix Multiplexing ---
  // We rapidly cycle through the LED rows so that the single “active” LED appears steadily lit.
  for (int row = 0; row < 5; row++) {
    // Turn off all LED rows to avoid ghosting.
    for (int r = 0; r < 5; r++) {
      digitalWrite(ledRowPins[r], LOW);
    }
    
    // For all LED columns, set them to OFF (HIGH in a common-cathode system).
    for (int c = 0; c < 6; c++) {
      digitalWrite(ledColPins[c], HIGH);
    }
    
    // If this is the row corresponding to the last pressed button...
    if (row == lastButtonRow) {
      // Turn ON the LED at the corresponding column.
      // Set the row HIGH (to source current) and the column LOW (to sink current).
      digitalWrite(ledRowPins[row], HIGH);
      digitalWrite(ledColPins[lastButtonCol], LOW);
    }
    
    // Short delay so the row is visible; the loop cycles fast enough for persistence of vision.
    delay(2);
  }
}

