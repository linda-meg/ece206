// ============================================================================
// main.cpp  --  TOP LEVEL: the smart home monitoring loop
// ----------------------------------------------------------------------------
// This file does no sensor math itself. It CALLS the lower levels in order,
// the way a real smart-home hub runs its polling loop:
//
//   1. Set up the house                   setupHome() -> addRoom(), addSensor()
//   2. Every 5 minutes, for every room, for every sensor:
//        read -> store -> filter -> check   updateSensor()
//        log any alert that CHANGED         (state-change detection)
//   3. If any sensor is in ALARM, notify the homeowner
//   4. At the end: summarize each room      summarizeRoom() -> decideHvac()
//   5. Draw the floor plan                  printFloorPlan()
//
// BUILD:   g++ -std=c++11 -Wall main.cpp home_processing.cpp -o smart_home
// RUN:     ./smart_home
// ============================================================================

#include <iostream>
#include <iomanip>
#include "smart_home_types.h"
#include "home_processing.h"

using namespace std;

int main() {
    // ---- Stage 1: build the house ------------------------------------------
    SmartHome home;
    setupHome(home);

    // Coordinate bounds check in action: (20, 20) is outside the garage walls.
    bool ok = addSensor(home.rooms[GARAGE], 499, LIGHT, {20, 20});
    cout << "Install test sensor at (20,20) in Garage: "
         << (ok ? "accepted" : "REJECTED (outside the room)") << "\n\n";

    cout << "Floor plan at start:\n";
    printFloorPlan(home);

    // ---- Stage 2 + 3: the polling loop -------------------------------------
    cout << "\nEVENT LOG (only changes are shown)\n";
    cout << "-----------------------------------------------------------\n";

    for (int step = 0; step < NUM_STEPS; step++) {          // LEVEL: time
        bool alarmThisStep = false;

        for (int r = 0; r < home.numRooms; r++) {           // LEVEL: room
            Room& room = home.rooms[r];

            for (int i = 0; i < room.numSensors; i++) {     // LEVEL: sensor
                Sensor& s = room.sensors[i];

                bool changed = updateSensor(s, r, step);
                if (s.status == ALERT_ALARM) alarmThisStep = true;

                if (changed) {
                    double shown = ALERT_RULES[s.type].useFilter ? s.filtered : s.raw;
                    cout << "[";
                    printClock(step);
                    cout << "] " << left << setw(12) << room.name
                         << setw(12) << SENSOR_NAME[s.type] << right
                         << fixed << setprecision(1) << setw(6) << shown << " "
                         << left << setw(4) << SENSOR_UNIT[s.type] << right
                         << "-> " << ALERT_NAME[s.status] << "\n";
                }
            }
        }

        if (alarmThisStep) {
            cout << "[";
            printClock(step);
            cout << "] *** ALARM: sending notification to homeowner's phone ***\n";
        }
    }

    // ---- Stage 4: room summaries -------------------------------------------
    cout << "\nROOM SUMMARY at ";
    printClock(NUM_STEPS - 1);
    cout << "  (thermostat set to " << home.setpoint << " F)\n";
    cout << "Room         | Temp F | Humid % | Occupied | Alert   | HVAC\n";
    cout << "-------------+--------+---------+----------+---------+-----\n";

    for (int r = 0; r < home.numRooms; r++) {
        RoomSummary sum = summarizeRoom(home.rooms[r], home.setpoint);

        cout << left << setw(12) << home.rooms[r].name << right << " | ";
        if (sum.hasTemp)     cout << setw(6) << sum.avgTemp;     else cout << "    --";
        cout << " | ";
        if (sum.hasHumidity) cout << setw(7) << sum.avgHumidity; else cout << "     --";
        cout << " | " << left << setw(8) << (sum.occupied ? "yes" : "no")
             << " | " << setw(7) << ALERT_NAME[sum.worst]
             << " | " << HVAC_NAME[sum.hvac] << right << "\n";
    }

    // ---- Stage 5: final floor plan -----------------------------------------
    cout << "\nFloor plan at end (UPPERCASE = sensor, ! = sensor in alert):\n";
    printFloorPlan(home);
    cout << "    l = living room   k = kitchen   b = bedroom   g = garage\n";
    return 0;
}
