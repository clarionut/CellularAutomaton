# Cellular Automaton

This repository contains details of an Arduino-based emulation of the [NonlinearCircuits Cellular Automata](https://www.nonlinearcircuits.com/modules/p/cellular-automata) synthesiser module.

![CellularAutomaton](https://github.com/user-attachments/assets/e6e95f51-a603-4c6c-a3c2-eee641102247)

The original is a tour-de-force of 4xxx CMOS circuit design, but a microcontroller-based implementation is much simpler to design and assemble, probably cheaper and, most importantly, offers the possibility for interesting additional functionality. In particular, this implemetation allows a probabilistic approach to the cellular automaton rules and can run using its own internal clock.

The rules (more details in the Arduino code) determine if each cell will be OFF or allowed to be ON in the next generation. An OFF cell will definitely be OFF but an 'allowed' cell may be ON or OFF depending on a voltage controlled Survival probability. VC inputs of 5V (or above) will set the Survival pobability to 1 (and the cell will definitely be ON) while inputs of 0V (or below) give a survival probability of 0 (and the cell will definitely be OFF). Intermediate values give an increasing probability of survival as the control voltage increases. This has the effect of varying the density of ON cells in the grid and increasing the randomness of the grid patterns. The individual Seed cell inputs also accept analogue (0 - 5V) control voltages rather than simple gates allowing probabilistic seeding of the grid. There is also a global Seed probability control which can be set manually or by an external control voltage allowing the Cellular Automaton to be run without requiring direct inputs to the individual seed cells.

In addition the module can run using an external clock or an internal clock with exponential voltage control. The clock mode is selected by a switch and the same input is used for both the external clock and voltage control of the internal clock.

Outputs are very similar to the Nonlinear Circuits original - a grid of 16 gate outputs (6V output in this case) which are high when the corresponding cell in the grid is ON and three pseudo-random CV outputs, one controlled by the top eight cells of the grid, one by the bottom eight cells and one by all cells combined.

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
