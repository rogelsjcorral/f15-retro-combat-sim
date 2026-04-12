F-15 STRIKE EAGLE SIMULATOR v1.0
================================

DOS-Style Combat Flight Simulator
Python and C++ Versions Included

---------------------------------------------------------------------

COPYRIGHT

Copyright (c) 2026 Rogel S.J. Corral.
Licensed under the MIT License.

This project is distributed for educational and modification purposes.
Refer to included documentation for usage details.

---------------------------------------------------------------------

WHAT IS THIS?

This is a retro-style combat flight simulator inspired by
MicroProse-era titles such as F-15 Strike Eagle II (1989).

It is designed to recreate the experience of early flight simulators
using:

- text-mode (ASCII) cockpit display
- simplified but coherent flight physics
- radar-based targeting
- command-line execution environment

Two versions are included:

PYTHON VERSION
  - Easier to run and modify
  - Cross-platform
  - No compilation required

C++ VERSION
  - Native Windows executable
  - Faster performance
  - Uses Windows Console API
  - Standalone executable

Both versions share the same core simulation model.

---------------------------------------------------------------------

QUICK START

PYTHON VERSION:

    python prototypes/f15_simulator.py

C++ VERSION:

    g++ -O2 src/F15_Simulator.cpp -o F15.exe
    F15.exe

See:

    docs/README.txt
    docs/README_CPP.txt

---------------------------------------------------------------------

BASIC CONTROLS

FLIGHT

    W / S     Pitch Up / Down
    A / D     Roll Left / Right

COMBAT

    F         Fire Cannon
    L         Lock Target

SYSTEM

    R         Radar Screen
    P         Pause Mission
    Q         Quit / Eject
    SPACE     Confirm / Continue

---------------------------------------------------------------------

SIMULATION MODEL

The simulator follows a simplified F-15 flight model:

SPEED
    Stall:     ~140 knots
    Cruise:    ~450 knots
    Maximum:   ~950 knots

ALTITUDE
    Ground to 50,000 feet
    Service ceiling ~45,000 ft

TURNING
    Bank angle determines turn rate
    High-G turns increase damage risk

STALL
    Occurs at low speed
    Nose drops automatically
    Recovery requires throttle and descent

DAMAGE
    Excessive G-forces cause structural damage
    Overspeed introduces additional stress

---------------------------------------------------------------------

PROJECT STRUCTURE

    src/            C++ implementation
    prototypes/     Python implementation
    docs/           Guides and documentation
    assets/         Screenshots and future media

Key documentation:

    docs/START_HERE.txt
    docs/README.txt
    docs/README_CPP.txt
    docs/F15_SIMULATOR_GUIDE.txt
    docs/F15_CPP_GUIDE.txt

---------------------------------------------------------------------

DESIGN INTENT

This project is not a full real-world simulator.

It is designed to balance:

- playability
- clarity
- performance
- educational value

The goal is to recreate the feel of early combat sims:

limited interface
direct control
fast decision making

No automation. No assistance. Only pilot input.

---------------------------------------------------------------------

CURRENT STATUS

Version: v1.0 (Python and C++)

Implemented:
- flight physics model
- radar system
- enemy contacts
- basic weapons
- terrain model
- mission loop

In Progress:
- improved combat engagement
- damage effects
- enemy behavior refinement

Planned:
- landing system refinement
- missile guidance logic
- sound effects
- mission expansion

---------------------------------------------------------------------

DOCUMENTATION

For full details, read:

    docs/F15_SIMULATOR_GUIDE.txt
    docs/F15_CPP_GUIDE.txt

These include:

- architecture overview
- physics model
- modding guide
- vector math reference
- advanced feature ideas

---------------------------------------------------------------------

FINAL NOTE

This project exists to demonstrate that simulation depth
does not require modern graphics or large frameworks.

Only structure, discipline, and a clear model.

---------------------------------------------------------------------

Copyright (c) 2026 Rogel S.J. Corral.
Licensed under the MIT License.
