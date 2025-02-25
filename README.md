# mres &ndash; A simple command-line resistance calculator for on-chip metals and vias
Mres is meant to provide a tool for chip/layout designers who want to estimate the metal resistance of the wiring, including vias.
The tool can give the default resistances for the provided metal/via geometry, but additional arguments can be given to control the width and the length of the metal wiring and the number of repetitions in x and y for vias.
If mres is called without any arguments it outputs a list of possible materials.
Help is printed by calling with `-h' or `--help'.
The normal mode is reached by calling with a material (e.g. `M1), and optionally a width and length (in this order) can be provided.
Here, the width is given in nanometers whereas the length is given in micrometers.

The metal/via data is not dynamically read from a configuration file but compiled into the binary.
The idea is that for every different technology node a suitable design environment is present anyway, where then the correct mres binary is selected.
This keeps things very simple.
An example of technology data for some dummy technology is given, which is quite easy to adapt to real nodes.
