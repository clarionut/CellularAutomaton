# Cellular Automaton

This repository contains details of an Arduino-based emulation of the [NonlinearCircuits Cellular Automata](https://www.nonlinearcircuits.com/modules/p/cellular-automata) synthesiser module.

![CellularAutomaton](https://github.com/user-attachments/assets/e6e95f51-a603-4c6c-a3c2-eee641102247)

The original is a tour-de-force of 4xxx CMOS circuit design, but a microcontroller-based implementation is much simpler to design and assemble, probably cheaper and, most importantly, offers the possibility for interesting additional functionality. In particular, this implemetation allows a probabilistic approach to the cellular automaton rules and can run using its own internal clock.

The rules (more details in the Arduino code) determine if each cell will be OFF or allowed to be ON in the next generation. An OFF cell will definitely be OFF but an 'allowed' cell may be ON or OFF depending on a voltage controlled Survival probability. VC inputs of 5V (or above) will set the Survival pobability to 1 (and the cell will definitely be ON) while inputs of 0V (or below) give a survival probability of 0 (and the cell will definitely be OFF). Intermediate values give an increasing probability of survival as the control voltage increases. This has the effect of varying the density of ON cells in the grid and increasing the randomness of the grid patterns. The individual Seed cell inputs also accept analogue (0 - 5V) control voltages rather than simple gates allowing probabilistic seeding of the grid. There is also a global Seed probability control which can be set manually or by an external control voltage allowing the Cellular Automaton to be run without requiring direct inputs to the individual seed cells.

In addition the module can run using an external clock or an internal clock with exponential voltage control. The clock mode is selected by a switch and the same input is used for both the external clock and voltage control of the internal clock.

Outputs are very similar to the Nonlinear Circuits original - a grid of 16 gate outputs (6V output in this case) which are high when the corresponding cell in the grid is ON and three pseudo-random CV outputs, one controlled by the top eight cells of the grid, one by the bottom eight cells and one by all cells combined.

## Hardware notes

The Arduino sketch requires 7 analogue inputs so I used a 16MHz Arduino Pro Mini board mounted on the PCB rather than an ATmega328P (only 6 analogue inputs) mounted directly on the board. This drives two CD4094B shift registers running on 6V which in turn drive the LED matrix and gate outputs. The 6V supply provides a normalled voltage to the probability and clock inputs via 10k resistors mounted on the input sockets; with the 50k input level potentiometers this provides 0 - 5V for each of the inputs in the absence of an external connection. Inputs are protected against out-of-range voltages. The LED matrix in the original is 3D printed and uses 16 green 3mm ultrabright LEDs. The STL files are on [Thingiverse](https://www.thingiverse.com/thing:6831147).

## KiCad files

The schematic and PCB files are provided in KiCad 7 format plus a PDF of the schematic. The schematic uses some custom symbols and the PCB some custom footprints, so you will need to download my [custom KiCad libraries](https://github.com/clarionut/kiCad_libraries) and tell KiCad where to find them. Note that Arduino Pro Minis don't all have the same footprint, so you may have to further customise the PCB. The PCB file is intended to be processed by the KiCad 'Round Tracks' plugin but will work perfectly well without. I've included the Gerber files but these will only be useful if you mount the PCB on brackets behind and perpendicular to the front panel.
