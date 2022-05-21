#define ENA   14          // Enable/speed motors Right        GPIO14(D5)
#define ENB   12          // Enable/speed motors Left         GPIO12(D6)
#define IN_1  15          // L298N in1 motors Rightx          GPIO15(D8)
#define IN_2  13          // L298N in2 motors Right           GPIO13(D7)
#define IN_3  2           // L298N in3 motors Left            GPIO2(D4)
#define IN_4  0           // L298N in4 motors Left            GPIO0(D3)

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <SocketIOclient.h>

#define USE_SERIAL Serial

int robotSpeed;
int speed_Coeff = 3;

WebSocketsClient webSocket;
SocketIOclient socketIO;

const char* ssid = "Kamlesh";
const char* pass = "kamleshshilpi";

void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case sIOtype_DISCONNECT:
            USE_SERIAL.printf("[IOc] Disconnected!\n");
            break;
        case sIOtype_CONNECT:
            {
                USE_SERIAL.printf("[IOc] Connected to url: %s\n", payload);
               
                socketIO.send(sIOtype_CONNECT, "/");

                // Registering myself on the NodeJS server SOCKET-IO
                DynamicJsonDocument doc(1024);
                JsonArray array = doc.to<JsonArray>();
               
                array.add("REGISTER-NODE-MCU");
             
                JsonObject param1 = array.createNestedObject();
               
                // My MAC address for robot: 10:52:1C:02:05:4E
                // param1["macAddress"] = WiFi.macAddress();

                param1["macAddress"] = "kamlesh";
                param1["clientName"] = "NodeMCU";
             
                String output;
                serializeJson(doc, output);

                initializeAllMovement();
                socketIO.sendEVENT(output);
             
                USE_SERIAL.println(output);
            }
            break;
        case sIOtype_EVENT:
            {
                 char * sptr = NULL;
                 int id = strtol((char *)payload, &sptr, 10);
                 USE_SERIAL.printf("[IOc] get event: %s id: %d\n", payload, id);
                 if(id) {
                     payload = (uint8_t *)sptr;
                 }
                 DynamicJsonDocument doc(1024);
                 DeserializationError error = deserializeJson(doc, payload, length);
                 if(error) {
                     USE_SERIAL.println(F("deserializeJson() failed: "));
                     USE_SERIAL.println(error.c_str());
                     return;
                 }
                 String eventName = doc[1];
                 captureMovement(eventName);
             }
            break;
        case sIOtype_ACK:
            USE_SERIAL.printf("[IOc] get ack: %u\n", length);
            hexdump(payload, length);
            break;
        case sIOtype_ERROR:
            USE_SERIAL.printf("[IOc] get error: %u\n", length);
            hexdump(payload, length);
            break;
        case sIOtype_BINARY_EVENT:
            USE_SERIAL.printf("[IOc] get binary: %u\n", length);
            hexdump(payload, length);
            break;
        case sIOtype_BINARY_ACK:
            USE_SERIAL.printf("[IOc] get binary ack: %u\n", length);
            hexdump(payload, length);
            break;
    }
}

void setup() {
  delay(5000);
  USE_SERIAL.print("Jai BholeNath JI");
 
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);
 
  USE_SERIAL.begin(115200);
 
  connectWiFi();

  socketIO.begin("167.71.225.180", 80, "/api/v1/robo-g-connect/socket.io/?EIO=4");
  socketIO.onEvent(socketIOEvent);
}

void loop() {
  socketIO.loop();
}

void initializeAllMovement() {
  robotSpeed = 400;
  builtinLedOFF();
}

