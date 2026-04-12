/*
 * ==================================================================
 *   F-15 STRIKE EAGLE SIMULATOR v1.0
 * ==================================================================
 *   DOS-Style Combat Flight Simulator
 *   Written in C++ for Windows 11
 * 
 *   Inspired by MicroProse's 1989 classic
 *   Physics model based on F-15E Strike Eagle aerodynamics
 * 
 *   Compile with:
 *     g++ -O2 F15_Simulator.cpp -o F15.exe
 *     OR
 *     cl /O2 F15_Simulator.cpp
 * 
 *   No external dependencies - uses Windows Console API directly
 *
 *  Copyright (c) 2026 Rogel S.J. Corral.
 *  Licensed under the MIT License.
 * ==================================================================
 */

#include <windows.h>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cstdarg>

// ============================================================================
// CONSTANTS & CONFIGURATION
// ============================================================================

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 24
#define GAME_TICK_MS 50  // 50ms per frame = ~20 FPS

// Aircraft limits (knots)
#define MIN_SPEED 150
#define MAX_SPEED 950
#define CRUISE_SPEED 450
#define STALL_SPEED 140

// Altitude limits (feet)
#define MIN_ALTITUDE 0
#define MAX_ALTITUDE 50000
#define CLIMB_CEILING 45000

// Physics parameters
#define DRAG_COEFFICIENT 0.08f
#define THRUST_RESPONSE 2.0f
#define G_LIMIT 8.0f
#define TURN_RADIUS_FACTOR 0.015f

// ============================================================================
// ENUMS
// ============================================================================

enum FlightPhase {
    MAIN_MENU = 0,
    MISSION_BRIEFING = 1,
    IN_FLIGHT = 2,
    RADAR_SCREEN = 3,
    LANDING = 4,
    MISSION_RESULTS = 5,
    GAME_OVER = 6
};

enum ContactType {
    FRIENDLY_FIGHTER = 0,
    ENEMY_FIGHTER = 1,
    SAM_SITE = 2,
    GROUND_TARGET = 3,
    WAYPOINT = 4
};

// ============================================================================
// DATA STRUCTURES
// ============================================================================

struct Vector3 {
    float x, y, z;
    
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    
    float magnitude() const {
        return sqrtf(x*x + y*y + z*z);
    }
    
    Vector3 normalize() const {
        float mag = magnitude();
        if (mag < 0.001f) return Vector3(0, 0, 0);
        return Vector3(x/mag, y/mag, z/mag);
    }
    
    float dot(const Vector3& other) const {
        return x*other.x + y*other.y + z*other.z;
    }
    
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x+other.x, y+other.y, z+other.z);
    }
    
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x-other.x, y-other.y, z-other.z);
    }
    
    Vector3 operator*(float scalar) const {
        return Vector3(x*scalar, y*scalar, z*scalar);
    }
};

struct Aircraft {
    Vector3 position;
    Vector3 velocity;
    float pitch;         // -90 to +90 degrees
    float roll;          // -180 to +180 degrees
    float heading;       // 0-360 degrees
    float throttle;      // 0.0 to 1.0
    
    float g_force;
    bool on_ground;
    bool damaged;
    float damage_level;
    
    // Weapons
    int cannon_ammo;
    int missiles_heat;
    int missiles_radar;
    int missiles_agm;
    int bombs;
    
    // Instruments
    int radar_range;
    bool autopilot;
    
    Aircraft() : pitch(0), roll(0), heading(0), throttle(0.6f),
                 g_force(1.0f), on_ground(false), damaged(false),
                 damage_level(0.0f), cannon_ammo(500), missiles_heat(4),
                 missiles_radar(4), missiles_agm(8), bombs(12),
                 radar_range(60), autopilot(false) {
        position = Vector3(0, 2000, 0);
        velocity = Vector3(100, 0, 0);
    }
    
    float currentSpeed() const {
        return velocity.magnitude() * 1.944f;  // m/s to knots
    }
    
    float currentAltitude() const {
        return position.y * 3.28084f;  // meters to feet
    }
    
    bool isStalled() const {
        return currentSpeed() < STALL_SPEED;
    }
};

