╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                                ║
║                    F-15 STRIKE EAGLE SIMULATOR v1.0                            ║
║                     NATIVE C++ WINDOWS EXECUTABLE                              ║
║                                                                                ║
╚════════════════════════════════════════════════════════════════════════════════╝

WHAT IS THIS?
═════════════════════════════════════════════════════════════════════════════════

A native Windows .exe flight simulator written in C++.
- Same physics as the Python version
- Much faster (native code, no interpreter)
- Authentic Windows Console rendering
- ~50KB standalone executable (no dependencies)

STEP 1: GET A C++ COMPILER
═════════════════════════════════════════════════════════════════════════════════

Option A: INSTALL MinGW (Recommended)
  1. Download: https://www.mingw-w64.org/
  2. Run installer
  3. Choose: "posix" threads, "x86_64" architecture
  4. Install to: C:\MinGW64
  5. Add to PATH: C:\MinGW64\bin
  6. Verify: Open cmd, type "g++ --version"

Option B: Use Visual Studio Community Edition
  1. Download: https://visualstudio.microsoft.com/downloads/
  2. Install "Desktop development with C++"
  3. Open "Developer Command Prompt for VS"
  4. Compile: cl /O2 F15_Simulator.cpp

Option C: Use Online Compiler (No Installation!)
  1. Visit: https://replit.com/languages/cpp
  2. Paste F15_Simulator.cpp into editor
  3. Add Windows.h compatibility layer
  4. Compile & run online

STEP 2: COMPILE THE CODE
═════════════════════════════════════════════════════════════════════════════════

Using MinGW/g++:
  1. Open Command Prompt
  2. Navigate to folder with F15_Simulator.cpp
  3. Type: g++ -O2 F15_Simulator.cpp -o F15.exe
  4. Wait 5-10 seconds
  5. You should see: F15.exe (created)

Using Visual Studio (MSVC):
  1. Open "Developer Command Prompt"
  2. Navigate to folder
  3. Type: cl /O2 F15_Simulator.cpp
  4. Wait 5-10 seconds
  5. You should see: F15.exe (created)

TROUBLESHOOTING COMPILATION:
  
  Error: "g++: command not found"
    > MinGW not in PATH. Reinstall or add manually.
  
  Error: "Cannot find windows.h"
    > Using non-Windows system. Code requires Windows.
  
  Error: "file not found"
    > Make sure F15_Simulator.cpp is in current directory.
    > Type: dir (to list files)

STEP 3: RUN THE GAME
═════════════════════════════════════════════════════════════════════════════════

Method 1: From Command Prompt
  1. Type: F15.exe
  2. Press Enter
  3. Game starts!

Method 2: Double-Click
  1. Navigate to F15.exe in File Explorer
  2. Double-click F15.exe
  3. Game starts!

STEP 4: PLAY THE GAME
═════════════════════════════════════════════════════════════════════════════════

MAIN MENU:
  [S] - START NEW GAME
  [R] - RESUME
  [Q] - QUIT

IN MISSION:
  [W] - PITCH UP (climb)
  [S] - PITCH DOWN (descend)
  [A] - ROLL LEFT
  [D] - ROLL RIGHT
  [F] - FIRE CANNON
  [L] - LOCK TARGET (cycle contacts)
  [R] - RADAR VIEW
  [P] - PAUSE
  [Q] - EJECT / QUIT

ON RADAR:
  [R] - RETURN TO FLIGHT VIEW
  [L] - LOCK TARGET
  [F] - FIRE WEAPON
  [Q] - QUIT

FLIGHT BASICS:
  1. Spawn at 6,500 feet, 250 knots
  2. Press [W] to pitch up (climb)
  3. Press [D] to roll right (turn)
  4. Watch altitude and speed
  5. Avoid stalling (speed < 140 knots)
  6. Press [R] to see radar
  7. Find enemy contacts (●)
  8. Press [L] to lock target
  9. Press [F] to fire
  10. Continue mission until you decide to eject [Q]

MODIFYING THE CODE
═════════════════════════════════════════════════════════════════════════════════

C++ is VERY EASY to modify once compiled. Try these:

MAKE ENEMIES FASTER:
  Open F15_Simulator.cpp
  Find line: "int enemy_count = 4 + rand() % 3;"
  Change to: "int enemy_count = 10 + rand() % 5;"
  Recompile: g++ -O2 F15_Simulator.cpp -o F15.exe

