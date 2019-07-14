// Please read the ready-to-localize tuturial together with this example.
// https://www.pozyx.io/Documentation/Tutorials/ready_to_localize
/**
  The Pozyx ready to localize tutorial (c) Pozyx Labs

  Please read the tutorial that accompanies this sketch: https://www.pozyx.io/Documentation/Tutorials/ready_to_localize/Arduino

  This tutorial requires at least the contents of the Pozyx Ready to Localize kit. It demonstrates the positioning capabilities
  of the Pozyx device both locally and remotely. Follow the steps to correctly set up your environment in the link, change the
  parameters and upload this sketch. Watch the coordinates change as you move your device around!
*/
#include <Pozyx.h>
#include <Pozyx_definitions.h>
#include <Wire.h>
#include <ESP8266WiFi.h>

//Initialization for WIFI Connection
const char* ssid     = "Redmi";
const char* password = "hahahaha";
const char* host = "192.168.43.58";
const uint16_t port = 8080;
unsigned long prevtime = 0;
const long interval = 2000;
String url = "/androidTA/calculation.php?username=";
String urlres = "/androidTA/resetjarak.php?username=";
String uname = "ask";
int num = 0;
int lowx = 2800, upx = 4600;
int lowy = 2000, upy = 3500;

//Initialization for Pozyx device
uint16_t remote_id = 0x677B;                            // set this to the ID of the remote device
bool remote = false;                                    // set this to true to use the remote ID
boolean use_processing = false;                         // set this to true to output data for the processing sketch
const uint8_t num_anchors = 4;                                    // the number of anchors
uint16_t anchors[num_anchors] = {0x6764, 0x6772, 0x6722, 0x671D};     // the network id of the anchors: change these to the network ids of your anchors.
int32_t anchors_x[num_anchors] = {0, 6000, 6000, 0};               // anchor x-coorindates in mm
int32_t anchors_y[num_anchors] = {5600, 0, 5600, 0};                 // anchor y-coordinates in mm
int32_t heights[num_anchors] = {1700, 1700 , 1700, 1700};              // anchor z-coordinates in mm
uint8_t algorithm = POZYX_POS_ALG_UWB_ONLY;             // positioning algorithm to use. try POZYX_POS_ALG_TRACKING for fast moving objects.
uint8_t dimension = POZYX_3D;                           // positioning dimension
int32_t height = 1000;                                  // height of device, required in 2.5D positioning
double ncoorx = 0, ncoory = 0;
int i = 1;

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

void resetJarak(const char* hosttt, const uint16_t portnumber, String urlreset, String username) {
  urlreset += username;
  WiFiClient client;
  if (!client.connect(hosttt, portnumber)) {
    Serial.println("Connection failed");
    delay(5000);
    return;
  }
  Serial.println("Reset distance");
  if (client.connected()) {
    Serial.println(urlreset);
    client.print(String("GET ") + urlreset + " HTTP/1.1\r\n" +
                 "Host: " + hosttt + "\r\n" +
                 "Connection: close\r\n\r\n");

    // wait for data to be available
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
  }
  Serial.println("Successfully reset");
  Serial.println("---------------------------------------------------------------\n");
  client.stop();
}

//Send data to localhost database
void sendingData(const char* hosttt, const uint16_t portnumber, String urlGet, String username, double x, double y) {
  Serial.print("Connecting to ");
  Serial.print(hosttt);
  Serial.print(':');
  Serial.println(portnumber);
  //http://localhost:8080/androidTA/calculation.php?username=ask&coorx2=100&coory2=200
  urlGet += username;
  urlGet += "&coorx2=";
  urlGet += x;
  urlGet += "&coory2=";
  urlGet += y;
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(hosttt, portnumber)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }

  // This will send a string to the server
  Serial.println("Sending data to server");
  if (client.connected()) {
    Serial.println(urlGet);
    client.print(String("GET ") + urlGet + " HTTP/1.1\r\n" +
                 "Host: " + hosttt + "\r\n" +
                 "Connection: close\r\n\r\n");

    // wait for data to be available
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
  }
  Serial.println("Closing connection");
  Serial.println("---------------------------------------------------------------\n");
  client.stop();
}