struct Contact {
    ContactType type;
    Vector3 position;
    Vector3 velocity;
    float heading;
    float altitude;
    int threat_level;
    bool locked;
    
    Contact() : type(ENEMY_FIGHTER), heading(0), altitude(0),
                threat_level(0), locked(false) {}
};

struct Terrain {
    float getElevation(float x, float z) const {
        // Gaussian peaks for mountains
        float mountain1 = 200.0f * expf(-((x-5000)*(x-5000) + (z-5000)*(z-5000)) / 5000000.0f);
        float mountain2 = 150.0f * expf(-((x-15000)*(x-15000) + (z-10000)*(z-10000)) / 8000000.0f);
        return mountain1 + mountain2;
    }
};

// ============================================================================
// GLOBAL STATE
// ============================================================================

struct GameState {
    FlightPhase phase;
    Aircraft aircraft;
    Terrain terrain;
    Contact contacts[20];
    int contact_count;
    int time_in_flight;
    int mission_score;
    int radar_contacts_destroyed;
    int current_mission;
    int selected_contact_idx;
    
    GameState() : phase(MAIN_MENU), contact_count(0), time_in_flight(0),
                  mission_score(0), radar_contacts_destroyed(0),
                  current_mission(0), selected_contact_idx(-1) {
        spawnInitialContacts();
    }
    
    void spawnInitialContacts() {
        contact_count = 0;
        
        // Friendly wingman
        contacts[contact_count].type = FRIENDLY_FIGHTER;
        contacts[contact_count].position = Vector3(1000, 2000, -1000);
        contacts[contact_count].velocity = Vector3(100, 0, 0);
        contacts[contact_count].heading = 0;
        contacts[contact_count].altitude = 6500;
        contacts[contact_count].threat_level = 0;
        contact_count++;
        
        // Enemy fighters
        int enemy_count = 4 + rand() % 3;
        for (int i = 0; i < enemy_count && contact_count < 20; i++) {
            contacts[contact_count].type = ENEMY_FIGHTER;
            contacts[contact_count].position = Vector3(
                10000.0f + (rand() % 20000),
                2500,
                -5000.0f + (rand() % 20000)
            );
            contacts[contact_count].velocity = Vector3(
                -80.0f - (rand() % 40),
                0,
                -50.0f + (rand() % 100)
            );
            contacts[contact_count].heading = rand() % 360;
            contacts[contact_count].altitude = 8000 + (rand() % 2000) - 1000;
            contacts[contact_count].threat_level = 6 + (rand() % 4);
            contact_count++;
        }
        
        // SAM sites
        for (int i = 0; i < 3 && contact_count < 20; i++) {
            contacts[contact_count].type = SAM_SITE;
            contacts[contact_count].position = Vector3(
                15000.0f + (rand() % 25000),
                0,
                5000.0f + (rand() % 15000)
            );
            contacts[contact_count].heading = 0;
            contacts[contact_count].altitude = 0;
            contacts[contact_count].threat_level = 5 + (rand() % 4);
            contact_count++;
        }
        
        // Ground targets
        for (int i = 0; i < 2 && contact_count < 20; i++) {
            contacts[contact_count].type = GROUND_TARGET;
            contacts[contact_count].position = Vector3(
                20000.0f + (rand() % 25000),
                0,
                10000.0f + (rand() % 15000)
            );
            contacts[contact_count].heading = 0;
            contacts[contact_count].altitude = 0;
            contacts[contact_count].threat_level = 3 + (rand() % 3);
            contact_count++;
        }
    }
};

// ============================================================================
// WINDOWS CONSOLE UTILITIES
// ============================================================================

HANDLE hStdOut = INVALID_HANDLE_VALUE;

void initConsole() {
    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    
    // Set console size
    COORD dwSize = {SCREEN_WIDTH, SCREEN_HEIGHT};
    SMALL_RECT srWindow = {0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1};
    SetConsoleScreenBufferSize(hStdOut, dwSize);
    SetConsoleWindowInfo(hStdOut, TRUE, &srWindow);
    
    // Hide cursor
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hStdOut, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hStdOut, &cursorInfo);
    
    // Set title
    SetConsoleTitleA("F-15 STRIKE EAGLE SIMULATOR v1.0");
}

