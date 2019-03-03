
////////////////////////////////////////////////
// Relay pinout
// Arduino side:
// Relay      Arduino pin
// VCC    -     5V
// GND    -     GND
// IN     -     Digital Pin 2
//
// Power supply side:
// GND passed through the pins labeled:
// "Normally Open"
//
const int relayPin = 2;
////////////////////////////////////////////////

////////////////////////////////////////////////
// Fan power block pinout:
// Ethernet   Power supply
// Orange   -   12V
// OR/WH    -   Relay (Power supply)
////////////////////////////////////////////////

////////////////////////////////////////////////
// Temp sensor pinout:
// TMP36    Arduino
// GRN    -   3V
// GRN/WH -   GND
// BRN    -   A0
//
const int temperatureSensorPin = A0;
int temperatureSensorValue = 0;
float voltage = 0;
float temperature = 0;
float fahrenheit = 0;

// The temp sensor returns skewed values.
// This is probably a factor of line loss as a result of distance
// and guage of wire used between the sensor and the controller.
const int FAHRENEITADJUSTMENTFACTOR = 35;
////////////////////////////////////////////////


const int FANPOWERONTHRESHOLD = 80; // Temperature to start fans
const unsigned long CHECKTHRESHOLD = 180000; // 3 mins
//const unsigned long CHECKTHRESHOLD = 18000; // 18 seconds

char formattedText[80];
long counter = 0;
unsigned long average;
unsigned long elapsedTime = 0;

// Uncomment the next line for serial output
//#define DEBUG

void setup() {
  Serial.begin(9600);
  delay(200);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);
  pinMode(temperatureSensorPin, INPUT);
}

void loop() {
  
  temperatureSensorValue = analogRead(temperatureSensorPin);
  voltage = (temperatureSensorValue / 1024.0) * 5.0;
  temperature = (voltage - .5) * 100;
  fahrenheit = (temperature * 9/5 + 32) + FAHRENEITADJUSTMENTFACTOR;

  average += (int)fahrenheit;
  counter++;
  elapsedTime = elapsedTime + 1000;

#ifdef DEBUG
  sprintf(formattedText, "temp: %d - average: %ld - elapsedTime: %ld", (int)fahrenheit, average / counter, elapsedTime);
  Serial.println(formattedText);
#endif

  if (CHECKTHRESHOLD < elapsedTime)
  {
    if (FANPOWERONTHRESHOLD < average / counter)
    {
      // It's showtime!
#ifdef DEBUG
      sprintf(formattedText, "ON: Temp: %d - Avg: %d", (int)fahrenheit, average / counter);
      Serial.println(formattedText);
#endif
      digitalWrite(relayPin, LOW);
    }
    else
    {
#ifdef DEBUG
      sprintf(formattedText, "OFF: Temp: %d - Avg: %d", (int)fahrenheit, average / counter);
      Serial.println(formattedText);
#endif
      digitalWrite(relayPin, HIGH);
    }
    average = 0;
    counter = 0;
    elapsedTime = 0;
  }
  delay(1000);
}
