## [v1.1] - 2026-04-13 00:55Z

### Added
- Added a real `PAUSED` flight phase separate from mission debrief.
- Added `renderPaused()` screen with resume and end-mission options.
- Added debounced key handling using `keyWasPressed[256]` to prevent key fluttering.
- Added continuous held-flight controls for pitch and roll via `handleContinuousFlightControls(...)`.
- Added roll auto-centering when lateral controls are released.
- Added locked-target status display on the radar screen.

### Changed
- Changed `time_in_flight` from `int` to `float` for proper elapsed-time tracking.
- Changed mission time accumulation from per-frame incrementing to real delta-time accumulation using `dt`.
- Changed pause behavior so `[P]` now enters a true paused state instead of jumping to mission results.
- Changed target lock behavior so previous locks are cleared before a new contact is locked.
- Changed next-mission reset flow to reinitialize the aircraft state cleanly.
- Changed mission debrief formatting to display fractional seconds for time in flight.

### Fixed
- Fixed incorrect mission timing where one rendered frame counted as one full second.
- Fixed repeated key-trigger behavior caused by raw `GetAsyncKeyState()` polling without debounce.
- Fixed persistent multi-target lock state by ensuring only one contact is marked as locked at a time.
- Fixed mission carry-over issues by resetting aircraft state and selected target index on next mission.

### Notes
- v1.1 is the control-and-timing cleanup pass for the v1.0 baseline.
- Physics model cleanup is scheduled for v1.2.
