#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize I2C LCD (Address 0x27, 16x2)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin Definitions for the 4 LEDs
const int LED_DATA_HAZARD       = 8;   // Red LED (Data Hazard)
const int LED_CONTROL_HAZARD    = 9;   // Yellow LED (Control Hazard)
const int LED_STRUCTURAL_HAZARD = 10;  // Blue LED (Structural Hazard)
const int LED_HAZARD_CLEAR      = 11;  // Green LED (Normal / Cleared)

// Maximum instructions in a batch
const int MAX_INSTR = 4;
String instructionBuffer[MAX_INSTR];
int totalInstructions = 0;

void setup() {
  Serial.begin(115200);
  
  pinMode(LED_DATA_HAZARD, OUTPUT);
  pinMode(LED_CONTROL_HAZARD, OUTPUT);
  pinMode(LED_STRUCTURAL_HAZARD, OUTPUT);
  pinMode(LED_HAZARD_CLEAR, OUTPUT);

  lcd.init();
  lcd.backlight();
  
  setLEDs(false, false, false, false); // All LEDs OFF initially
}

void loop() {
  // STEP 1: Ask for number of instructions
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pipeline visualizer");
  lcd.setCursor(0, 1);
  lcd.print("Enter Count:");

  while (Serial.available() == 0);
  totalInstructions = Serial.parseInt();
  while (Serial.available() > 0) Serial.read(); // Flush buffer

  if (totalInstructions < 1) totalInstructions = 1;
  if (totalInstructions > MAX_INSTR) totalInstructions = MAX_INSTR;

  // STEP 2: Collect instructions sequentially
  for (int i = 0; i < totalInstructions; i++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter Inst ");
    lcd.print(i + 1);
    lcd.setCursor(0, 1);
    lcd.print("In Serial Mon");

    while (Serial.available() == 0);
    String inst = Serial.readStringUntil('\n');
    inst.trim();
    inst.toUpperCase();
    instructionBuffer[i] = inst;
    delay(400);
  }

  // STEP 3: Run Concurrent Pipelined Execution & Hazard Simulation
  runPipelinedBatch();

  // STEP 4: Thank You & Reset Screen
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PROCESS COMPLETE");
  lcd.setCursor(0, 1);
  lcd.print("THANK YOU! :)");
  setLEDs(false, false, false, false); // Turn off all LEDs when complete
  delay(4000);
}