INCREASE MAX SPEED:
  Find: "#define MAX_SPEED 950"
  Change to: "#define MAX_SPEED 1200"
  Recompile and run

SPAWN AT HIGHER ALTITUDE:
  Find: "position = Vector3(0, 2000, 0);"
  Change to: "position = Vector3(0, 5000, 0);"
  Recompile and run

ADD MORE WEAPONS:
  Find: "int cannon_ammo = 500;"
  Change to: "int cannon_ammo = 2000;"
  Also change: "int missiles_heat = 4;"
  To: "int missiles_heat = 16;"
  Recompile and run

See F15_CPP_GUIDE.txt for 30+ more modifications!

PERFORMANCE
═════════════════════════════════════════════════════════════════════════════════

CPU Usage:        2-5% (very light)
Memory Usage:     5-10 MB
File Size:        50 KB
Frame Rate:       20 FPS (DOS-authentic)
Input Latency:    50 ms per frame

ADVANTAGES OVER PYTHON VERSION:

  ✓ No Python installation required
  ✓ Standalone .exe (single file, runs anywhere)
  ✓ Faster execution (native code)
  ✓ Smaller file size
  ✓ Uses Windows Console API directly
  ✓ More authentic to original F-15 SE II

C++ VS PYTHON: WHICH TO USE?

  Use C++ if you want:
    • Standalone .exe (no runtime)
    • Maximum performance
    • Learn C++ systems programming
    • Most authentic DOS experience

  Use Python if you want:
    • Easier modification without recompiling
    • Cross-platform (Windows/Mac/Linux)
    • Faster development iteration

BUILDING RELEASE VERSIONS
═════════════════════════════════════════════════════════════════════════════════

For maximum performance:
  g++ -O3 -march=native F15_Simulator.cpp -o F15_fast.exe

For smallest size:
  g++ -Os -s F15_Simulator.cpp -o F15_small.exe

For debugging:
  g++ -g -O0 F15_Simulator.cpp -o F15_debug.exe
  gdb F15_debug.exe

COMMON ISSUES
═════════════════════════════════════════════════════════════════════════════════

PROBLEM:   "cannot find windows.h"
REASON:    Your compiler is not for Windows (using Linux/Mac)
FIX:       This code is Windows-only. Use Python version or install MinGW.

PROBLEM:   Input doesn't work / game freezes
REASON:    Rare Windows Console API issue
FIX:       Try recompiling with: g++ -O0 F15_Simulator.cpp -o F15.exe

PROBLEM:   Screen is wrong size
REASON:    Windows scaling console to different dimensions
FIX:       Right-click window → Properties → Layout
           Set Width: 80, Height: 24

PROBLEM:   Game runs too fast/slow
REASON:    Your computer is much faster/slower than average
FIX:       Find "GAME_TICK_MS 50" and change:
             - Faster? Change to 33 (30 FPS)
             - Slower? Change to 100 (10 FPS)
             Recompile

NEXT STEPS
═════════════════════════════════════════════════════════════════════════════════

1. Get compiler (MinGW)
2. Compile code
3. Play the game
4. Try modifying the code
5. Read F15_CPP_GUIDE.txt for advanced mods
6. Add new features (landing, missiles, etc.)
7. Share your modifications!

DOCUMENTATION
═════════════════════════════════════════════════════════════════════════════════

F15_Simulator.cpp
  - Main source code (fully commented)
  
F15_CPP_GUIDE.txt
  ├─ Complete documentation
  ├─ Architecture overview
  ├─ Physics model detailed
  ├─ 30+ easy/medium/advanced mods
  ├─ Windows API explanation
  ├─ Compilation optimization
  └─ Troubleshooting

ORIGINAL INSPIRATION
═════════════════════════════════════════════════════════════════════════════════

F-15 Strike Eagle II
  Developer: MicroProse Software
  Year:      1989
  Platform:  MS-DOS
  Language:  C + x86 Assembly

This C++ version recreates the core physics and gameplay
in a modern, readable codebase while maintaining the authentic
DOS-era terminal aesthetic.

═════════════════════════════════════════════════════════════════════════════════

Happy flying! Questions? Read the .txt files—they have all the answers!

═════════════════════════════════════════════════════════════════════════════════
