#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT22.h>

const char *My_SSID = "Ruc2hxd";
const char *My_Pass = "ruch1232";
const char *MQTT_Server = "broker.emqx.io";
const char *PTopic = "weerachot";
#define MQTT_Port 1883
#define pinDATA 15

DHT22 dht22(pinDATA);
WiFiClient espClient;
PubSubClient client(espClient);

long lastDHT = 0;
char msg[150];

void Setup_Wifi() {
  WiFi.begin(My_SSID, My_Pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP32-Temp-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT connected");
    } else {
      delay(3000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Setup_Wifi();
  client.setServer(MQTT_Server, MQTT_Port);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  long now = millis();
  if (now - lastDHT > 5000) {  // ส่งทุก 5 วินาที
    lastDHT = now;
    float t = dht22.getTemperature();
    float h = dht22.getHumidity();

    if (!isnan(t) && !isnan(h)) {
      snprintf(msg, 150, "{\"temperature\": %.2f, \"humidity\": %.2f}", t, h);
      client.publish(PTopic, msg);
      Serial.print(t);
      Serial.print(" | ");
      Serial.println(h);
    }
  }
}