void clearScreen() {
    COORD coordScreen = {0, 0};
    DWORD cCharsWritten;
    SetConsoleCursorPosition(hStdOut, coordScreen);
    FillConsoleOutputCharacterA(hStdOut, ' ', SCREEN_WIDTH * SCREEN_HEIGHT, coordScreen, &cCharsWritten);
    SetConsoleCursorPosition(hStdOut, coordScreen);
}

void setCursorPos(int x, int y) {
    COORD coord = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(hStdOut, coord);
}

void printAt(int x, int y, const char* format, ...) {
    setCursorPos(x, y);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
}

// ============================================================================
// PHYSICS ENGINE
// ============================================================================

void updateAircraft(Aircraft& aircraft, const Terrain& terrain, float dt) {
    // Throttle to thrust
    float currentSpeed = aircraft.currentSpeed();
    float maxThrust = 250.0f;  // m/s
    float minThrust = 50.0f;
    
    float targetSpeed = minThrust + aircraft.throttle * (maxThrust - minThrust);
    float velocityMag = aircraft.velocity.magnitude();
    
    float accel;
    if (velocityMag < targetSpeed) {
        accel = THRUST_RESPONSE;
    } else {
        accel = -DRAG_COEFFICIENT * (velocityMag - targetSpeed);
    }
    
    // Pitch to vertical acceleration
    float pitchRad = aircraft.pitch * 3.14159f / 180.0f;
    float verticalAccel = sinf(pitchRad) * 9.81f;
    
    // Roll to turn rate
    float rollRad = aircraft.roll * 3.14159f / 180.0f;
    float turnRate = sinf(rollRad) * TURN_RADIUS_FACTOR * velocityMag;
    
    // Update heading
    aircraft.heading += turnRate * dt;
    if (aircraft.heading >= 360.0f) aircraft.heading -= 360.0f;
    if (aircraft.heading < 0.0f) aircraft.heading += 360.0f;
    
    // Update position
    float headingRad = aircraft.heading * 3.14159f / 180.0f;
    aircraft.position.x += cosf(headingRad) * accel * dt;
    aircraft.position.z += sinf(headingRad) * accel * dt;
    aircraft.position.y += verticalAccel * dt;
    
    // Update velocity
    aircraft.velocity.x = cosf(headingRad) * targetSpeed;
    aircraft.velocity.z = sinf(headingRad) * targetSpeed;
    aircraft.velocity.y = sinf(pitchRad) * targetSpeed * 0.5f;
    
    // Terrain collision
    float terrainElev = terrain.getElevation(aircraft.position.x, aircraft.position.z);
    if (aircraft.position.y < terrainElev + 100) {
        aircraft.position.y = terrainElev + 100;
        aircraft.on_ground = true;
    } else {
        aircraft.on_ground = false;
    }
    
    // Altitude ceiling
    if (aircraft.position.y > CLIMB_CEILING) {
        aircraft.position.y = CLIMB_CEILING;
        if (aircraft.pitch > 0) aircraft.pitch = 0;
    }
    
    // Stall behavior
    if (aircraft.isStalled()) {
        aircraft.pitch = std::max(aircraft.pitch - 5.0f, -45.0f);
    }
    
    // G-force calculation
    float accelTotal = sqrtf(accel*accel + verticalAccel*verticalAccel + turnRate*turnRate);
    aircraft.g_force = std::max(1.0f, 1.0f + accelTotal / 9.81f);
    
    // Damage from excessive G
    if (aircraft.g_force > G_LIMIT) {
        aircraft.damage_level += 0.01f * (aircraft.g_force - G_LIMIT);
        aircraft.damaged = true;
    }
    
    // Overspeed damage
    if (currentSpeed > MAX_SPEED * 1.1f) {
        aircraft.damage_level += 0.005f;
        aircraft.damaged = true;
    }
}