void setup() {
  Serial.begin(115200);
  connectWifi(ssid, password);
  if (Pozyx.begin() == POZYX_FAILURE) {
    Serial.println(F("ERROR: Unable to connect to POZYX shield"));
    Serial.println(F("Reset required"));
    delay(100);
    abort();
  }
  if (!remote) {
    remote_id = NULL;
  }
  Serial.println(F("----------POZYX POSITIONING V1.1----------"));
  Serial.println(F("NOTES:"));
  Serial.println(F("- No parameters required."));
  Serial.println();
  Serial.println(F("- System will auto start anchor configuration"));
  Serial.println();
  Serial.println(F("- System will auto start positioning"));
  Serial.println(F("----------POZYX POSITIONING V1.1----------"));
  Serial.println();
  Serial.println(F("Performing manual anchor configuration:"));

  // clear all previous devices in the device list
  Pozyx.clearDevices(remote_id);
  // sets the anchor manually
  setAnchorsManual();
  // sets the positioning algorithm
  Pozyx.setPositionAlgorithm(algorithm, dimension, remote_id);

  printCalibrationResult();
  delay(2000);
  //resetJarak(host, port, urlres, uname);
  Serial.println(F("Starting positioning: "));
}

void loop() {
  //delay(40);
  coordinates_t position;
  int status;
  if (remote) {
    status = Pozyx.doRemotePositioning(remote_id, &position, dimension, height, algorithm);
  } else {
    status = Pozyx.doPositioning(&position, dimension, height, algorithm);
  }
  if (status == POZYX_SUCCESS) {
    //prints out the result
    //printCoordinates(position);
    uint16_t network_id = remote_id;
    if (network_id == NULL) {
      network_id = 0;
    }
    if (!use_processing) {
      if (position.x > 0 && position.y > 0) {
        Serial.print("POS ID 0x");
        Serial.print(network_id, HEX);
        Serial.print(", x(mm): ");
        Serial.print(position.x);
        Serial.print(", y(mm): ");
        Serial.print(position.y);
        Serial.print(", z(mm): ");
        Serial.println(position.z);
        ncoorx = ncoorx + position.x;
        ncoory = ncoory + position.y;
        sendingData(host, port, url, uname, ncoorx , ncoory);
        ncoorx = 0;
        ncoory = 0;
      }
    }
  } else {
    // prints out the error code
    printErrorCode("Positioning");
  }
}

// error printing function for debugging
void printErrorCode(String operation) {
  uint8_t error_code;
  if (remote_id == NULL) {
    Pozyx.getErrorCode(&error_code);
    Serial.print("ERROR ");
    Serial.print(operation);
    Serial.print(", local error code: 0x");
    Serial.println(error_code, HEX);
    return;
  }
  int status = Pozyx.getErrorCode(&error_code, remote_id);
  if (status == POZYX_SUCCESS) {
    Serial.print("ERROR ");
    Serial.print(operation);
    Serial.print(" on ID 0x");
    Serial.print(remote_id, HEX);
    Serial.print(", error code: 0x");
    Serial.println(error_code, HEX);
  } else {
    Pozyx.getErrorCode(&error_code);
    Serial.print("ERROR ");
    Serial.print(operation);
    Serial.print(", couldn't retrieve remote error code, local error: 0x");
    Serial.println(error_code, HEX);
  }
}

// print out the anchor coordinates (also required for the processing sketch)
void printCalibrationResult() {
  uint8_t list_size;
  int status;

  status = Pozyx.getDeviceListSize(&list_size, remote_id);
  Serial.print("list size: ");
  Serial.println(status * list_size);

  if (list_size == 0) {
    printErrorCode("configuration");
    return;
  }

  uint16_t device_ids[list_size];
  status &= Pozyx.getDeviceIds(device_ids, list_size, remote_id);

  Serial.println(F("Calibration result:"));
  Serial.print(F("Anchors found: "));
  Serial.println(list_size);

  coordinates_t anchor_coor;
  for (int i = 0; i < list_size; i++)
  {
    Serial.print("ANCHOR,");
    Serial.print("0x");
    Serial.print(device_ids[i], HEX);
    Serial.print(",");
    Pozyx.getDeviceCoordinates(device_ids[i], &anchor_coor, remote_id);
    Serial.print(anchor_coor.x);
    Serial.print(",");
    Serial.print(anchor_coor.y);
    Serial.print(",");
    Serial.println(anchor_coor.z);
  }
}
// function to manually set the anchor coordinates
void setAnchorsManual() {
  for (int i = 0; i < num_anchors; i++) {
    device_coordinates_t anchor;
    anchor.network_id = anchors[i];
    anchor.flag = 0x1;
    anchor.pos.x = anchors_x[i];
    anchor.pos.y = anchors_y[i];
    anchor.pos.z = heights[i];
    Pozyx.addDevice(anchor, remote_id);
  }
  if (num_anchors > 4) {
    Pozyx.setSelectionOfAnchors(POZYX_ANCHOR_SEL_AUTO, num_anchors, remote_id);
  }
}
