/*
 * Constant sampling of ADXL335 and sending it to client at 1K Samples Per Second 
 * Checked with  LABVIEW Waveform Graph 
 * Using FIR Moving Average [20] 
 */

#include <WiFi.h>
#include <filters.h>
#include <FIR.h>
FIR<float, 20> firx;
FIR<float, 20> firy;
FIR<float, 20> firz;

#define PERIOD 600

unsigned long last_us = 0L;
int passed;

const char* ssid = "Radopsys Lab";
const char* password =  "Radopsys@2019";
WiFiServer wifiServer(80);

const IPAddress local_IP(192, 168, 1, 162);
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);

void setup() {
  WiFi.config(local_IP, gateway, subnet);
  Serial.begin(115200);

  // For a moving average we want all of the coefficients to be unity.
  float coefx[20] = { 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1.};
  float coefy[20] = { 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1.};
  float coefz[20] = { 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1.};

  firx.setFilterCoeffs(coefx);
  firy.setFilterCoeffs(coefy);
  firz.setFilterCoeffs(coefz);

  delay(1000);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());
  wifiServer.begin();
  pinMode(5, OUTPUT);
}

void loop ()
{
  WiFiClient client = wifiServer.available();

  int x1, x, y1, y, z1, z;

  if (client) {
    while (client.connected()) {
      if (micros () - last_us > PERIOD)
      {
        last_us += PERIOD ;

        digitalWrite(5, HIGH);   // For determining Sampling Rate 

        x1 = analogRead(36);
        x = firx.processReading(x1);

        y1 = analogRead(39);
        y = firy.processReading(y1);

        z1 = analogRead(34);
        z = firz.processReading(z1);

        passed = client.print("*" + String (x) + "  " + String (y) + "  " + String (z) + "#");
        digitalWrite(5, LOW);     // For determining Sampling Rate
        Serial.println(passed);
        delayMicroseconds(105);   // For acheiving perfect sampling rate. 
      }
    }
    delay(10);
  }
  client.stop();
  Serial.print(passed);
  Serial.println("Client disconnected");
}
