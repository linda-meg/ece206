// ============================================================================
// home_processing.cpp  --  Function DEFINITIONS (how each operation works)
// ============================================================================

#include <iostream>
#include <iomanip>
#include "home_processing.h"

using namespace std;

// ---------------------------------------------------------------------------
// Simulated sensor data (stands in for real hardware)
// ---------------------------------------------------------------------------

// Motion detected (1) or not (0) in each room at each time step.
// A 2D array: [room][time step]
static const int MOTION_PATTERN[NUM_ROOMS][NUM_STEPS] = {
    { 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 },   // Living room: family home, then leaves
    { 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0 },   // Kitchen: someone is cooking
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1 },   // Bedroom: someone goes to rest
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }    // Garage: empty
};

// Kitchen smoke level (ppm) over time: the dinner gets burned at steps 7-9.
static const double SMOKE_LOG[NUM_STEPS] = {
    20, 20, 22, 21, 25, 30, 60, 150, 350, 120, 40, 25
};

// ---------------------------------------------------------------------------
// Building the house
// ---------------------------------------------------------------------------

void addRoom(SmartHome& home, const char* name, char symbol,
             Point topLeft, int width, int height) {
    if (home.numRooms >= NUM_ROOMS) return;      // array is full
    Room& r = home.rooms[home.numRooms];
    r.name       = name;
    r.symbol     = symbol;
    r.topLeft    = topLeft;
    r.width      = width;
    r.height     = height;
    r.numSensors = 0;
    home.numRooms++;
}

// Is point p inside this room's rectangle?
bool isInsideRoom(const Room& room, Point p) {
    return p.x >= room.topLeft.x && p.x < room.topLeft.x + room.width &&
           p.y >= room.topLeft.y && p.y < room.topLeft.y + room.height;
}

// Install a sensor. Rejects it if the room is full or the location
// is outside the room's walls (a coordinate bounds check).
bool addSensor(Room& room, int id, SensorType type, Point location) {
    if (room.numSensors >= MAX_SENSORS)  return false;
    if (!isInsideRoom(room, location))   return false;

    Sensor& s = room.sensors[room.numSensors];
    s.id       = id;
    s.type     = type;
    s.location = location;
    s.head     = 0;
    s.count    = 0;
    s.raw      = 0.0;
    s.filtered = 0.0;
    s.status   = ALERT_OK;
    for (int i = 0; i < WINDOW; i++) s.history[i] = 0.0;

    room.numSensors++;
    return true;
}

// Floor plan (16 x 8):
//   +--------+--------+
//   | Living | Kitchen|   rows 0-3
//   +--------+--------+
//   | Bedroom| Garage |   rows 4-7
//   +--------+--------+
void setupHome(SmartHome& home) {
    home.numRooms = 0;
    home.setpoint = 72.0;

    addRoom(home, "Living Room", 'l', {0, 0}, 8, 4);
    addRoom(home, "Kitchen",     'k', {8, 0}, 8, 4);
    addRoom(home, "Bedroom",     'b', {0, 4}, 8, 4);
    addRoom(home, "Garage",      'g', {8, 4}, 8, 4);

    Room* r = home.rooms;   // shorter name for the array
    addSensor(r[LIVING_ROOM], 101, TEMPERATURE, {1, 1});
    addSensor(r[LIVING_ROOM], 102, HUMIDITY,    {3, 1});
    addSensor(r[LIVING_ROOM], 103, LIGHT,       {2, 2});
    addSensor(r[LIVING_ROOM], 104, MOTION,      {5, 2});

    addSensor(r[KITCHEN],     201, TEMPERATURE, {9, 1});
    addSensor(r[KITCHEN],     202, HUMIDITY,    {11, 1});
    addSensor(r[KITCHEN],     203, SMOKE,       {13, 1});
    addSensor(r[KITCHEN],     204, MOTION,      {12, 2});

    addSensor(r[BEDROOM],     301, TEMPERATURE, {2, 5});
    addSensor(r[BEDROOM],     302, MOTION,      {5, 6});

    addSensor(r[GARAGE],      401, TEMPERATURE, {10, 5});
    addSensor(r[GARAGE],      402, HUMIDITY,    {13, 6});
}

// ---------------------------------------------------------------------------
// Reading and filtering
// ---------------------------------------------------------------------------

// SIMULATED HARDWARE READ.
// On a real board (Arduino / mBot) this would be something like
// analogRead(pin) followed by a conversion to degrees, %, or ppm.
double readSensor(const Sensor& s, int roomIndex, int step) {
    // Small repeatable "noise" between -2 and +2, so values wiggle a bit.
    double noise   = ((step * 7 + s.id * 3) % 5) - 2;
    bool   cooking = (roomIndex == KITCHEN && step >= 4 && step <= 9);

    switch (s.type) {
        case TEMPERATURE: {
            const double base[NUM_ROOMS] = { 72.0, 74.0, 68.0, 58.0 };
            double value = base[roomIndex] + noise * 0.5;
            if (cooking) value += (step - 3) * 3.0;      // the stove heats up
            return value;
        }
        case HUMIDITY: {
            const double base[NUM_ROOMS] = { 45.0, 50.0, 40.0, 55.0 };
            double value = base[roomIndex] + noise;
            if (cooking)             value += 12.0;      // steam from pots
            if (roomIndex == GARAGE) value += step * 1.5; // slow moisture build-up
            return value;
        }
        case MOTION:
            return MOTION_PATTERN[roomIndex][step];
        case SMOKE:
            return SMOKE_LOG[step];
        case LIGHT:
            // lights are on when someone is in the room
            return MOTION_PATTERN[roomIndex][step] ? 300.0 + noise * 10 : 40.0 + noise;
        default:
            return 0.0;
    }
}

