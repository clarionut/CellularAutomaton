/*
    Cellular Automaton

    Program to run an Arduino-based Cellular Automaton simulation of the 
    NonlinearCircuits Cellular Automata Eurorack module. The original is
    a tour-de-force of 4xxx CMOS circuit design, but a microcontroller-
    based implementation is much simpler to design and assemble, probably
    cheaper, and also offers the possibility for additional functionality.

    This implementation has a voltage control for the probability of a
    cell being ON in each generation. This can be varied from 100% (the
    active cells are controlled entirely by the rules of the cellular
    automaton) to about 55% (the density of ON cells is generally lower
    and there are longer periods of inactivity). In addition, the seed
    cells (see below) can be activated with an adjustable random
    probability or by CV. In contrast to the original, the seed CVs
    affect the probability of the seed cells being ON rather than being
    a simple switch.

    The same rules as the original are implemented in the simulation. All
    cells except the corners use rule 150 (the cell itself and its
    orthogonal neighbours are all included in the XOR function) while the
    corner cells use rule 90 (only the orthogonal neighbours are counted).
    Each of the corner cells has a hidden off-grid seed cell neighbour
    which can be activated by an external CV.
    
    The rules are based on an extended multi-input XOR function - this is
    possible because the basic 2-input XOR function is associative. In
    practice this means that the output will be ON if the number of ON
    inputs is ODD, and OFF if the number of ON inputs is EVEN. In the
    simulation, if the cell count under the appropriate rule is ODD the
    cell will be ON in the next generation (or will have the possibility
    of being ON if the probability is < 100%).

    Control inputs for synth module:
    - four analogue inputs for seed cells, voltages set probability of
      the cell being on
    - pot for manual probability control of seed cells
    - CV input for probability of cell survival normalled to 5V
    - pot for survival CV level
    - external clock input, doubling as CV input for internal clock
    - switch for external/internal clock
    - pot for internal clock speed (CV level input, normalled to 5V)

    Outputs:
    - 16 gates, one for each cell
    - 3 CVs, one for top half of grid, one for bottom half and one for
      the whole grid (voltage levels as in the NLC original)

    C G Earnshaw 08 08 2024
*/

#define SRSTROBE 8
#define SRDATA 11
#define SRCLOCK 12
#define CLKMODE 10

uint8_t cell[6][6] = {0};
uint8_t next[6][6] = {0};
int16_t seed[4];
int16_t seedProb;
uint8_t lastClock = 0;
unsigned long genMillis = 0L;

void setup() {
  // put your setup code here, to run once:
  pinMode(SRSTROBE, OUTPUT);
  pinMode(SRCLOCK, OUTPUT);
  pinMode(SRDATA, OUTPUT);
  pinMode(CLKMODE, INPUT_PULLUP);
  randomSeed(analogRead(A6));
  //Serial.begin(115200);
}

void loop() {
  if (digitalRead(CLKMODE)) {
    // HIGH = switch open = external clock
    if (digitalRead(A5) != lastClock) {
      // Clock has changed...
       if (lastClock) {
        // ...and has gone low
        lastClock = 0;
      } else {
        // ...and has gone high, so update the CA
        updateCells();
        lastClock = 1;
      }
    }
  } else if ((millis() - genMillis) > exp((float) (1023 - analogRead(A5)) / 95.)) {
    // Internal clock and the CA needs updating
    genMillis = millis();
    updateCells();
  }
}

void setSeeds() {
  // Set some low-probability values in the off-grid seed cells
  analogRead(A0);
  seed[0] = analogRead(A0);
  analogRead(A0);
  seed[1] = analogRead(A1);
  analogRead(A2);
  seed[2] = analogRead(A2);
  analogRead(A3);
  seed[3] = analogRead(A3);
  analogRead(A7);
  seedProb = analogRead(A7);

  cell[1][0] = (random(1024) > 1023 - (seed[0] + seedProb)) ? 1 : 0;
  cell[1][5] = (random(1024) > 1023 - (seed[1] + seedProb)) ? 1 : 0;
  cell[4][0] = (random(1024) > 1023 - (seed[2] + seedProb)) ? 1 : 0;
  cell[4][5] = (random(1024) > 1023 - (seed[3] + seedProb)) ? 1 : 0;
}

void updateCells() {
  // Loop over the grid cells to work out which will be on in the next generation
  setSeeds();
  for (uint8_t i = 1; i < 5; i++) {
    for (uint8_t j = 1; j < 5; j++) {
      // Apply rule 90 to all cells
      uint8_t cellCount = cell[i-1][j] + cell[i+1][j] + cell[i][j-1] + cell[i][j+1];
      // Update to rule 150 for non-corner cells
      if (!(((1 == i) || (4 == i)) && ((1 == j) || (4 == j)))) {
        cellCount += cell[i][j];
      }
      // Apply survival probability to potentially ON cells
      if (cellCount & 0x01) {
        next[i][j] = (random(1750) > (1023 - analogRead(A4)) ? 1 : 0); // maximum useful ratio ~58%
      } else {
        next[i][j] = 0;
      }
    }
  }
  // Update the cells
  uint16_t cells = 0;
  for (uint8_t i = 1; i < 5; i++) {
    for (uint8_t j = 1; j < 5; j++) {
      cell[i][j] = next[i][j];
      cells <<= 1;
      cells |= cell[i][j];
    }
  }
  // Send the new generation to the gates and the display
  shiftReg(cells);
}

void shiftReg(uint16_t data) {
  //sends a 16-bit unsigned int to the 4094 shift registers
  digitalWrite(SRCLOCK, LOW);
  digitalWrite(SRSTROBE, LOW);
  shiftOut(SRDATA, SRCLOCK, LSBFIRST, data & 0x00FF);
  shiftOut(SRDATA, SRCLOCK, LSBFIRST, (data >> 8) & 0x00FF);
  digitalWrite(SRSTROBE, HIGH);
}