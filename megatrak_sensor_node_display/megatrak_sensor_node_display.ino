//************************************************************
// this is a simple example that uses the easyMesh library
//
// 1. blinks led once for every node on the mesh
// 2. blink cycle repeats every BLINK_PERIOD
// 3. sends a silly message to every node on the mesh at a random time between 1 and 5 seconds
// 4. prints anything it receives to Serial.print
//
//
//************************************************************
#include <painlessMesh.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>

#define TFT_RST -1 //for TFT I2C Connector Shield V1.0.0 and TFT 1.4 Shield V1.0.0
#define TFT_CS D4  //for TFT I2C Connector Shield V1.0.0 and TFT 1.4 Shield V1.0.0
#define TFT_DC D3  //for TFT I2C Connector Shield V1.0.0 and TFT 1.4 Shield V1.0.0

// some gpio pin that is connected to an LED...
// on my rig, this is 5, change to the right number of your LED.
// #define   LED             2       // GPIO number of connected LED, ON ESP-12 IS GPIO2

// #define   BLINK_PERIOD    3000 // milliseconds until cycle repeat
// #define   BLINK_DURATION  100  // milliseconds LED is on for

#define   MESH_SSID       "megatraksensormesh"
#define   MESH_PASSWORD   "megatraksensormeshpass"
#define   MESH_PORT       5555

#define REV_PIN D2

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

uint16_t revCounter = 0;
float revCounter2 = 0.0;

// String NODE_NAME = "node_1";

// Prototypes
void sendMessage(); 
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback(); 
void nodeTimeAdjustedCallback(int32_t offset); 
void delayReceivedCallback(uint32_t from, int32_t delay);

Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;

bool calc_delay = false;
SimpleList<uint32_t> nodes;

void sendMessage() ; // Prototype
Task taskSendMessage( TASK_SECOND * 1, TASK_FOREVER, &sendMessage ); // start with a one second interval

// Task to blink the number of nodes
// Task blinkNoNodes;
// bool onFlag = false;

void setup() {
  Serial.begin(115200);

  // pinMode(LED, OUTPUT);
  tft.initR(INITR_144GREENTAB);
  tft.setTextWrap(false); // Allow text to run off right edge
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(3);
  pinMode(REV_PIN,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(REV_PIN), revCounting, FALLING);

  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  //mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION | COMMUNICATION);  // set before init() so that you can see startup messages
  //mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION | GENERAL);  // set before init() so that you can see startup messages
  //mesh.setDebugMsgTypes(ERROR | CONNECTION | SYNC);  // set before init() so that you can see startup messages
  mesh.setDebugMsgTypes(ERROR);  // set before init() so that you can see startup messages

  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);
  // // mesh.setName(NODE_NAME);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();

  // blinkNoNodes.set(BLINK_PERIOD, (mesh.getNodeList().size() + 1) * 2, []() {
  //     // If on, switch off, else switch on
  //     if (onFlag)
  //       onFlag = false;
  //     else
  //       onFlag = true;
  //     blinkNoNodes.delay(BLINK_DURATION);

  //     if (blinkNoNodes.isLastIteration()) {
  //       // Finished blinking. Reset task for next run 
  //       // blink number of nodes (including this node) times
  //       blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
  //       // Calculate delay based on current mesh time and BLINK_PERIOD
  //       // This results in blinks between nodes being synced
  //       blinkNoNodes.enableDelayed(BLINK_PERIOD - 
  //           (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
  //     }
  // });
  // userScheduler.addTask(blinkNoNodes);
  // blinkNoNodes.enable();

  randomSeed(analogRead(A0));


}

void loop() {
  userScheduler.execute(); // it will run mesh scheduler as well
  mesh.update();
  // digitalWrite(LED, !onFlag);
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
  tft.setTextSize(3);
  tft.println(revCounter);

  tft.setCursor(0, 80);
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
  tft.setTextSize(3);
  tft.println(revCounter2);
}

void revCounting(){
  ++revCounter;
}

void sendMessage() {
  String msg = "Hello from node ";
  msg += mesh.getNodeId();
  msg += " myFreeMemory: " + String(ESP.getFreeHeap());
  mesh.sendBroadcast(msg);

  if (calc_delay) {
    SimpleList<uint32_t>::iterator node = nodes.begin();
    while (node != nodes.end()) {
      mesh.startDelayMeas(*node);
      node++;
    }
    calc_delay = false;
  }

  Serial.printf("Sending message: %s\n", msg.c_str());
  
  taskSendMessage.setInterval( random(TASK_SECOND * 1, TASK_SECOND * 5));  // between 1 and 5 seconds
}


void receivedCallback(uint32_t from, String & msg) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
  // Reset blink task
  // onFlag = false;
  // blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
  // blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
 
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());
  // Reset blink task
  // onFlag = false;
  // blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
  // blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
 
  nodes = mesh.getNodeList();

  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
  calc_delay = true;
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}
