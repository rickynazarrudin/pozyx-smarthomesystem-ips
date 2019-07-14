#include <Wire.h>
#include <ESP8266WiFi.h>

//Initialization for WIFI Connection
const char* ssid     = "Redmi";
const char* password = "hahahaha";
const char* host = "192.168.43.58";
const uint16_t port = 8080;
const int relay1 = 12;   //Arduino pin that triggers relay #1
const int relay2 = 13;   //Arduino pin that triggers relay #2
String uname = "ask";
String readString;
WiFiClient client;
void extendActuator() {
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, LOW);
}
void retractActuator() {
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, HIGH);
}
// Function to connect to get the connection and set the esp8266 on WEMOS D1 to be a client
void connectWifi(const char* wifi, const char* pswd) {
  Serial.print("Connecting to ");
  Serial.println(wifi);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi, pswd);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  connectWifi(ssid, password);
}

void loop() {
  if (client.connect(host, port))
  {
    Serial.println("Connected");
    client.println("GET /androidTA/door_status.php?door=door_1 HTTP/1.1");
    client.println("Host: 192.168.43.58:8080");
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("Connection failed");
  }
  while (client.connected() && !client.available()) delay(1); //waits for data
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
    readString += c; //places captured byte in readString
  }
  client.stop();
  Serial.println("Closing connection");
  Serial.println("---------------------------------------------------------------\n");
  //Serial.println(readString);
  if (readString.indexOf("door_1") > 0)
  {
    Serial.println("Open the first door");
    extendActuator();
  }else if (readString.indexOf("closed") > 0){
    Serial.println("Close the door");
    retractActuator();
  }else{
    Serial.println(readString);
  }
  readString = ""; 
}
