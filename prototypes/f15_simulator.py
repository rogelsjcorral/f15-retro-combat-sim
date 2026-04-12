#!/usr/bin/env python3
"""
F-15 STRIKE EAGLE SIMULATOR v1.0
Inspired by MicroProse's 1989 classic
For MS-DOS compatible command-line environments

Flight Physics Model:
- Simplified aerodynamic model inspired by F-15 SE II
- Realistic stall behavior, pitch dynamics, speed envelope
- Terrain elevation, SAM threats, enemy contacts
- Weapons: guns, heat-seekers, radar missiles, dumb bombs

Copyright (c) 2026 Rogel S.J. Corral.
Licensed under the MIT License.
"""

import os
import sys
import math
import random
import time
from enum import Enum
from dataclasses import dataclass
from typing import List, Tuple, Optional

# Cross-platform input handling
try:
    import msvcrt  # Windows console input
    WINDOWS = True
except ImportError:
    WINDOWS = False
    if not WINDOWS:
        import tty
        import termios

# ============================================================================
# CONSTANTS & CONFIGURATION
# ============================================================================

SCREEN_WIDTH = 80
SCREEN_HEIGHT = 24
GAME_TICK = 0.05  # 50ms per frame (~20 FPS)

# Aircraft limits (F-15E approximate)
MIN_SPEED = 150      # knots - below this = stall
MAX_SPEED = 950      # knots - max level flight speed
CRUISE_SPEED = 450   # knots - cruise altitude speed
STALL_SPEED = 140    # knots - hard stall threshold

MIN_ALTITUDE = 0     # feet
MAX_ALTITUDE = 50000 # feet
CLIMB_CEILING = 45000  # service ceiling

# Physics parameters
DRAG_COEFFICIENT = 0.08
THRUST_RESPONSE = 2.0  # acceleration/deceleration rate
G_LIMIT = 8.0  # max G forces before damage
TURN_RADIUS_FACTOR = 0.015

# ============================================================================
# ENUMS
# ============================================================================

class FlightPhase(Enum):
    MAIN_MENU = 0
    MISSION_BRIEFING = 1
    IN_FLIGHT = 2
    RADAR_SCREEN = 3
    LANDING = 4
    MISSION_RESULTS = 5
    GAME_OVER = 6

class WeaponType(Enum):
    CANNON = 0
    AIM9_SIDEWINDER = 1    # Heat seeker
    AIM7_SPARROW = 2       # Radar missile
    AGM65_MAVERICK = 3     # Air-to-ground
    MK82_BOMB = 4          # Dumb bomb

class ContactType(Enum):
    FRIENDLY_FIGHTER = 0
    ENEMY_FIGHTER = 1
    SAM_SITE = 2
    GROUND_TARGET = 3
    WAYPOINT = 4

# ============================================================================
# DATA STRUCTURES
# ============================================================================

@dataclass
class Vector3:
    """3D vector for position/velocity"""
    x: float
    y: float  # altitude
    z: float
    
    def magnitude(self) -> float:
        return math.sqrt(self.x**2 + self.y**2 + self.z**2)
    
    def normalize(self) -> 'Vector3':
        mag = self.magnitude()
        if mag < 0.001:
            return Vector3(0, 0, 0)
        return Vector3(self.x/mag, self.y/mag, self.z/mag)
    
    def dot(self, other: 'Vector3') -> float:
        return self.x*other.x + self.y*other.y + self.z*other.z
    
    def __add__(self, other: 'Vector3') -> 'Vector3':
        return Vector3(self.x+other.x, self.y+other.y, self.z+other.z)
    
    def __sub__(self, other: 'Vector3') -> 'Vector3':
        return Vector3(self.x-other.x, self.y-other.y, self.z-other.z)
    
    def __mul__(self, scalar: float) -> 'Vector3':
        return Vector3(self.x*scalar, self.y*scalar, self.z*scalar)

