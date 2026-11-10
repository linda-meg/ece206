// ============================================================================
// home_processing.h  --  Function DECLARATIONS (the "menu" of operations)
// ----------------------------------------------------------------------------
// This header says WHAT each function does and which types it uses.
// The code itself (HOW each one works) is in home_processing.cpp.
// ============================================================================

#ifndef HOME_PROCESSING_H
#define HOME_PROCESSING_H

#include "smart_home_types.h"

// ---- Building the house -----------------------------------------------------
void addRoom(SmartHome& home, const char* name, char symbol,
             Point topLeft, int width, int height);
bool addSensor(Room& room, int id, SensorType type, Point location);
void setupHome(SmartHome& home);

// ---- Coordinates ------------------------------------------------------------
bool isInsideRoom(const Room& room, Point p);

// ---- Reading and filtering --------------------------------------------------
double readSensor(const Sensor& s, int roomIndex, int step);   // simulated hardware
void   storeReading(Sensor& s, double value);                  // ring buffer
double movingAverage(const Sensor& s);

// ---- Decisions --------------------------------------------------------------
AlertLevel  checkAlert(SensorType type, double value);
bool        updateSensor(Sensor& s, int roomIndex, int step);   // true if alert changed
HvacMode    decideHvac(double temp, double setpoint, bool occupied);
RoomSummary summarizeRoom(const Room& room, double setpoint);

// ---- Display ----------------------------------------------------------------
void printClock(int step);
void printFloorPlan(const SmartHome& home);

#endif // HOME_PROCESSING_H
