## [v1.1]
**DTG:** 130055Z APR 26

### MISSION SUMMARY
Control and timing cleanup pass for the v1.0 baseline.

### ADDED
- Added true `PAUSED` flight phase separate from mission debrief.
- Added dedicated pause screen with resume and end-mission options.
- Added debounced key handling using `keyWasPressed[256]` to prevent toggle and menu key flutter.
- Added continuous held-flight controls for pitch and roll through `handleContinuousFlightControls(...)`.
- Added automatic roll re-centering when lateral input is released.
- Added locked-target status display on the tactical radar screen.

### CHANGED
- Changed `time_in_flight` from `int` to `float` for accurate elapsed-time tracking.
- Changed mission time accumulation from frame-based incrementing to real delta-time accumulation using `dt`.
- Changed `[P]` behavior so pause now enters a true paused state instead of jumping to mission results.
- Changed target lock handling so previous locks are cleared before a new contact is designated.
- Changed next-mission flow to reset aircraft state cleanly before the next sortie.
- Changed mission debrief formatting to display fractional seconds for time in flight.

### FIXED
- Fixed mission timer inflation caused by counting rendered frames as whole seconds.
- Fixed repeated key-trigger behavior caused by raw `GetAsyncKeyState()` polling without debounce.
- Fixed persistent multi-target lock state by ensuring only one contact is marked as locked at a time.
- Fixed state carry-over between missions by resetting aircraft state and selected target index.

### STATUS
- v1.1 complete.
- v1.2 scheduled for flight model and physics cleanup.
