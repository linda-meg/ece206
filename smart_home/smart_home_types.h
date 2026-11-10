// ============================================================================
// smart_home_types.h  --  Custom data types for the ECE 206 Smart Home Project
// Author: Prof. Linda Megerdichian
// ----------------------------------------------------------------------------
// This header defines ONLY data: constants, enums, lookup tables, and structs.
// Every other file #includes it, so the whole project agrees on what a
// "sensor", "room", or "home" looks like.
//
// DATA HIERARCHY (top to bottom):
//
//   SmartHome                          <- LEVEL 1: the whole house
//     └── Room rooms[]                 <- LEVEL 2: one room on the floor plan
//           └── Sensor sensors[]       <- LEVEL 3: one device on the wall
//                 └── double history[] <- LEVEL 4: its most recent readings
//
//   Helper types:
//     Point        - an (x, y) location on the floor plan
//     AlertRule    - the limits for one kind of sensor
//     RoomSummary  - the results we compute for each room
// ============================================================================

#ifndef SMART_HOME_TYPES_H     // include guard: prevents double inclusion
#define SMART_HOME_TYPES_H

// ---- Global sizes -----------------------------------------------------------
const int NUM_ROOMS        = 4;    // rooms in the house
const int MAX_SENSORS      = 4;    // most sensors one room can hold
const int WINDOW           = 4;    // moving-average window (ring buffer size)
const int NUM_STEPS        = 12;   // how many times we poll the sensors
const int MINUTES_PER_STEP = 5;    // time between polls
const int START_MINUTE     = 17 * 60;   // simulation starts at 17:00 (5 PM)
const int PLAN_W           = 16;   // floor plan width  (x direction)
const int PLAN_H           = 8;    // floor plan height (y direction)

// ---- Room IDs (used as array indexes) ---------------------------------------
enum RoomId {
    LIVING_ROOM = 0,
    KITCHEN     = 1,
    BEDROOM     = 2,
    GARAGE      = 3
};

// ---- Sensor types and their lookup tables ----------------------------------
// The enum value is also the index into every table below, so
// SENSOR_NAME[SMOKE] is "Smoke" and SENSOR_UNIT[SMOKE] is "ppm".
enum SensorType {
    TEMPERATURE = 0,
    HUMIDITY,
    MOTION,
    SMOKE,
    LIGHT,
    NUM_SENSOR_TYPES          // = 5, handy for sizing arrays
};

const char        SENSOR_SYMBOL[NUM_SENSOR_TYPES] = { 'T', 'H', 'M', 'S', 'L' };
const char* const SENSOR_NAME[NUM_SENSOR_TYPES]   = { "Temperature", "Humidity",
                                                      "Motion", "Smoke", "Light" };
const char* const SENSOR_UNIT[NUM_SENSOR_TYPES]   = { "F", "%", "", "ppm", "lux" };

// ---- Alert levels -----------------------------------------------------------
enum AlertLevel {
    ALERT_OK = 0,
    ALERT_WARNING,
    ALERT_ALARM
};
const char* const ALERT_NAME[3] = { "OK", "WARNING", "ALARM" };

// ---- Heating / cooling modes ------------------------------------------------
enum HvacMode {
    HVAC_OFF = 0,
    HVAC_HEAT,
    HVAC_COOL
};
const char* const HVAC_NAME[3] = { "OFF", "HEAT", "COOL" };

// ---- Alert rules: one row per sensor type -----------------------------------
// useFilter = true  -> check the SMOOTHED value (ignores brief noise)
// useFilter = false -> check the RAW value (react instantly, e.g. smoke)
struct AlertRule {
    bool   enabled;
    bool   useFilter;
    double warnLow;     // below this  -> WARNING
    double warnHigh;    // above this  -> WARNING
    double alarmHigh;   // at or above -> ALARM
};

const AlertRule ALERT_RULES[NUM_SENSOR_TYPES] = {
    //  enabled useFilter warnLow warnHigh alarmHigh
    {   true,   true,     60.0,   85.0,    120.0 },   // TEMPERATURE (F)
    {   true,   true,     25.0,   65.0,     85.0 },   // HUMIDITY (%)
    {   false,  false,     0.0,    0.0,      0.0 },   // MOTION (no alerts)
    {   true,   false,    -1.0,  100.0,    300.0 },   // SMOKE (ppm)
    {   false,  false,     0.0,    0.0,      0.0 }    // LIGHT (no alerts)
};

// ---- Coordinates on the floor plan ------------------------------------------
// (0,0) is the TOP-LEFT corner. x goes right, y goes down.
// The floor plan grid is indexed  grid[y][x]  -> row first, then column.
struct Point {
    int x;
    int y;
};

// ---- LEVEL 3 / 4: a sensor and its reading history -------------------------
struct Sensor {
    int        id;
    SensorType type;
    Point      location;          // where it's mounted on the floor plan

    double     history[WINDOW];   // ring buffer of the last WINDOW readings
    int        head;              // index where the NEXT reading will go
    int        count;             // how many slots are filled (max WINDOW)

    double     raw;               // most recent reading
    double     filtered;          // moving average of history[]
    AlertLevel status;            // current alert level
};

// ---- LEVEL 2: a room --------------------------------------------------------
struct Room {
    const char* name;
    char        symbol;           // letter drawn on the floor plan
    Point       topLeft;          // room position on the floor plan
    int         width;
    int         height;
    Sensor      sensors[MAX_SENSORS];
    int         numSensors;
};

// ---- LEVEL 1: the whole house -----------------------------------------------
struct SmartHome {
    Room   rooms[NUM_ROOMS];
    int    numRooms;
    double setpoint;              // target thermostat temperature (F)
};

// ---- Results computed for each room -----------------------------------------
struct RoomSummary {
    bool       hasTemp;
    bool       hasHumidity;
    double     avgTemp;
    double     avgHumidity;
    bool       occupied;          // any motion in the recent window?
    AlertLevel worst;             // highest alert of any sensor in the room
    HvacMode   hvac;              // what the thermostat decides to do
};

#endif // SMART_HOME_TYPES_H
