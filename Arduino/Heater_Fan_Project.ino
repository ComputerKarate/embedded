////////////////////////////////////////////////
// Relay pinout:
// VCC - 5V
// GND - GND
// IN  - Pin2
////////////////////////////////////////////////

////////////////////////////////////////////////
// Fan power block pinout:
// Orange - 12V
// OR/WH  - GND
////////////////////////////////////////////////

////////////////////////////////////////////////
// Temp sensor pinout:
// GRN    - 3V
// GRN/WH - GND
// BRN    - A0
////////////////////////////////////////////////

const int temperatureSensorPin = A0;
const int relayPin = 2;
int temperatureSensorValue = 0;
float voltage = 0;
float temperature = 0;
float fahrenheit = 0;
const int FanPowerOnThreshold = 80; // Temperature to start fans
char formattedText[80];
unsigned long elapsedTime = 0;
unsigned long average;
long counter = 0;
const unsigned long CHECKTHRESHOLD = 180000; // 3 mins
//const unsigned long CHECKTHRESHOLD = 18000; // 18 seconds

#define DEBUG

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
  fahrenheit = temperature * 9/5 + 32;

  average += (int)fahrenheit;
  counter++;
  elapsedTime = elapsedTime + 1000;

#ifdef DEBUG
  sprintf(formattedText, "average: %ld - elapsedTime: %ld",
                average / counter, elapsedTime);
  Serial.println(formattedText);
#endif

  if (CHECKTHRESHOLD < elapsedTime)
  {
    if (FanPowerOnThreshold < average / counter)
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
