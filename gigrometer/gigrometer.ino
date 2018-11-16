// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include <DHT.h>
#include <Wire.h>
#include <RtcDS1307.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 3     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define countof(a) (sizeof(a) / sizeof(a[0]))

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);
RtcDS1307<TwoWire> Rtc(Wire);
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup() {
  //Serial.begin(9600); //uncomment to print to console
  dht.begin();
  lcd.init();                      // initialize the lcd
  lcd.init();


  Serial.print("compiled: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);

  //--------RTC SETUP ------------
  // if you are using ESP-01 then uncomment the line below to reset the pins to
  // the available pins for SDA, SCL
  // Wire.begin(0, 2); // due to limited pins, use pin 0 and 2 for SDA, SCL

  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();

  if (!Rtc.IsDateTimeValid())
  {
    // Common Cuases:
    //    1) first time you ran and the device wasn't running yet
    //    2) the battery on the device is low or even missing

    Serial.println("RTC lost confidence in the DateTime!");

    // following line sets the RTC to the date & time this sketch was compiled
    // it will also reset the valid flag internally unless the Rtc device is
    // having an issue

    Rtc.SetDateTime(compiled);
  }

  if (!Rtc.GetIsRunning())
  {
    Serial.println("RTC was not actively running, starting now");
    Rtc.SetIsRunning(true);
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled)
  {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  }
  else if (now > compiled)
  {
    Serial.println("RTC is newer than compile time. (this is expected)");
  }
  else if (now == compiled)
  {
    Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }

  // never assume the Rtc was last configured by you, so
  // just clear them to your needed state
  Rtc.SetSquareWavePin(DS1307SquareWaveOut_Low);

  lcd.clear();
}

void loop() {
  int k = 0;
  while (k < 10) {
    printTempHumidity();
    // Wait a few seconds between measurements.
    k += 2;
    delay(2000);
  }
  lcd.clear();

  int i = 0;
  while (i < 7) {
    RtcDateTime now = Rtc.GetDateTime();
    printDateTime(now);
    i++;
    delay(1000);
  }
  lcd.clear();
}

void printTempHumidity() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Humidity: ");
  lcd.setCursor(10, 0);
  lcd.print((int)h);
  lcd.setCursor(12, 0);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("Temperature: ");
  lcd.setCursor(13, 1);
  lcd.print((int)t);
  lcd.setCursor(15, 1);
  lcd.print("C");
}

void printDateTime(const RtcDateTime& dt) {
  char datestring[20];
  char timestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u.%02u.%04u"),
             dt.Month(),
             dt.Day(),
             dt.Year());
  snprintf_P(timestring,
             countof(timestring),
             PSTR("%02u:%02u:%02u"),
             dt.Hour(),
             dt.Minute(),
             dt.Second());

  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  lcd.setCursor(6, 0);
  lcd.print(timestring);
  lcd.backlight();
  lcd.setCursor(0, 1);
  lcd.print("Date: ");
  lcd.setCursor(6, 1);
  lcd.print(datestring);
}