@dataclass
class Aircraft:
    """Player aircraft state"""
    position: Vector3
    velocity: Vector3
    pitch: float      # -90 to +90 degrees
    roll: float       # -180 to +180 degrees
    heading: float    # 0-360 degrees (compass)
    throttle: float   # 0.0 to 1.0
    
    # Flight state
    g_force: float = 1.0
    on_ground: bool = False
    damaged: bool = False
    damage_level: float = 0.0  # 0.0 = healthy, 1.0 = destroyed
    
    # Weapons
    cannon_ammo: int = 500
    missiles_heat: int = 4      # AIM-9
    missiles_radar: int = 4     # AIM-7
    missiles_agm: int = 8       # Mavericks
    bombs: int = 12             # Mk82
    
    # Instrument state
    radar_range: int = 60       # nm
    radar_mode: str = "AIR"     # AIR or GND
    autopilot: bool = False
    
    def current_speed(self) -> float:
        """Speed in knots"""
        return self.velocity.magnitude() * 1.944  # m/s to knots
    
    def current_altitude(self) -> float:
        """Altitude in feet"""
        return self.position.y * 3.28084  # meters to feet
    
    def is_stalled(self) -> bool:
        return self.current_speed() < STALL_SPEED

@dataclass
class Contact:
    """Radar/visual contact"""
    contact_type: ContactType
    position: Vector3
    velocity: Vector3
    heading: float
    altitude: float
    threat_level: int  # 0-9
    locked: bool = False
    
    def distance_to(self, other_pos: Vector3) -> float:
        delta = self.position - other_pos
        return delta.magnitude()

@dataclass
class Missile:
    """In-flight missile"""
    weapon_type: WeaponType
    position: Vector3
    velocity: Vector3
    target: Optional[Contact]
    time_to_impact: float
    active: bool = True

@dataclass
class Terrain:
    """Terrain elevation data (simplified grid)"""
    def get_elevation(self, x: float, z: float) -> float:
        """Return terrain elevation in meters at given position"""
        # Simplified terrain: some mountains, valleys
        mountain_1 = 200 * math.exp(-((x-5000)**2 + (z-5000)**2) / 5000000)
        mountain_2 = 150 * math.exp(-((x-15000)**2 + (z-10000)**2) / 8000000)
        return mountain_1 + mountain_2

# ============================================================================
# GAME STATE
# ============================================================================

class GameState:
    def __init__(self):
        self.phase = FlightPhase.MAIN_MENU
        self.aircraft = Aircraft(
            position=Vector3(0, 2000, 0),  # 2000m = ~6500 feet
            velocity=Vector3(100, 0, 0),   # 100 m/s forward
            pitch=0,
            roll=0,
            heading=0,
            throttle=0.6
        )
        self.terrain = Terrain()
        self.contacts: List[Contact] = []
        self.missiles: List[Missile] = []
        self.time_in_flight = 0.0
        self.mission_score = 0
        self.radar_contacts_destroyed = 0
        self.selected_contact_idx = -1
        
        # Mission data
        self.current_mission = 0
        self.missions = [
            {"name": "GULF PATROL", "target": "Locate and engage enemy fighters"},
            {"name": "COMBAT CAP", "target": "Defend airspace from incoming raid"},
            {"name": "STRIKE MISSION", "target": "Destroy SAM sites and ground targets"},
        ]
        
        # Generate initial contacts
        self._spawn_initial_contacts()
    
    def _spawn_initial_contacts(self):
        """Create initial air/ground contacts for the mission"""
        # Friendly wingman
        self.contacts.append(Contact(
            contact_type=ContactType.FRIENDLY_FIGHTER,
            position=Vector3(1000, 2000, -1000),
            velocity=Vector3(100, 0, 0),
            heading=0,
            altitude=6500,
            threat_level=0
        ))
        
        # Enemy fighters (4-6 bandits)
        for i in range(random.randint(4, 6)):
            x = random.uniform(10000, 30000)
            z = random.uniform(-5000, 15000)
            self.contacts.append(Contact(
                contact_type=ContactType.ENEMY_FIGHTER,
                position=Vector3(x, 2500, z),
                velocity=Vector3(random.uniform(-80, -120), 0, random.uniform(-50, 50)),
                heading=random.uniform(0, 360),
                altitude=8000 + random.uniform(-1000, 1000),
                threat_level=random.randint(6, 9)
            ))
        
        # SAM sites (stationary ground threats)
        for i in range(3):
            x = random.uniform(15000, 40000)
            z = random.uniform(5000, 20000)
            self.contacts.append(Contact(
                contact_type=ContactType.SAM_SITE,
                position=Vector3(x, 0, z),
                velocity=Vector3(0, 0, 0),
                heading=0,
                altitude=0,
                threat_level=random.randint(5, 8)
            ))
        
        # Ground targets
        for i in range(2):
            x = random.uniform(20000, 45000)
            z = random.uniform(10000, 25000)
            self.contacts.append(Contact(
                contact_type=ContactType.GROUND_TARGET,
                position=Vector3(x, 0, z),
                velocity=Vector3(0, 0, 0),
                heading=0,
                altitude=0,
                threat_level=random.randint(3, 5)
            ))

