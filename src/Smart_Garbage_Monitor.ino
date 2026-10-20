#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

//----------- Wi-Fi Configuration (Placeholders) ---------//
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";
//-------------------------------------------------------//

//----------- ThingSpeak Channel Configuration ----------//
unsigned long Channel_ID = 0000000;              // Enter your Channel ID
const char * WriteAPIKey = "YOUR_WRITE_API_KEY"; // Enter your Write API Key
const int Field_number = 1; 
//-------------------------------------------------------//

// ---------- Garbage Bin Physical Dimensions ----------//
const float total_height = 21.5; // Total bin height in cm
const float hold_height = 16.0;  // Holding capacity in cm
float snsr_to_max = 0.0;
//-------------------------------------------------------//

// Hardware Pin Configuration
const int trigger = D5;
const int echo = D6;

long duration;
int distanceCM;
int resultCM;
int bin_lvl = 0;

WiFiClient client;
unsigned long lastUploadTime = 0;
const unsigned long uploadInterval = 60000; // Upload interval: 1 minute (60,000 ms)

void setup()
{
  Serial.begin(115200);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  
  snsr_to_max = total_height - hold_height;
  delay(1500);
}

void loop()
{
  ensureInternetConnection();
  measureDistance();

  // Non-blocking upload every 60 seconds
  if (millis() - lastUploadTime >= uploadInterval)
  {
    Serial.print("Garbage Level: ");
    Serial.print(bin_lvl);
    Serial.println("%");
    
    uploadData();
    lastUploadTime = millis();
  }
}

void measureDistance()
{
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  duration = pulseIn(echo, HIGH);
  distanceCM = duration * 0.034 / 2;

  bin_lvl = map(distanceCM, snsr_to_max, total_height, 100, 0);
  if (bin_lvl > 100) bin_lvl = 100;
  if (bin_lvl < 0) bin_lvl = 0;  
}

void uploadData()
{
  ensureInternetConnection();
  int status = ThingSpeak.writeField(Channel_ID, Field_number, bin_lvl, WriteAPIKey);
  
  if (status == 200) {
    Serial.println("Data updated successfully.");
  } else {
    Serial.print("Upload failed, HTTP error code: ");
    Serial.println(status);
  }
}

void ensureInternetConnection()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Connecting to Wi-Fi...");
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWi-Fi Connected.");
  }
}
