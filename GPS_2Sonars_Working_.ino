// ---------------------------------------------------------------------------
// Project File 
// Tom & Afek
// One Bin System 
//  
// ---------------------------------------------------------------------------

//***** INCLUDE *****//
#include <NewPing.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
//-------- END INCLUDE --------------//



//*********GPS SETUP ***********//
  // Choose two Arduino pins to use for software serial
  // The GPS Shield uses D2 and D3 by default when in DLINE mode
  int RXPin = 2;
  int TXPin = 3;
  // The Skytaq EM-506 GPS module included in the GPS Shield Kit
  // uses 4800 baud by default
  int GPSBaud = 4800;
  // Create a TinyGPS++ object called "gps"
  TinyGPSPlus gps;
  // Create a software serial port called "gpsSerial"
  SoftwareSerial gpsSerial(RXPin, TXPin);
//------------- GPS SETUP END -------------------------//



// **********SONAR SETUP ******************//
  #define SONAR_NUM     2 // Number of sensors.
  #define MAX_DISTANCE 15 // Maximum distance (in cm) to ping.
  #define PING_INTERVAL 33 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).
  
  unsigned long pingTimer[SONAR_NUM]; // Holds the times when the next ping should happen for each sensor.
  unsigned int cm[SONAR_NUM];         // Where the ping distances are stored.
  uint8_t currentSensor = 0;          // Keeps track of which sensor is active.

  NewPing sonar[SONAR_NUM] = {     // Sensor object array.
  NewPing(12, 11, MAX_DISTANCE), // Each sensor's trigger pin, echo pin, and max distance to ping.
  NewPing(10, 9, MAX_DISTANCE)
};
// ----------------END SONAR SENSORS SETUP ------------//



void setup() {
  Serial.begin(9600); // The Chosen Baud


  //******* GPS ******//
  // Start the software serial port at the GPS's default baud
  gpsSerial.begin(GPSBaud);
  //--------END GPS-----//

  
  // ******* PING (SONAR) ****** //
  pingTimer[0] = millis() + 75;           // First ping starts at 75ms, gives time for the Arduino to chill before starting.
  for (uint8_t i = 1; i < SONAR_NUM; i++) // Set the starting time for each sensor.
    pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;
  // --------- END PING ----------//

}// END void setup() 



void loop() {

  // ********* PING (SONAR) ******** // in the future add the load cell too*
  
  for (uint8_t i = 0; i < SONAR_NUM; i++) { // Loop through all the sensors.
    if (millis() >= pingTimer[i]) {         // Is it this sensor's time to ping?
      pingTimer[i] += PING_INTERVAL * SONAR_NUM;  // Set next time this sensor will be pinged.
      if (i == 0 && currentSensor == SONAR_NUM - 1) oneSensorCycle(); // Sensor ping cycle complete, do something with the results.
      sonar[currentSensor].timer_stop();          // Make sure previous timer is canceled before starting a new ping (insurance).
      currentSensor = i;                          // Sensor being accessed.
      cm[currentSensor] = 0;                      // Make distance zero in case there's no ping echo for this sensor.
      sonar[currentSensor].ping_timer(echoCheck); // Do the ping (processing continues, interrupt will call echoCheck to look for echo).
    }

  // Our Conditions for using GPS 
  if (cm[1] >= 1 && cm[1] <= 3)
  {
    Serial.println("GPS Start now! the current courdinate is: ");
    for(int i=0; i <= 20; i++) // let the GPS work properly (the GPS respond slowly)
    {
    while (gpsSerial.available() > 0)
        if(gps.encode(gpsSerial.read()))
          displayInfo();
    delay (100);
   }
  }
 } 
}// END void loop()


// ********** FUNCTION AREA *************
// here we write function to use in void loop

void echoCheck() { // If ping received, set the sensor distance to array.
  if (sonar[currentSensor].check_timer())
    cm[currentSensor] = sonar[currentSensor].ping_result / US_ROUNDTRIP_CM;
}

void oneSensorCycle() { // Sensor ping cycle complete, do something with the results.
  // The following code would be replaced with your code that does something with the ping results.
  for (uint8_t i = 0; i < SONAR_NUM; i++) {
    Serial.print(i);
    Serial.print("=");
    Serial.print(cm[i]);
    Serial.print("cm ");
  }
  Serial.println();
}

// ******** GPS functions **********
void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}
