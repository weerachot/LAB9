#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT22.h>

// ---------- WiFi ----------
const char* ssid = "Ruc2hxd";
const char* password = "ruch1232";

// ---------- MQTT ----------
const char* mqtt_server = "broker.emqx.io";
const char* pubTopic = "weerachot";
const char* subTopic = "weerachot/c";

// ---------- DHT ----------
#define pinDATA 15
DHT22 dht22(pinDATA);

// ---------- LED ----------
#define LED1 23
#define LED2 22

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;

// ---------- WiFi ----------
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ---------- MQTT callback ----------
void callback(char* topic, byte* payload, unsigned int length) {

  String message = "";

  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Message received: ");
  Serial.println(message);

  // ---------- ควบคุม LED ----------
  if (message == "LED1_ON") {
    digitalWrite(LED1, HIGH);
  } 
  else if (message == "LED1_OFF") {
    digitalWrite(LED1, LOW);
  } 
  else if (message == "LED2_ON") {
    digitalWrite(LED2, HIGH);
  } 
  else if (message == "LED2_OFF") {
    digitalWrite(LED2, LOW);
  }
}

// ---------- MQTT reconnect ----------
void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting MQTT...");

    String clientId = "ESP32-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");

      // subscribe คำสั่งควบคุม
      client.subscribe(subTopic);

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retry in 2 sec");
      delay(2000);
    }
  }
}

// ---------- setup ----------
void setup() {
  Serial.begin(115200);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

// ---------- loop ----------
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();

  if (now - lastMsg > 3000) {

    float temp = dht22.getTemperature();
    float hum = dht22.getHumidity();

    if (isnan(temp) || isnan(hum)) {
      Serial.println("Failed to read from DHT!");
      return;
    }

    char msg[100];

    snprintf(msg, sizeof(msg),
             "{\"Tempp\": %.2f, \"Humid\": %.2f}",
             temp, hum);

    Serial.print("Publish: ");
    Serial.println(msg);

    client.publish(pubTopic, msg);

    lastMsg = now;
  }
}