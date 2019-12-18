// uMQTTBroker and AsyncWebServer require additional libraries:
//  - uMQTTBroker:       https://github.com/martin-ger/uMQTTBroker
//  - ESPAsyncTCP:       https://github.com/me-no-dev/ESPAsyncTCP
//  - ESPAsyncWebServer: https://github.com/me-no-dev/ESPAsyncWebServer
#include <ESP8266WiFi.h>
#include <FS.h>
#include <uMQTTBroker.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Gateway IP address, gateway, and netmask
const IPAddress netIP(192, 168, 4, 1);              // Network IP
const IPAddress netGateway = netIP;                 // Network gateway
const IPAddress netNetmask(255, 255, 255, 0);       // Network netmask

// Access point SSID and passsword
const char* apSSID = "Secret Santa";                // Access point SSID
const char* apPassword = "Christmas 2019!";         // Access point password

// MQTT broker port, max subscribed and retain topics, and message topic
const uint16_t mqttPort = 1883;                     // Bind port
const uint16_t mqttMaxSub = 30;                     // Max subscribed topics
const uint16_t mqttMaxRetain = 30;                  // Max retained topics
const char* mqttTopicCmnd = "cmnd/sonoff/POWER";    // Message topic for commands
const char* mqttTopicStat = "stat/sonoff/POWER";    // Message topic for state
const uint8_t mqttQosCmnd = 1;                      // Message QoS for commands
const uint8_t mqttQosStat = 1;                      // Message QoS for state
bool mqttState = false;                             // Sonoff state

// AsyncWebServer port
const uint16_t webPort = 80;                        // Web server port

// Instantiate AsyncWebServer
AsyncWebServer server(webPort);

// Variables to track the client list, and the last time we listed clients
String clientList = "";
long lastClientList = 0;

// Reset function
void(*reset) (void) = 0;

// Template variable replace function
String processor(const String& var) {
    if (var == "STATE") {
        return mqttState ? "ON" : "OFF";
    } else if (var == "CLIENTLIST") {
        return clientList;
    }
}

// Handle MQTT messages from the Sonoff or other MQTT clients
void handleMQTT(uint32_t* args, const char* topic, uint32_t topic_len, const char* data, uint32_t data_len) {
    // Read the topic and data strings into variables
    char topic_str[topic_len + 1]; os_memcpy(topic_str, topic, topic_len); topic_str[topic_len] = '\0';
    char data_str[data_len + 1]; os_memcpy(data_str, data, data_len); data_str[data_len] = '\0';

    // If the topic is the stat topic
    if (strcmp(topic_str, mqttTopicStat) == 0) {
        // And the data is either "ON" or "OFF"
        if (strcmp(data_str, "ON") == 0 || strcmp(data_str, "OFF") == 0) {
            // Update mqttState accordingly
            mqttState = strcmp(data_str, "ON") == 0;
        }
    }
}

// List connected clients
String getClients() {
    struct station_info* station_list = wifi_softap_get_station_info();

    String rtn = "";

    // Generate an HTML table
    rtn = "<table style=\"display: inline-block;border-spacing: 0;border-collapse: collapse;\">";
    rtn += "<thead><tr><th>IP Address</th><th>MAC Address</th></tr></thead>";
    rtn += "<tbody>";

    // Add a row for each client
    while (station_list != NULL) {
        char station_mac[18] = { 0 }; sprintf(station_mac, "%02X:%02X:%02X:%02X:%02X:%02X", MAC2STR(station_list->bssid));
        String station_ip = IPAddress((&station_list->ip)->addr).toString();
        rtn += "<tr><td>" + station_ip + "</td><td>" + station_mac + "</td></tr>";
        station_list = STAILQ_NEXT(station_list, next);
    }

    // Terminate the table
    rtn += "</tbody></table>";

    wifi_softap_free_station_info();

    return rtn;
}

