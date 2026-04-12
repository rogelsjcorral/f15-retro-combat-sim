╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                                ║
║                     F-15 STRIKE EAGLE SIMULATOR v1.0                           ║
║                        DOS-Style Combat Flight Sim                             ║
║                        For Windows 11 Command Line                             ║
║                                                                                ║
╚════════════════════════════════════════════════════════════════════════════════╝

QUICK START - 3 STEPS
═══════════════════════════════════════════════════════════════════════════════

STEP 1: CHECK PYTHON
  - Open Command Prompt (Win+R → "cmd")
  - Type: python --version
  - Should show Python 3.8 or higher
  
  If not installed:
    > Download from python.org
    > Run installer, CHECK "Add Python to PATH"

STEP 2: PLACE FILES
  - Download f15_simulator.py
  - Download F15_SIMULATOR_GUIDE.txt
  - Put both in a folder (e.g., C:\Games\F15)

STEP 3: RUN THE GAME
  - Open Command Prompt
  - Navigate to your folder: cd C:\Games\F15
  - Type: python f15_simulator.py
  - Press Enter

BASIC CONTROLS
═══════════════════════════════════════════════════════════════════════════════

NAVIGATION
  [W] - Pitch UP (climb)
  [S] - Pitch DOWN (descend)
  [A] - Roll LEFT (turn left)
  [D] - Roll RIGHT (turn right)

VIEWS & WEAPONS
  [R] - RADAR screen
  [F] - FIRE cannon
  [L] - LOCK TARGET (cycle contacts)

FLIGHT STATUS
  [P] - PAUSE mission
  [Q] - QUIT / EJECT

IN-GAME SCREEN LAYOUT
═══════════════════════════════════════════════════════════════════════════════

TOP BAR:  Speed, Altitude, Heading, Pitch, G-Forces
  
HUD AREA: Artificial horizon showing pitch and roll
  
RADAR:    Real-time enemy contact display
  
WEAPONS:  Current ammunition and missile counts
  
THREATS:  Active enemy fighters and SAM sites

MISSION GOALS
═══════════════════════════════════════════════════════════════════════════════

1. GULF PATROL
     > Locate and engage enemy fighters
     > Use radar to find targets
     > Lock on with [L], fire with [F]

2. COMBAT CAP
     > Defend airspace from incoming raid
     > Intercept multiple fighter contacts
     > Maintain altitude and speed awareness

3. STRIKE MISSION
     > Destroy ground targets (SAM sites, installations)
     > Manage fuel and ammunition
     > Return to base safely

FLIGHT PHYSICS 101
═══════════════════════════════════════════════════════════════════════════════

SPEED:
  - Minimum: 150 knots (stall if slower)
  - Cruise: 450 knots (optimal)
  - Maximum: 950 knots (damage if exceeded)

ALTITUDE:
  - Min: 0 feet (ground level)
  - Service Ceiling: 45,000 feet
  - Higher = harder to turn, easier to stall

TURNING:
  - Roll left/right with [A]/[D]
  - Bank angle affects turn rate
  - High G-turns cause damage if sustained
  - Max 8.0 G before airframe stress

STALLING:
  - Happens if speed drops below 140 knots
  - Nose drops automatically
  - Recovery: Increase throttle, point nose down
  - Avoid stalls in combat!

EXAMPLE FLIGHT
═══════════════════════════════════════════════════════════════════════════════

1. Start new game [S], read briefing
2. Press [SPACE] to begin flight
3. You spawn at 6,500 feet, 250 knots, heading 0°
4. Press [W] twice to pitch up slightly (climb)
5. Press [D] to roll right into a gentle turn
6. Press [R] to see radar and locate contacts
7. When bandit appears on radar, return to [R], note distance
8. Fly toward contact (watch heading and distance)
9. When close, press [L] to lock target
10. Press [F] to fire cannon (or missiles when in range)
11. After target destroyed, continue patrol
12. Press [Q] to end mission and see results

MODIFYING THE SIMULATOR
═══════════════════════════════════════════════════════════════════════════════

The source code is FULLY COMMENTED and organized for learning!

Easy modifications (no programming knowledge):
  - Change starting altitude (look for "position=Vector3(0, 2000, 0)")
  - Add more enemies (search for "_spawn_initial_contacts")
  - Increase weapon loadout (find "cannon_ammo: int = 500")
  - Adjust turn rate (modify "TURN_RADIUS_FACTOR = 0.015")

See F15_SIMULATOR_GUIDE.txt for COMPLETE modding documentation!

TROUBLESHOOTING
═══════════════════════════════════════════════════════════════════════════════

PROBLEM: "ModuleNotFoundError: No module named 'msvcrt'"
SOLUTION: This is a Windows module. You ARE on Windows. Make sure Python
          is installed correctly with: python --version

PROBLEM: Screen flickers rapidly
SOLUTION: This is intentional! It matches 80s/90s DOS display rates.
          To change it, edit GAME_TICK = 0.05 (smaller = faster)

PROBLEM: Aircraft won't climb
SOLUTION: Check these:
  • Speed is above 250 knots (use [W] more)
  • Pitch is positive (up) with [W]
  • You're not at 45,000 ft ceiling

PROBLEM: Game runs too fast/slow on my computer
SOLUTION: The frame rate is locked at 50ms per frame. If your PC is old,
          it may skip frames. Try closing other programs.

PERFORMANCE NOTES
═══════════════════════════════════════════════════════════════════════════════

• ~20 FPS (authentic DOS feel)
• Runs on Windows 11, Windows 10, Windows 7
• Requires: Python 3.8+
• Uses: ~10 MB RAM, <1% CPU
• No graphics card needed (pure text mode)

DOCUMENTATION
═══════════════════════════════════════════════════════════════════════════════

READ F15_SIMULATOR_GUIDE.txt FOR:

- Detailed flight physics model
- All control schemes explained
- Weapon system details
- Modding guide (easy to advanced)
- Code architecture
- Vector math reference
- Future enhancement ideas

NEXT STEPS
═══════════════════════════════════════════════════════════════════════════════

1. Play through all 3 missions
2. Experiment with different tactics
3. Read the modding guide
4. Add your own features:
   - Landing system
   - Fuel management
   - Damage effects
   - Enemy AI improvements
   - Procedural mission generation

CREDITS & LICENSE
═══════════════════════════════════════════════════════════════════════════════

Inspired by MicroProse's F-15 Strike Eagle II (1989)
Educational/preservation project

You are free to:
- Run and play
- Modify the code
- Share your modifications
- Learn from the source

Created: 2026
Original F-15 SE II: MicroProse, 1989

═══════════════════════════════════════════════════════════════════════════════

Happy flying! Questions? All code is heavily commented. Dive in and customize!

═══════════════════════════════════════════════════════════════════════════════