void updateContacts(Contact* contacts, int count, float dt) {
    for (int i = 0; i < count; i++) {
        if (contacts[i].type == ENEMY_FIGHTER) {
            // Simple AI wandering
            if ((rand() % 100) < 2) {
                contacts[i].heading += (rand() % 20) - 10;
            }
            
            // Update position
            float headingRad = contacts[i].heading * 3.14159f / 180.0f;
            float speed = contacts[i].velocity.magnitude();
            contacts[i].position.x += cosf(headingRad) * speed * dt;
            contacts[i].position.z += sinf(headingRad) * speed * dt;
            contacts[i].position.y += ((rand() % 100) - 50) * 0.5f * dt;
        }
    }
}

// ============================================================================
// RENDERING
// ============================================================================

void renderMainMenu() {
    clearScreen();
    
    printAt(1, 0,  "╔════════════════════════════════════════════════════════════════════════════════╗");
    printAt(1, 1,  "║                                                                                ║");
    printAt(1, 2,  "║                    F-15 STRIKE EAGLE SIMULATOR v1.0                            ║");
    printAt(1, 3,  "║                      Inspired by MicroProse 1989                               ║");
    printAt(1, 4,  "║                                                                                ║");
    printAt(1, 5,  "╠════════════════════════════════════════════════════════════════════════════════╣");
    printAt(1, 6,  "║                                                                                ║");
    printAt(1, 7,  "║  [S] START NEW GAME                                                            ║");
    printAt(1, 8,  "║  [R] RESUME FLIGHT                                                             ║");
    printAt(1, 9,  "║  [Q] QUIT                                                                      ║");
    printAt(1, 10, "║                                                                                ║");
    printAt(1, 11, "╚════════════════════════════════════════════════════════════════════════════════╝");
}

void renderMissionBriefing(const GameState& state) {
    clearScreen();
    
    const char* missions[] = {"GULF PATROL", "COMBAT CAP", "STRIKE MISSION"};
    const char* objectives[] = {
        "Locate and engage enemy fighters",
        "Defend airspace from incoming raid",
        "Destroy SAM sites and ground targets"
    };
    
    int mission_idx = state.current_mission % 3;
    
    printAt(1, 0,  "╔════════════════════════════════════════════════════════════════════════════════╗");
    printAt(1, 1,  "║                         MISSION BRIEFING                                       ║");
    printAt(1, 2,  "╠════════════════════════════════════════════════════════════════════════════════╣");
    printAt(1, 3,  "║                                                                                ║");
    printAt(1, 4,  "║ MISSION: %-65s║", missions[mission_idx]);
    printAt(1, 5,  "║ OBJECTIVE: %-62s║", objectives[mission_idx]);
    printAt(1, 6,  "║                                                                                ║");
    printAt(1, 7,  "║  TACTICAL SITUATION:                                                           ║");
    printAt(1, 8,  "║  - Enemy fighters detected in patrol sector                                    ║");
    printAt(1, 9,  "║  - Multiple SAM sites active                                                   ║");
    printAt(1, 10, "║  - Wingman will provide support                                                ║");
    printAt(1, 11, "║                                                                                ║");
    printAt(1, 12, "║  LOADOUT: 500 rounds cannon, 4x AIM-9, 4x AIM-7, 8x AGM-65, 12x Mk82           ║");
    printAt(1, 13, "║                                                                                ║");
    printAt(1, 14, "║ [SPACE] TO BEGIN FLIGHT                                                        ║");
    printAt(1, 15, "║                                                                                ║");
    printAt(1, 16, "╚════════════════════════════════════════════════════════════════════════════════╝");
}

