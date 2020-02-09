#include <Arduino.h>
#include <Wire.h>  // for LiquidCrystal.
#include <LiquidCrystal.h> // for LCD shield.
#include <Adafruit_Sensor.h> // for DHT.
#include <DHT.h> // for DHT.

#define BTN_UP   1
#define BTN_DOWN 2
#define BTN_NONE 10

#define DHTPIN 13 // Pin for DHT22.
#define DHTTYPE DHT22 // DHT 22 Change this if you have a DHT11.
DHT dht(DHTPIN, DHTTYPE);

bool error = 1; // Measurements error state.
bool btn_None; // No button pressed flag.
unsigned long measurement_Delay = 60000; // Wait between measurements 1 minute by derfault.
unsigned long measurement_Timer;
unsigned long buttonDelay;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // Initialize the library with the numbers of the interface pins.

void setup()
{
        //Serial.begin(115200);
        lcd.begin(16, 2); // set up the LCD's number of columns and rows.
        dht.begin(); // Initialize DHT sensor.
}

int detectButton() {
        int keyAnalog =  analogRead(A0); // Read analog pin for input.
        if (keyAnalog < 200) {
                return BTN_UP;
        } else if (keyAnalog < 400) {
                return BTN_DOWN;
        } else {
                return BTN_NONE;
        }
}

void show_interval() // Draw INTERVAL menu on LCD.
{
        buttonDelay = millis();
        lcd.setCursor(0, 0);
        lcd.print("INTERVAL        ");
        lcd.setCursor(9, 0);
        lcd.print(measurement_Delay/1000);
        lcd.print(" sec");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        //Serial.print( Draw"INTERVAL menu on LCD "); //for debugging.
        //Serial.println(measurement_Delay/1000);  //for debugging.
}


void sendTemps()  // Get data from DHT22 and draw on LCD.
{
        float h = dht.readHumidity();
        float t = dht.readTemperature();
        //Serial.print("TEMP: "); //for debugging.
        //Serial.println(t); //for debugging.
        //Serial.print("HUM: "); //for debugging.
        //Serial.println(h); //for debugging.

        if (isnan(h) || isnan(t))   // Check for errors from DHT sensor.
        {
                //Serial.println("Failed to read from DHT sensor!"); //for debugging.
                lcd.setCursor(0, 0);
                lcd.print("  SENSOR ERROR  ");
                lcd.setCursor(0, 1);
                lcd.print("                ");
                error = 1;
                return;
        }

        lcd.setCursor(0, 0);
        lcd.print("TEMP:           ");
        lcd.setCursor(6, 0);
        lcd.print(round(t));
        lcd.print(" c");
        lcd.setCursor(7, 1);
        lcd.print("HUM:     ");
        lcd.setCursor(12, 1);
        lcd.print(round(h));
        lcd.print(" %");
        error = 0;
        measurement_Timer = millis();
}


void buttonJob()   // Button logic.
{
        int button = detectButton();
        switch (button) {
        case BTN_UP:
                btn_None = 0;
                if (measurement_Delay < 10000) {
                        measurement_Delay = measurement_Delay + 1000;
                        show_interval();
                        break;
                } else {
                        measurement_Delay = measurement_Delay + 10000;
                        show_interval();
                        break;
                }

        case BTN_DOWN:
                btn_None = 0;
                if (measurement_Delay < 3000) {
                        show_interval();
                        break;
                } else if (measurement_Delay <= 19000) {
                        measurement_Delay = measurement_Delay - 1000;
                        show_interval();
                        break;
                }else{
                        measurement_Delay = measurement_Delay - 10000;
                        show_interval();
                        break;
                }

        case BTN_NONE:
                if (btn_None == 0) sendTemps();
                btn_None = 1;
                break;
        }
        buttonDelay = millis();
}


void loop()
{
        if ((millis() - buttonDelay) > 500) buttonJob();

        if (error == 1 && btn_None == 1)
        {
                delay (2000); // Wait for next measurements if error occurs.
                sendTemps();
        }

        if ((millis() - measurement_Timer) > measurement_Delay && btn_None == 1) sendTemps();
}