void runPipelinedBatch() {
  int stages[MAX_INSTR] = {0};
  int totalCycles = totalInstructions + 3;

  String destRegs[MAX_INSTR];
  String sourceRegs[MAX_INSTR];

  for (int i = 0; i < totalInstructions; i++) {
    destRegs[i] = extractDestReg(instructionBuffer[i]);
    sourceRegs[i] = extractSourceRegs(instructionBuffer[i]);
  }

  for (int cycle = 1; cycle <= totalCycles; cycle++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Clock Cycle: ");
    lcd.print(cycle);

    for (int i = 0; i < totalInstructions; i++) {
      if (stages[i] > 0 && stages[i] < 4) {
        stages[i]++;
      } else if (stages[i] == 0 && cycle >= (i + 1)) {
        stages[i] = 1;
      }
    }

    String stageSummary = "";
    for (int i = 0; i < totalInstructions; i++) {
      if (stages[i] == 1) stageSummary += "I" + String(i+1) + ":IF ";
      else if (stages[i] == 2) stageSummary += "I" + String(i+1) + ":ID ";
      else if (stages[i] == 3) stageSummary += "I" + String(i+1) + ":EX ";
      else if (stages[i] == 4) stageSummary += "I" + String(i+1) + ":WB ";
    }
    lcd.setCursor(0, 1);
    lcd.print(stageSummary);
    setLEDs(false, false, false, false); // Keep LEDs off during normal pipeline flow
    delay(1500);

    // Hazard Detection & Remediation
    for (int i = 1; i < totalInstructions; i++) {
      if (stages[i] == 2) { 
        
        // 1. Structural Hazard Check
        if (instructionBuffer[i].indexOf("LOAD") != -1 || instructionBuffer[i].indexOf("STORE") != -1 || instructionBuffer[i].indexOf("MEM") != -1) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Structural Hazard");
          lcd.setCursor(0, 1);
          lcd.print("Remedy: Bubble");
          setLEDs(false, false, true, false); // Blue LED ON
          delay(2000);
          
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Status: CLEARED");
          setLEDs(false, false, false, true); // Green LED ON only after clearing
          delay(1200);
          setLEDs(false, false, false, false); // Turn green LED back off after showing resolved state
        }
        // 2. Control Hazard Check
        else if (instructionBuffer[i].indexOf("BEQ") != -1 || instructionBuffer[i].indexOf("JMP") != -1 || instructionBuffer[i].indexOf("BRANCH") != -1) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Control Hazard");
          lcd.setCursor(0, 1);
          lcd.print("Remedy: Flush");
          setLEDs(false, true, false, false); // Yellow LED ON
          delay(2000);
          
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Status: CLEARED");
          setLEDs(false, false, false, true); // Green LED ON only after clearing
          delay(1200);
          setLEDs(false, false, false, false); // Turn green LED back off
        }
        // 3. Data Hazard Check
        else if (destRegs[i-1] != "" && sourceRegs[i].indexOf(destRegs[i-1]) != -1) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Data Hazard Det.");
          lcd.setCursor(0, 1);
          lcd.print("Remedy: Forward");
          setLEDs(true, false, false, false); // Red LED ON
          delay(2000);
          
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Status: CLEARED");
          setLEDs(false, false, false, true); // Green LED ON only after clearing
          delay(1200);
          setLEDs(false, false, false, false); // Turn green LED back off
        }
      }
    }
  }

  // Display Final Results
  for (int i = 0; i < totalInstructions; i++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("I"); lcd.print(i + 1); lcd.print(": ");
    lcd.print(instructionBuffer[i]);

    long ans = computeResult(instructionBuffer[i]);
    lcd.setCursor(0, 1);
    lcd.print("Final Ans: ");
    lcd.print(ans);
    setLEDs(false, false, false, false);
    delay(2500);
  }
}

String extractDestReg(String inst) {
  int firstSpace = inst.indexOf(' ');
  int firstComma = inst.indexOf(',');
  if (firstSpace != -1 && firstComma != -1 && firstComma > firstSpace) {
    return inst.substring(firstSpace + 1, firstComma);
  }
  return "";
}

String extractSourceRegs(String inst) {
  int firstComma = inst.indexOf(',');
  if (firstComma != -1) {
    return inst.substring(firstComma + 1);
  }
  return "";
}

long computeResult(String inst) {
  int firstDigitIdx = -1;
  for (int i = 0; i < inst.length(); i++) {
    if (isDigit(inst[i]) || (inst[i] == '-' && i + 1 < inst.length() && isDigit(inst[i + 1]))) {
      firstDigitIdx = i;
      break;
    }
  }

  if (firstDigitIdx == -1) return 0;

  String sub = inst.substring(firstDigitIdx);
  long num1 = sub.toInt();
  
  int separator = sub.indexOf(',');
  if (separator == -1) separator = sub.indexOf(' ');

  long num2 = 0;
  if (separator != -1) {
    num2 = sub.substring(separator + 1).toInt();
  }

  if (inst.indexOf("ADD") != -1) return num1 + num2;
  if (inst.indexOf("SUB") != -1) return num1 - num2;
  if (inst.indexOf("MUL") != -1) return num1 * num2;
  if (inst.indexOf("DIV") != -1 && num2 != 0) return num1 / num2;

  return num1;
}

void setLEDs(bool data, bool control, bool structural, bool cleared) {
  digitalWrite(LED_DATA_HAZARD, data ? HIGH : LOW);
  digitalWrite(LED_CONTROL_HAZARD, control ? HIGH : LOW);
  digitalWrite(LED_STRUCTURAL_HAZARD, structural ? HIGH : LOW);
  digitalWrite(LED_HAZARD_CLEAR, cleared ? HIGH : LOW);
}