void renderInFlight(const GameState& state) {
    clearScreen();
    
    const Aircraft& a = state.aircraft;
    
    // Top bar
    printAt(0, 0, "┌─────────────────────────────────────────────────────────────────────────────────┐");
    printAt(1, 1, "SPD:%3.0f kt  ALT:%5.0f ft  HDG:%3.0f°  PITCH:%+3.0f°  G:%.1f",
            a.currentSpeed(), a.currentAltitude(), a.heading, a.pitch, a.g_force);
    printAt(0, 2, "├─────────────────────────────────────────────────────────────────────────────────┤");
    
    // Artificial horizon (simplified)
    for (int i = 0; i < 6; i++) {
        if (i == 3) {
            printAt(1, 3+i, "═════════════════════════════════════════════════════════════════════════════");
        } else {
            printAt(1, 3+i, "                                                                             ");
        }
    }
    
    printAt(0, 9, "├─────────────────────────────────────────────────────────────────────────────────┤");
    
    // Radar contacts (count)
    int enemy_count = 0;
    int sam_count = 0;
    for (int i = 0; i < state.contact_count; i++) {
        if (state.contacts[i].type == ENEMY_FIGHTER) enemy_count++;
        if (state.contacts[i].type == SAM_SITE) sam_count++;
    }
    
    printAt(1, 10, "CANNON:%3d  MSL-H:%d  MSL-R:%d  AGM:%2d  BOMB:%2d  |  %d BANDITS  %d SAM SITES",
            a.cannon_ammo, a.missiles_heat, a.missiles_radar, a.missiles_agm, a.bombs,
            enemy_count, sam_count);
    
    printAt(0, 11, "├─────────────────────────────────────────────────────────────────────────────────┤");
    printAt(1, 12, "[W/S] PITCH  [A/D] ROLL  [R] RADAR  [F] FIRE  [L] LOCK  [P] PAUSE  [Q] EJECT    ");
    printAt(0, 13, "└─────────────────────────────────────────────────────────────────────────────────┘");
}

void renderRadarScreen(const GameState& state) {
    clearScreen();
    
    printAt(1, 0,  "╔════════════════════════════════════════════════════════════════════════════════╗");
    printAt(1, 1,  "║                      TACTICAL RADAR DISPLAY                                    ║");
    printAt(1, 2,  "╠════════════════════════════════════════════════════════════════════════════════╣");
    
    for (int i = 0; i < 10; i++) {
        printAt(1, 3+i, "║                                                                                ║");
    }
    
    printAt(1, 13, "║ CONTACTS:                                                                      ║");
    
    for (int i = 0; i < std::min(state.contact_count, 7); i++) {
        const Contact& c = state.contacts[i];
        const char* type_char = "?";
        if (c.type == FRIENDLY_FIGHTER) type_char = "◆";
        else if (c.type == ENEMY_FIGHTER) type_char = "●";
        else if (c.type == SAM_SITE) type_char = "▲";
        else if (c.type == GROUND_TARGET) type_char = "■";
        
        Vector3 delta = c.position - state.aircraft.position;
        float distance = delta.magnitude() * 0.000539957f;  // meters to nm
        
        printAt(1, 14+i, "║ %s %s %6.1f nm  ALT %5.0f ft  TGT:%d                                      ║",
                type_char, type_char, distance, c.altitude, c.threat_level);
    }
    
    printAt(1, 22, "║ [R] RETURN  [L] LOCK  [F] FIRE  [Q] QUIT                                      ║");
    printAt(1, 23, "╚════════════════════════════════════════════════════════════════════════════════╝");
}

void renderMissionResults(const GameState& state) {
    clearScreen();
    
    printAt(1, 0,  "╔════════════════════════════════════════════════════════════════════════════════╗");
    printAt(1, 1,  "║                         MISSION DEBRIEF                                        ║");
    printAt(1, 2,  "╠════════════════════════════════════════════════════════════════════════════════╣");
    printAt(1, 3,  "║                                                                                ║");
    printAt(1, 4,  "║ TIME IN FLIGHT: %6d seconds                                                  ║", state.time_in_flight);
    printAt(1, 5,  "║ CONTACTS DESTROYED: %d                                                        ║", state.radar_contacts_destroyed);
    printAt(1, 6,  "║ MISSION SCORE: %d                                                             ║", state.mission_score);
    printAt(1, 7,  "║                                                                                ║");
    printAt(1, 8,  "║ [SPACE] NEXT MISSION  [Q] RETURN TO MAIN MENU                                 ║");
    printAt(1, 9,  "║                                                                                ║");
    printAt(1, 10, "╚════════════════════════════════════════════════════════════════════════════════╝");
}

void renderScreen(const GameState& state) {
    switch (state.phase) {
        case MAIN_MENU:
            renderMainMenu();
            break;
        case MISSION_BRIEFING:
            renderMissionBriefing(state);
            break;
        case IN_FLIGHT:
            renderInFlight(state);
            break;
        case RADAR_SCREEN:
            renderRadarScreen(state);
            break;
        case MISSION_RESULTS:
            renderMissionResults(state);
            break;
        default:
            break;
    }
}