# ============================================================================
# PHYSICS ENGINE
# ============================================================================

class PhysicsEngine:
    @staticmethod
    def update_aircraft(aircraft: Aircraft, terrain: Terrain, dt: float):
        """Update aircraft position and velocity based on flight model"""
        
        # Throttle to thrust conversion
        current_speed = aircraft.current_speed()
        max_thrust = 250  # m/s
        min_thrust = 50
        
        target_speed = min_thrust + aircraft.throttle * (max_thrust - min_thrust)
        current_velocity_mag = aircraft.velocity.magnitude()
        
        # Thrust acceleration (with drag)
        if current_velocity_mag < target_speed:
            accel = THRUST_RESPONSE
        else:
            accel = -DRAG_COEFFICIENT * (current_velocity_mag - target_speed)
        
        # Convert pitch/roll to acceleration
        pitch_rad = math.radians(aircraft.pitch)
        
        # Vertical component (climb/descent)
        vertical_accel = math.sin(pitch_rad) * 9.81
        
        # Horizontal banking (turn)
        roll_rad = math.radians(aircraft.roll)
        turn_rate = math.sin(roll_rad) * TURN_RADIUS_FACTOR * current_velocity_mag
        
        # Update heading based on turn
        aircraft.heading += turn_rate * dt
        aircraft.heading %= 360
        
        # Update position
        heading_rad = math.radians(aircraft.heading)
        forward_x = math.cos(heading_rad) * accel * dt
        forward_z = math.sin(heading_rad) * accel * dt
        vertical_y = vertical_accel * dt
        
        aircraft.position.x += forward_x
        aircraft.position.z += forward_z
        aircraft.position.y += vertical_y
        
        # Update velocity
        aircraft.velocity.x = math.cos(heading_rad) * target_speed
        aircraft.velocity.z = math.sin(heading_rad) * target_speed
        aircraft.velocity.y = math.sin(pitch_rad) * target_speed * 0.5
        
        # Terrain collision
        terrain_elevation = terrain.get_elevation(aircraft.position.x, aircraft.position.z)
        if aircraft.position.y < terrain_elevation + 100:  # 100m safety
            aircraft.position.y = terrain_elevation + 100
            aircraft.on_ground = True
        else:
            aircraft.on_ground = False
        
        # Altitude ceiling
        if aircraft.position.y > CLIMB_CEILING:
            aircraft.position.y = CLIMB_CEILING
            aircraft.pitch = min(aircraft.pitch, 0)  # Can only dive
        
        # Stall warning/behavior
        if aircraft.is_stalled():
            # Stall drops nose
            aircraft.pitch = max(aircraft.pitch - 5, -45)
        
        # G-force calculation (simplified)
        accel_total = math.sqrt((accel**2) + (vertical_accel**2) + (turn_rate**2))
        aircraft.g_force = max(1.0, 1.0 + accel_total / 9.81)
        
        # Damage accumulation from excessive G
        if aircraft.g_force > G_LIMIT:
            aircraft.damage_level += 0.01 * (aircraft.g_force - G_LIMIT)
            aircraft.damaged = True
        
        # Overspeed damage
        if current_speed > MAX_SPEED * 1.1:
            aircraft.damage_level += 0.005
            aircraft.damaged = True
    
    @staticmethod
    def update_contacts(contacts: List[Contact], dt: float):
        """Update enemy AI and contact positions"""
        for contact in contacts:
            if contact.contact_type == ContactType.ENEMY_FIGHTER:
                # Simple AI: wander and occasionally turn
                if random.random() < 0.02:  # 2% chance per frame
                    contact.heading += random.uniform(-10, 10)
                
                # Update position
                heading_rad = math.radians(contact.heading)
                speed = contact.velocity.magnitude()
                contact.position.x += math.cos(heading_rad) * speed * dt
                contact.position.z += math.sin(heading_rad) * speed * dt
                contact.position.y += (random.random() - 0.5) * 50 * dt

