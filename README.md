 Timer Utility (C++)
Features
Supports ONESHOT and PERIODIC timers.

Allows timer scheduling via:

Specific intervals (e.g., 1h 30m 10s)

Specific times (e.g., "14:30:00")

Runs timers on a separate thread.

Automatically invokes provided callback functions when timers expire.

Thread-safe with mutex and condition_variable for precise scheduling.

Technologies Used
C++11 and above

STL Threads, Mutex, Condition Variables
Chrono library for time manipulation
