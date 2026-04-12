╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                                ║
║                    F-15 STRIKE EAGLE SIMULATOR v1.1                            ║
║                     NATIVE C++ WINDOWS EXECUTABLE                              ║
║                                                                                ║
╚════════════════════════════════════════════════════════════════════════════════╝

WHAT'S NEW IN v1.1
═════════════════════════════════════════════════════════════════════════════════

✓ TRUE PAUSE SYSTEM
  • [P] now pauses the simulation (not mission end)
  • Resume or abort from pause screen

✓ SMOOTH FLIGHT CONTROLS
  • Hold keys for continuous pitch/roll
  • No more "tap-tap" flying

✓ INPUT FIXES
  • Key debounce prevents rapid unintended toggles
  • Radar/menu switching is now stable

✓ ACCURATE MISSION TIMER
  • Uses real elapsed time (not frame count)
  • Debrief shows proper seconds

✓ TARGET LOCK CLEANUP
  • Only one target can be locked at a time
  • Radar now shows LOCK status

═════════════════════════════════════════════════════════════════════════════════

WHAT IS THIS?
═════════════════════════════════════════════════════════════════════════════════

A native Windows .exe flight simulator written in C++.

• Standalone executable (no dependencies)
• Authentic DOS-style console rendering
• Real-time flight physics (F-15 inspired)
• ~50KB binary

═════════════════════════════════════════════════════════════════════════════════

QUICK START
═════════════════════════════════════════════════════════════════════════════════

Compile:
  g++ -O2 F15_Simulator.cpp -o F15.exe

Run:
  F15.exe

═════════════════════════════════════════════════════════════════════════════════

CONTROLS
═════════════════════════════════════════════════════════════════════════════════

FLIGHT:
  [W] - Pitch UP (hold)
  [S] - Pitch DOWN (hold)
  [A] - Roll LEFT (hold)
  [D] - Roll RIGHT (hold)

COMBAT:
  [F] - Fire cannon
  [L] - Lock target (cycles contacts)

SYSTEM:
  [R] - Radar view
  [P] - Pause
  [Q] - End mission / Quit

═════════════════════════════════════════════════════════════════════════════════

PERFORMANCE
═════════════════════════════════════════════════════════════════════════════════

Frame Rate:     ~20 FPS (DOS-authentic)
CPU Usage:      2-5%
Memory:         ~5-10 MB
Latency:        ~50 ms

═════════════════════════════════════════════════════════════════════════════════

NOTES
═════════════════════════════════════════════════════════════════════════════════

v1.1 focuses on control stability and timing accuracy.

Next phase (v1.2):
  → Flight model cleanup
  → Improved physics coherence

═════════════════════════════════════════════════════════════════════════════════
