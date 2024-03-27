#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
// Update these with values suitable for your network.

#include <ESP8266Firebase.h>

#define FIREBASE_HOST "https://database-iot-python-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "AIzaSyBvWJkzD6_nSaFg2LPIoZlp-P065kdyV2k"
Firebase firebase(FIREBASE_HOST);
const char* ssid = "BossBad";
const char* password = "0918438492";
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

int bu_sw1 = 0;
int bu_sw2 = 0;
int bu_sw3 = 0;
int bu_sw4 = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String data_callback = "";
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    data_callback += (char)payload[i];
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);  // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
  Serial.println(" CALL BACK :" + data_callback);

  JsonDocument doc;
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, data_callback);

  // Test if parsing succeeds
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  // Fetch the values
  //
  // Most of the time, you can rely on the implicit casts.
  // In other case, you can do doc["time"].as<long>();
  int switch1 = doc["switch1"];
  EEPROM.write(10, switch1);
  // digitalWrite(D3, switch1);
  int switch2 = doc["switch2"];
  EEPROM.write(20, switch2);
  // digitalWrite(D3, switch2);
  // int switch3 = doc["switch3"];
  // EEPROM.write(30, switch3);
  // digitalWrite(D4, switch3);
  // int switch4 = doc["switch4"];
  // EEPROM.write(40, switch4);
  // digitalWrite(D5, switch4);

  Serial.println(" Data Switch 1 : " + String(switch1));
  Serial.println(" Data Switch 2 : " + String(switch2));
  // Serial.println(" Data Switch 3 : " + String(switch3));
  // Serial.println(" Data Switch 4 : " + String(switch4));
  
  digitalWrite(D3,switch1);
  digitalWrite(D5,switch1);
  digitalWrite(D8,switch2);
  digitalWrite(D6,switch2);

  Serial.println("Send Command ! ");

  EEPROM.commit();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("\n mqtt connected");
      // Once connected, publish an announcement...
      client.publish("Glenda5261", "on_start");
      // ... and resubscribe
      client.subscribe("dataSwitch");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  EEPROM.begin(512);
  // bu_sw1 = EEPROM.read(10);
  // bu_sw2 = EEPROM.read(20);
  // bu_sw3 = EEPROM.read(30);
  // bu_sw4 = EEPROM.read(40);

  pinMode(BUILTIN_LED, OUTPUT);  // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  pinMode(D6, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D8, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(A0,INPUT);

  Serial.print("Read Realtime Database \n");
  
  bu_sw1 = firebase.getInt("switch1");
  bu_sw2 = firebase.getInt("switch2");
  // bu_sw3 = firebase.getInt("switch3");
  // bu_sw4 = firebase.getInt("switch4");

  Serial.print("sw 1 : " + String(bu_sw1));
  Serial.print("\nsw 2 : " + String(bu_sw2));
  // Serial.print("\nsw 3 : " + String(bu_sw3));
  // Serial.println("\nsw 4 : " + String(bu_sw4));

  // digitalWrite(D6, bu_sw1);
  digitalWrite(D3, bu_sw1);
  digitalWrite(D5,bu_sw1);
  digitalWrite(D8, bu_sw2);
  digitalWrite(D6,bu_sw2);
  // digitalWrite(D5, bu_sw4);

  // firebase.setInt("push", 789);
  Serial.println(" Finish Setup ! ");
}
int data_send = 0 ;
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  firebase.setInt("push", map(analogRead(A0),1024,0,0,100));
  delay(1000);
  client.loop();
}