# ============================================================================
# RENDERING ENGINE
# ============================================================================

def clear_screen():
    """Clear terminal screen (Windows-compatible)"""
    os.system('cls' if os.name == 'nt' else 'clear')

def render_screen(state: GameState):
    """Render current game view to terminal"""
    clear_screen()
    
    if state.phase == FlightPhase.MAIN_MENU:
        render_main_menu(state)
    elif state.phase == FlightPhase.MISSION_BRIEFING:
        render_mission_briefing(state)
    elif state.phase == FlightPhase.IN_FLIGHT:
        render_in_flight(state)
    elif state.phase == FlightPhase.RADAR_SCREEN:
        render_radar_screen(state)
    elif state.phase == FlightPhase.MISSION_RESULTS:
        render_mission_results(state)

def render_main_menu(state: GameState):
    """Main menu screen"""
    print("╔" + "═"*78 + "╗")
    print("║" + " "*78 + "║")
    print("║" + "F-15 STRIKE EAGLE SIMULATOR v1.0".center(78) + "║")
    print("║" + "Inspired by MicroProse 1989".center(78) + "║")
    print("║" + " "*78 + "║")
    print("╠" + "═"*78 + "╣")
    print("║" + " "*78 + "║")
    print("║  [S] START NEW GAME                                                          ║")
    print("║  [R] RESUME FLIGHT                                                           ║")
    print("║  [Q] QUIT                                                                    ║")
    print("║" + " "*78 + "║")
    print("╚" + "═"*78 + "╝")

def render_mission_briefing(state: GameState):
    """Mission briefing screen"""
    mission = state.missions[state.current_mission % len(state.missions)]
    print("╔" + "═"*78 + "╗")
    print("║" + "MISSION BRIEFING".center(78) + "║")
    print("╠" + "═"*78 + "╣")
    print("║" + " "*78 + "║")
    print(f"║ MISSION: {mission['name']:<63}║")
    print(f"║ OBJECTIVE: {mission['target']:<59}║")
    print("║" + " "*78 + "║")
    print("║  TACTICAL SITUATION:                                                          ║")
    print("║  - Enemy fighters detected in patrol sector                                   ║")
    print("║  - Multiple SAM sites active                                                  ║")
    print("║  - Wingman will provide support                                               ║")
    print("║" + " "*78 + "║")
    print("║  LOADOUT:                                                                     ║")
    print("║  - M61 Vulcan Cannon (500 rounds)                                             ║")
    print("║  - 4x AIM-9 Sidewinder (heat-seeking)                                         ║")
    print("║  - 4x AIM-7 Sparrow (radar-guided)                                            ║")
    print("║  - 8x AGM-65 Maverick (air-to-ground)                                         ║")
    print("║  - 12x Mk82 bombs (500 lb)                                                    ║")
    print("║" + " "*78 + "║")
    print("║ [SPACE] TO BEGIN FLIGHT                                                       ║")
    print("║" + " "*78 + "║")
    print("╚" + "═"*78 + "╝")

