# Pipeline Visualizer with Automatic Data Hazard Detection and Resolution

A simulator that shows how instructions move through a CPU pipeline stage by stage, and automatically detects and resolves data hazards.

## What It Does

- Simulates a [5-stage] CPU pipeline (IF, ID, EX, MEM, WB).
- Automatically detects data hazards such as **RAW (Read After Write)** [, WAR, WAW].
- Resolves hazards using **forwarding** and/or **stalling** (pipeline bubbles).
- Displays a cycle-by-cycle pipeline diagram so you can see where each instruction is at every clock cycle.

## Circuit Diagram

![Pipeline Circuit Diagram](CIRC.PNG)

## Technologies Used

- Language: [Python / Java / JavaScript / C++]
- Tools: [Logisim / Draw.io / VS Code]

## Project Structure

```
pipeline-visualizer/
├── src/          Source code
├── circuit/      Circuit diagram
├── report/       Project report (PDF)
├── screenshots/  Output screenshots
└── README.md
```

## How to Run

1. Download or clone this repository.
2. Open a terminal in the `src` folder.
3. Run the program:

**Python:**
```
python main.py
```

**Java:**
```
javac Main.java
java Main
```

**JavaScript / HTML:**
Open `index.html` in your browser.

## Sample Input

```
ADD R1, R2, R3
SUB R4, R1, R5
AND R6, R1, R7
```

## Sample Output

![Output Screenshot](OP1.PNG)

(The second instruction depends on R1 from the first, so a RAW hazard is detected and resolved using forwarding or a stall.)

## Report

The full project report is available here: [Project Report](report/Project_Report.pdf)

## Author

- NAME: T Padma Loshini
- REG.NO:1250110121195
- CLASS: II-B.Tech CSE -"B" sec