void captureMovement(String movement) {
    USE_SERIAL.println(movement);
    if (movement == "forward") goForward();
    else if (movement == "back") goBack();
    else if (movement == "left") goLeft();
    else if (movement == "right") goRight();
    else if (movement == "forward-right") goForwardRight();
    else if (movement == "forward-left") goForwardLeft();
    else if (movement == "back-right") goBackRight();
    else if (movement == "back-left") goBackLeft();
    else if (movement == "speed-0") robotSpeed = 100;
    else if (movement == "speed-1") robotSpeed = 125;
    else if (movement == "speed-2") robotSpeed = 150;
    else if (movement == "speed-3") robotSpeed = 175;
    else if (movement == "speed-4") robotSpeed = 200;
    else if (movement == "speed-5") robotSpeed = 250;
    else if (movement == "speed-6") robotSpeed = 300;
    else if (movement == "speed-7") robotSpeed = 350;
    else if (movement == "speed-8") robotSpeed = 400;
    else if (movement == "speed-9") robotSpeed = 450;
    else if (movement == "builtin-led-on") builtinLedON();
    else if (movement == "builtin-led-off") builtinLedOFF();
    else if (movement == "stop-all-movement") stopAllMovement();
    else if (movement == "stop") stopRobot();
}

void builtinLedON(){
    digitalWrite(LED_BUILTIN, LOW);
}

void builtinLedOFF(){
    digitalWrite(LED_BUILTIN, HIGH);
}

void goForward(){
    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, HIGH);
    analogWrite(ENA, robotSpeed);

    digitalWrite(IN_3, LOW);
    digitalWrite(IN_4, HIGH);
    analogWrite(ENB, robotSpeed);
}

void goBack(){
    digitalWrite(IN_1, HIGH);
    digitalWrite(IN_2, LOW);
    analogWrite(ENA, robotSpeed);

    digitalWrite(IN_3, HIGH);
    digitalWrite(IN_4, LOW);
    analogWrite(ENB, robotSpeed);
}

void goRight(){
    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, HIGH);
    analogWrite(ENA, robotSpeed);

    digitalWrite(IN_3, HIGH);
    digitalWrite(IN_4, LOW);
    analogWrite(ENB, robotSpeed);
}

void goLeft(){
    digitalWrite(IN_1, HIGH);
    digitalWrite(IN_2, LOW);
    analogWrite(ENA, robotSpeed);

    digitalWrite(IN_3, LOW);
    digitalWrite(IN_4, HIGH);
    analogWrite(ENB, robotSpeed);
}

void goForwardRight(){
    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, HIGH);
    analogWrite(ENA, robotSpeed);

    digitalWrite(IN_3, LOW);
    digitalWrite(IN_4, HIGH);
    analogWrite(ENB, robotSpeed/speed_Coeff);
}

void goForwardLeft(){
    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, HIGH);
    analogWrite(ENA, robotSpeed/speed_Coeff);

    digitalWrite(IN_3, LOW);
    digitalWrite(IN_4, HIGH);
    analogWrite(ENB, robotSpeed);
}

void goBackRight(){
    digitalWrite(IN_1, HIGH);
    digitalWrite(IN_2, LOW);
    analogWrite(ENA, robotSpeed);

    digitalWrite(IN_3, HIGH);
    digitalWrite(IN_4, LOW);
    analogWrite(ENB, robotSpeed/speed_Coeff);
}

void goBackLeft(){
    digitalWrite(IN_1, HIGH);
    digitalWrite(IN_2, LOW);
    analogWrite(ENA, robotSpeed/speed_Coeff);

    digitalWrite(IN_3, HIGH);
    digitalWrite(IN_4, LOW);
    analogWrite(ENB, robotSpeed);
}

void stopRobot() {
    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, LOW);
    analogWrite(ENA, 0);

    digitalWrite(IN_3, LOW);
    digitalWrite(IN_4, LOW);
    analogWrite(ENB, 0);
 }

 void stopAllMovement() {
    stopRobot();
    builtinLedOFF();
 }

 void connectWiFi() {
    WiFi.begin(ssid, pass);
    while(WiFi.status() != WL_CONNECTED){
      USE_SERIAL.print(".");
      delay(1000);
    }
    USE_SERIAL.println("WiFi connected");
    USE_SERIAL.println(WiFi.localIP());
 }