// Setup code
void setup() {
    // Setup serial
    Serial.begin(115200);
    delay(10);

    // Set pin mode
    pinMode(LED_BUILTIN, OUTPUT);

    // Initialize SPIFFS
    Serial.println("\nInitializing SPIFFS...");
    if (!SPIFFS.begin()) {
        Serial.println("Failure! Resetting in 10 seconds.");
        delay(10000);
        reset();
    }
    Serial.println("Success! SPIFFS initialized.");

    // Configure gateway
    Serial.println("\nConfiguring gateway...");
    Serial.print("- IP Address: "); Serial.println(netIP);
    Serial.print("- Gateway: "); Serial.println(netGateway);
    Serial.print("- Netmask: "); Serial.println(netNetmask);
    if (!WiFi.softAPConfig(netIP, netGateway, netNetmask)) {
        Serial.println("Failure! Resetting in 10 seconds.");
        delay(10000);
        reset();
    }
    Serial.println("Success! Gateway configured.");

    // Start access point
    Serial.println("\nStarting access point...");
    Serial.print("- SSID: "); Serial.println(apSSID);
    Serial.print("- Password: "); Serial.println(apPassword);
    if (!WiFi.softAP(apSSID, apPassword)) {
        Serial.println("Failure! Resetting in 10 seconds.");
        delay(10000);
        reset();
    }
    Serial.println("Success! Access point started.");

    // Start MQTT broker
    Serial.println("\nStarting MQTT broker...");
    Serial.print("- Port: "); Serial.println(mqttPort);
    Serial.print("- Max sub: "); Serial.println(mqttMaxSub);
    Serial.print("- Max retain: "); Serial.println(mqttMaxRetain);
    Serial.print("- Cmnd topic: "); Serial.println(mqttTopicCmnd);
    Serial.print("- Stat topic: "); Serial.println(mqttTopicStat);
    if (!MQTT_server_start(mqttPort, mqttMaxSub, mqttMaxRetain)) {
        Serial.println("Failure! Resetting in 10 seconds.");
        delay(10000);
        reset();
    }
    Serial.println("Success! MQTT broker started.");

    // Subscribe to MQTT topics and register a callback
    MQTT_local_subscribe((uint8_t*)mqttTopicCmnd, mqttQosCmnd);
    MQTT_local_subscribe((uint8_t*)mqttTopicStat, mqttQosStat);
    MQTT_server_onData(handleMQTT);

    // Start the web server
    Serial.println("\nStarting web server...");
    Serial.print("- Port: "); Serial.println(webPort);

    // Serve the main page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.print("-- 200 (/) : "); Serial.println(request->url());
        //request->send(200, "text/plain", "Hello, world!");
        request->send(SPIFFS, "/index.html", String(), false, processor);
    });

    // Serve resources
    server.on("/res/css/main.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/res/css/main.css", "text/css");
    });
    server.on("/res/js/main.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/res/js/main.js", "text/javascript");
    });

    // Requests to get state and clients and set state
    server.on("/state", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", mqttState ? "ON" : "OFF");
    });
    server.on("/clients", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", clientList);
    });
    server.on("/toggle", HTTP_POST, [](AsyncWebServerRequest *request) {
        mqttState = !mqttState;
        MQTT_local_publish((uint8_t*)mqttTopicCmnd, (uint8_t*)(mqttState ? "ON" : "OFF"), mqttState ? 2 : 3, mqttQosCmnd, 0);
        request->send(200, "text/plain", mqttState ? "ON" : "OFF");
    });

    // For everything else, serve a 404 page
    server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "Not found");
    });

    // Start the AsyncWebServer
    server.begin();
    Serial.println("Success! Web server started.");
}

// Main loop
void loop() {
    // Update the client list every 3 seconds
    if (millis() >= lastClientList + 3000 && WiFi.softAPgetStationNum() > 0) {
        clientList = getClients();
        lastClientList = millis();
    }
}

