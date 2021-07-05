/*  USB cable tester
 * Send a level high or low on one side of the cable and read from the other side.
 * Then do the same the other way.
 * Display result on the serial monitor.
 * Only testing for continuity. No performance test.
 * Connector 1
 * VBUS --> D12
 * D+   --> D11
 * D-   --> D10
 * GND  --> D9
 * 
 * Connector 2
 * VBUS --> D8
 * D+   --> D7
 * D-   --> D6
 * GND  --> D5
 * 
 * All 8 lines tired to a 2.2k Ohms resistor
 * Common of the resistors tired to D4
 */
uint8_t pinlist[8] = {12, 11, 10, 9, 8, 7, 6, 5};
char* pinName[4]={"VBUS "," D+  "," D-  ","GND  "};
uint8_t pinPull = 4;
uint8_t cableState[4][4];

// fill the table with 0
void clearTable(void){
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      cableState[i][j] = 0;
    }
  }
}

// display the connections detected during the scan
void displayTable(void){
  char* stateStr[4] = {"  _| ","  ~~ ","  ?? "};
  Serial.print("      ");
  for(int i = 0; i< 4; i++){
    Serial.print(pinName[i]);
  }
  Serial.println();
  for (int i = 0; i < 4; i++) {
    Serial.print(pinName[i]);
    for (int j = 0; j < 4; j++) {
      int state = 4;
      switch (cableState[i][j]) {
        case 3:
        case 12:
        case 15:
          state = 0; // connected
          break;
        case 0:
          state = 1;
          break;
        default:
          state = 2;
          break;
      }
      Serial.print(stateStr[state]);
    }
    Serial.println("");
  }
  Serial.println("");
}

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 8; i++) {
    pinMode(pinlist[i], INPUT); // all I/Os HiZ
  }
  pinMode(pinPull,OUTPUT);
  digitalWrite(pinPull, HIGH);
}


void loop() {
  clearTable();
  
  // drive from connector one of the cable and read from connector two
  for (int i = 0; i < 4; i++) {
    pinMode(pinlist[i], OUTPUT);
    for (int j = 0; j < 4; j++) {
      // Output HIGH, lines pulled down
      digitalWrite(pinlist[i], HIGH);
      digitalWrite(pinPull, LOW);
      delay(20);
      int level = digitalRead(pinlist[j + 4]);
      cableState[i][j] |=  (level == HIGH) ? 1 : 0;

      // Output LOW, lines pulled high
      digitalWrite(pinlist[i], LOW);
      digitalWrite(pinPull, HIGH);
      delay(20);
      level = digitalRead(pinlist[j + 4]);
      cableState[i][j] |=  (level == LOW) ? 2 : 0;
    }
    pinMode(pinlist[i], INPUT);
  }

  // drive from connector two of the cable and read from connector one
  for (int i = 0; i < 4; i++) {
    pinMode(pinlist[i+4], OUTPUT);
      // Output HIGH, lines pulled down
    for (int j = 0; j < 4; j++) {
      digitalWrite(pinlist[i+4], HIGH);
      digitalWrite(pinPull, LOW);
      delay(20);
      int level = digitalRead(pinlist[j]);
      cableState[i][j] |=  (level == HIGH) ? 4 : 0;

      // Output LOW, lines pulled high
      digitalWrite(pinlist[i+4], LOW);
      digitalWrite(pinPull, HIGH);
      delay(20);
      level = digitalRead(pinlist[j]);
      cableState[i][j] |=  (level == LOW) ? 8 : 0;
      //Serial.println(cableState[i][j]);
    }
    pinMode(pinlist[i + 4], INPUT);
  }
  
  displayTable();
  delay(2000);
}