// ============================================================================
// INPUT HANDLING
// ============================================================================

bool handleInput(GameState& state) {
    if (!GetAsyncKeyState(VK_ESCAPE)) {
        // Non-blocking key check using Windows API
        for (int key = 0; key < 256; key++) {
            if (GetAsyncKeyState(key) & 0x8000) {
                char c = (char)key;
                c = toupper(c);
                
                if (state.phase == MAIN_MENU) {
                    if (c == 'S') state.phase = MISSION_BRIEFING;
                    if (c == 'Q') return false;
                }
                else if (state.phase == MISSION_BRIEFING) {
                    if (c == ' ') {
                        state.phase = IN_FLIGHT;
                        state.time_in_flight = 0;
                    }
                }
                else if (state.phase == IN_FLIGHT) {
                    if (c == 'Q') state.phase = MISSION_RESULTS;
                    if (c == 'P') state.phase = MISSION_RESULTS;
                    if (c == 'R') state.phase = RADAR_SCREEN;
                    if (c == 'W') state.aircraft.pitch = std::min(state.aircraft.pitch + 2.0f, 45.0f);
                    if (c == 'S') state.aircraft.pitch = std::max(state.aircraft.pitch - 2.0f, -45.0f);
                    if (c == 'A') state.aircraft.roll = std::max(state.aircraft.roll - 3.0f, -90.0f);
                    if (c == 'D') state.aircraft.roll = std::min(state.aircraft.roll + 3.0f, 90.0f);
                    if (c == 'F') {
                        if (state.aircraft.cannon_ammo > 0) {
                            state.aircraft.cannon_ammo -= 10;
                            state.mission_score += rand() % 10 + 1;
                        }
                    }
                    if (c == 'L') {
                        if (state.contact_count > 0) {
                            state.selected_contact_idx = (state.selected_contact_idx + 1) % state.contact_count;
                            state.contacts[state.selected_contact_idx].locked = true;
                        }
                    }
                }
                else if (state.phase == RADAR_SCREEN) {
                    if (c == 'R') state.phase = IN_FLIGHT;
                    if (c == 'Q') return false;
                }
                else if (state.phase == MISSION_RESULTS) {
                    if (c == ' ') {
                        state.current_mission++;
                        state.phase = MISSION_BRIEFING;
                        state.spawnInitialContacts();
                    }
                    if (c == 'Q') state.phase = MAIN_MENU;
                }
                
                return true;
            }
        }
    }
    return true;
}

// ============================================================================
// MAIN GAME LOOP
// ============================================================================

int main() {
    initConsole();
    srand((unsigned)time(NULL));
    
    GameState state;
    
    printf("\n%s\n", "════════════════════════════════════════════════════════════════════════════════");
    printf("%s\n", "F-15 STRIKE EAGLE SIMULATOR - Press any key to begin...");
    printf("%s\n\n", "════════════════════════════════════════════════════════════════════════════════");
    
    Sleep(2000);  // 2 second splash screen
    
    LARGE_INTEGER lastTime, currentTime, frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);
    
    while (true) {
        // Frame timing
        QueryPerformanceCounter(&currentTime);
        float dt = (float)(currentTime.QuadPart - lastTime.QuadPart) / (float)frequency.QuadPart;
        lastTime = currentTime;
        
        // Cap dt to game tick
        if (dt > (GAME_TICK_MS / 1000.0f) * 2) dt = GAME_TICK_MS / 1000.0f;
        
        // Input
        if (!handleInput(state)) break;
        
        // Physics update
        if (state.phase == IN_FLIGHT) {
            updateAircraft(state.aircraft, state.terrain, dt);
            updateContacts(state.contacts, state.contact_count, dt);
            state.time_in_flight++;
        }
        
        // Render
        renderScreen(state);
        
        // Frame delay (50ms = 20 FPS)
        Sleep(GAME_TICK_MS);
    }
    
    clearScreen();
    printf("Thank you for flying F-15 STRIKE EAGLE!\n");
    Sleep(1000);
    
    return 0;
}