// RING BUFFER: write the new value at 'head', then move head forward.
// When head reaches the end it wraps back to 0, overwriting the oldest value.
//
//   WINDOW = 4     [ 71 | 72 | 70 | 73 ]   head -> index 2
//   store(75)      [ 71 | 72 | 75 | 73 ]   head -> index 3
void storeReading(Sensor& s, double value) {
    s.history[s.head] = value;
    s.head = (s.head + 1) % WINDOW;
    if (s.count < WINDOW) s.count++;
}

// Average of the readings stored so far (smooths out noise).
double movingAverage(const Sensor& s) {
    if (s.count == 0) return 0.0;
    double sum = 0.0;
    for (int i = 0; i < s.count; i++) {
        sum += s.history[i];
    }
    return sum / s.count;
}

// ---------------------------------------------------------------------------
// Decisions
// ---------------------------------------------------------------------------

// Look up this sensor type's rule and compare the value against it.
AlertLevel checkAlert(SensorType type, double value) {
    const AlertRule& rule = ALERT_RULES[type];
    if (!rule.enabled)                                    return ALERT_OK;
    if (value >= rule.alarmHigh)                          return ALERT_ALARM;
    if (value > rule.warnHigh || value < rule.warnLow)    return ALERT_WARNING;
    return ALERT_OK;
}

// One full update for one sensor: read -> store -> filter -> check.
// Returns true only when the alert level CHANGES, so the log isn't flooded
// with the same message every 5 minutes (state-change detection).
bool updateSensor(Sensor& s, int roomIndex, int step) {
    s.raw = readSensor(s, roomIndex, step);
    storeReading(s, s.raw);
    s.filtered = movingAverage(s);

    double valueToCheck = ALERT_RULES[s.type].useFilter ? s.filtered : s.raw;
    AlertLevel newStatus = checkAlert(s.type, valueToCheck);

    bool changed = (newStatus != s.status);
    s.status = newStatus;
    return changed;
}

// Thermostat logic with a "deadband" so the system doesn't flip on and off.
// Empty rooms get a wider band (eco mode) to save energy.
HvacMode decideHvac(double temp, double setpoint, bool occupied) {
    double band = occupied ? 3.0 : 6.0;
    if (temp > setpoint + band) return HVAC_COOL;
    if (temp < setpoint - band) return HVAC_HEAT;
    return HVAC_OFF;
}

// Walk every sensor in the room and combine the results.
RoomSummary summarizeRoom(const Room& room, double setpoint) {
    RoomSummary sum = { false, false, 0.0, 0.0, false, ALERT_OK, HVAC_OFF };
    double tempTotal = 0.0, humTotal = 0.0;
    int    tempCount = 0,   humCount = 0;

    for (int i = 0; i < room.numSensors; i++) {
        const Sensor& s = room.sensors[i];

        if (s.type == TEMPERATURE) { tempTotal += s.filtered; tempCount++; }
        if (s.type == HUMIDITY)    { humTotal  += s.filtered; humCount++;  }

        if (s.type == MOTION) {          // occupied if ANY recent reading saw motion
            for (int k = 0; k < s.count; k++) {
                if (s.history[k] > 0) sum.occupied = true;
            }
        }
        if (s.status > sum.worst) sum.worst = s.status;
    }

    if (tempCount > 0) { sum.hasTemp = true;     sum.avgTemp     = tempTotal / tempCount; }
    if (humCount  > 0) { sum.hasHumidity = true; sum.avgHumidity = humTotal  / humCount;  }
    if (sum.hasTemp)   sum.hvac = decideHvac(sum.avgTemp, setpoint, sum.occupied);
    return sum;
}

// ---------------------------------------------------------------------------
// Display
// ---------------------------------------------------------------------------

// Print the time of a step as HH:MM (e.g. step 3 -> 17:15).
void printClock(int step) {
    int minutes = START_MINUTE + step * MINUTES_PER_STEP;
    cout << setfill('0') << setw(2) << minutes / 60 << ":"
         << setw(2) << minutes % 60 << setfill(' ');
}

// Draw the house as a 2D character grid using everyone's coordinates.
//   lowercase letter = room floor,  UPPERCASE = sensor,  ! = sensor in alert
void printFloorPlan(const SmartHome& home) {
    char grid[PLAN_H][PLAN_W];

    // 1) start with empty space
    for (int y = 0; y < PLAN_H; y++)
        for (int x = 0; x < PLAN_W; x++)
            grid[y][x] = ' ';

    // 2) paint each room's floor
    for (int r = 0; r < home.numRooms; r++) {
        const Room& room = home.rooms[r];
        for (int y = 0; y < PLAN_H; y++) {
            for (int x = 0; x < PLAN_W; x++) {
                Point p = { x, y };
                if (isInsideRoom(room, p)) grid[y][x] = room.symbol;
            }
        }
    }

    // 3) place every sensor at its coordinates
    for (int r = 0; r < home.numRooms; r++) {
        const Room& room = home.rooms[r];
        for (int i = 0; i < room.numSensors; i++) {
            const Sensor& s = room.sensors[i];
            grid[s.location.y][s.location.x] =
                (s.status == ALERT_OK) ? SENSOR_SYMBOL[s.type] : '!';
        }
    }

    // 4) print the grid
    for (int y = 0; y < PLAN_H; y++) {
        cout << "    ";
        for (int x = 0; x < PLAN_W; x++) cout << grid[y][x] << ' ';
        cout << "\n";
    }
}