def render_in_flight(state: GameState):
    """Main flight view with HUD"""
    aircraft = state.aircraft
    
    # Top status bar
    print("┌" + "─"*78 + "┐")
    speed = aircraft.current_speed()
    alt = aircraft.current_altitude()
    print(f"│ SPD:{speed:3.0f}kt  ALT:{alt:5.0f}ft  HDG:{aircraft.heading:03.0f}°  PITCH:{aircraft.pitch:+3.0f}°  G:{aircraft.g_force:.1f}  │")
    print("├" + "─"*78 + "┤")
    
    # Artificial horizon
    pitch_offset = int(aircraft.pitch / 10)
    for i in range(8):
        line_y = 4 + i - pitch_offset
        if i == 4:
            # Center horizon line
            print("│ " + " "*30 + "════════════" + " "*30 + " │")
        else:
            print("│ " + " "*78 + " │")
    
    print("├" + "─"*78 + "┤")
    
    # Radar window (simplified)
    print("│ RADAR: ", end="")
    render_radar_mini(state)
    print(" │")
    
    print("├" + "─"*78 + "┤")
    
    # Weapons status
    print(f"│ CANNON:{aircraft.cannon_ammo:3d}  MSL-H:{aircraft.missiles_heat}  MSL-R:{aircraft.missiles_radar}  AGM:{aircraft.missiles_agm:2d}  BOMB:{aircraft.bombs:2d}  │")
    
    # Threat indicators
    print("│ THREATS: ", end="")
    threat_count = len([c for c in state.contacts if c.contact_type == ContactType.ENEMY_FIGHTER])
    print(f"{threat_count} BANDITS  ", end="")
    sam_count = len([c for c in state.contacts if c.contact_type == ContactType.SAM_SITE])
    print(f"{sam_count} SAM SITES" + " "*40 + "│")
    
    print("├" + "─"*78 + "┤")
    print("│ [↑↓] PITCH  [←→] ROLL  [W/S] THROTTLE  [R] RADAR  [F] FIRE  [L] LOCK-ON  │")
    print("│ [SPACE] BRAKE  [P] PAUSE  [Q] EJECT                                          │")
    print("└" + "─"*78 + "┘")

def render_radar_mini(state: GameState):
    """Mini radar display (8x8 chars)"""
    radar_str = "     "
    for contact in state.contacts[:3]:  # Show first 3
        if contact.contact_type == ContactType.ENEMY_FIGHTER:
            radar_str += "●"
        elif contact.contact_type == ContactType.SAM_SITE:
            radar_str += "▲"
        elif contact.contact_type == ContactType.FRIENDLY_FIGHTER:
            radar_str += "◆"
    print(radar_str[:23], end="")

def render_radar_screen(state: GameState):
    """Full radar display"""
    print("╔" + "═"*78 + "╗")
    print("║" + "TACTICAL RADAR DISPLAY".center(78) + "║")
    print("╠" + "═"*78 + "╣")
    
    # Radar sweep display
    for i in range(12):
        print("║" + " "*78 + "║")
    
    print("║ CONTACTS:" + " "*68 + "║")
    
    for idx, contact in enumerate(state.contacts[:8]):
        contact_char = {
            ContactType.FRIENDLY_FIGHTER: "◆",
            ContactType.ENEMY_FIGHTER: "●",
            ContactType.SAM_SITE: "▲",
            ContactType.GROUND_TARGET: "■",
        }.get(contact.contact_type, "?")
        
        distance = contact.distance_to(state.aircraft.position) * 0.000539957  # meters to nm
        print(f"║ {contact_char} {contact_char} {distance:6.1f}nm  ALT {contact.altitude:5.0f}ft  TGT:{contact.threat_level}  │")
    
    print("║" + " "*78 + "║")
    print("║ [R] RETURN TO FLIGHT VIEW  [L] LOCK TARGET  [F] FIRE  [Q] QUIT           │")
    print("╚" + "═"*78 + "╝")

def render_mission_results(state: GameState):
    """Post-mission debrief"""
    print("╔" + "═"*78 + "╗")
    print("║" + "MISSION DEBRIEF".center(78) + "║")
    print("╠" + "═"*78 + "╣")
    print("║" + " "*78 + "║")
    print(f"║ TIME IN FLIGHT: {state.time_in_flight:6.1f} seconds".ljust(79) + "║")
    print(f"║ CONTACTS DESTROYED: {state.radar_contacts_destroyed}".ljust(79) + "║")
    print(f"║ MISSION SCORE: {state.mission_score}".ljust(79) + "║")
    print("║" + " "*78 + "║")
    print("║ [SPACE] NEXT MISSION  [Q] RETURN TO MAIN MENU                                ║")
    print("║" + " "*78 + "║")
    print("╚" + "═"*78 + "╝")

# ============================================================================
# INPUT HANDLING
# ============================================================================

def handle_input(state: GameState) -> bool:
    """Handle keyboard input, return True if game should continue"""
    try:
        if WINDOWS:
            if msvcrt.kbhit():
                key = msvcrt.getch().decode('utf-8', errors='ignore').upper()
            else:
                key = None
        else:
            # Non-blocking input for Unix/Linux
            import select
            if select.select([sys.stdin], [], [], 0)[0]:
                key = sys.stdin.read(1).upper()
            else:
                key = None
        
        if not key:
            return True
            
        if state.phase == FlightPhase.MAIN_MENU:
            if key == 'S':
                state.phase = FlightPhase.MISSION_BRIEFING
            elif key == 'Q':
                return False
        
        elif state.phase == FlightPhase.MISSION_BRIEFING:
            if key == ' ':
                state.phase = FlightPhase.IN_FLIGHT
                state.time_in_flight = 0.0
        
        elif state.phase == FlightPhase.IN_FLIGHT:
            aircraft = state.aircraft
            
            if key == 'Q':
                state.phase = FlightPhase.MISSION_RESULTS
            elif key == 'P':
                state.phase = FlightPhase.MISSION_RESULTS
            elif key == 'R':
                state.phase = FlightPhase.RADAR_SCREEN
            elif key == 'W':  # Pitch up or increase throttle
                aircraft.pitch = min(aircraft.pitch + 2, 45)
            elif key == 'S':  # Pitch down or decrease throttle
                aircraft.pitch = max(aircraft.pitch - 2, -45)
            elif key == 'A':  # Roll left
                aircraft.roll = max(aircraft.roll - 3, -90)
            elif key == 'D':  # Roll right
                aircraft.roll = min(aircraft.roll + 3, 90)
            elif key == 'F':  # Fire cannon
                if aircraft.cannon_ammo > 0:
                    aircraft.cannon_ammo -= 10
                    state.mission_score += random.randint(1, 10)
            elif key == 'L':  # Lock target
                if state.contacts:
                    state.selected_contact_idx = (state.selected_contact_idx + 1) % len(state.contacts)
                    if state.selected_contact_idx < len(state.contacts):
                        state.contacts[state.selected_contact_idx].locked = True
        
        elif state.phase == FlightPhase.RADAR_SCREEN:
            if key == 'R':
                state.phase = FlightPhase.IN_FLIGHT
            elif key == 'Q':
                return False
        
        elif state.phase == FlightPhase.MISSION_RESULTS:
            if key == ' ':
                state.current_mission += 1
                state.phase = FlightPhase.MISSION_BRIEFING
                state._spawn_initial_contacts()
            elif key == 'Q':
                state.phase = FlightPhase.MAIN_MENU
    
    except Exception as e:
        pass
    
    return True

# ============================================================================
# MAIN GAME LOOP
# ============================================================================

def main():
    """Main game loop"""
    state = GameState()
    physics = PhysicsEngine()
    
    print("\n" + "="*80)
    print("F-15 STRIKE EAGLE SIMULATOR - Press any key to begin...")
    print("="*80)
    time.sleep(1)
    
    last_frame_time = time.time()
    
    try:
        while True:
            # Frame timing
            current_time = time.time()
            dt = min(GAME_TICK, current_time - last_frame_time)
            last_frame_time = current_time
            
            # Input
            if not handle_input(state):
                break
            
            # Physics update
            if state.phase == FlightPhase.IN_FLIGHT:
                physics.update_aircraft(state.aircraft, state.terrain, dt)
                physics.update_contacts(state.contacts, dt)
                state.time_in_flight += dt
            
            # Render
            render_screen(state)
            
            # Frame delay
            time.sleep(GAME_TICK)
    
    except KeyboardInterrupt:
        print("\n\nGame interrupted by user.")
    
    finally:
        clear_screen()
        print("Thank you for flying F-15 STRIKE EAGLE!")

if __name__ == "__main__":
    main()
