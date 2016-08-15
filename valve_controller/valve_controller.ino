#include <avr/io.h>
#include <EEPROM.h>

#define SOFTWARE_VERSION "001"
#define MAX_MSG_LEN 3
#define FORWARD_RX_PIN A1
#define BACKWARD_RX_PIN A2
#define FORWARD_TX_PIN A3
#define BACKWARD_TX_PIN A4
#define DEVICE_ID_LEN 1

#define SYN 0x09
#define ACK 0x10

#define IDENTITY_ADDRESS 32

// Addresses
const int MASTER = 0x01;
const int EVERYONE = 0x00;

// Commands
const int SET_ID = 0x05;
const int OPEN_VALVE = 0x06;
const int CLOSE_VALVE = 0x07;
const int IDENTIFY = 0x08;

class Message {
  public:
    int destination;
    int command;
    int arg;
    
    Message(int destination, int command, int arg) {
      this->destination = destination;
      this->command = command;
      this->arg = arg;
    }
    
    static Message* parse(int* intArray) {
      return new Message(intArray[0], intArray[1], intArray[2]);      
    }
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

bool hasId() {
  return ((int)EEPROM.read(IDENTITY_ADDRESS + 1) == SOFTWARE_VERSION[0] &&
          (int)EEPROM.read(IDENTITY_ADDRESS + 2) == SOFTWARE_VERSION[1] &&
          (int)EEPROM.read(IDENTITY_ADDRESS + 3) == SOFTWARE_VERSION[2]);
}

bool setId(int id) {
  EEPROM.write(IDENTITY_ADDRESS, id);
  EEPROM.write(IDENTITY_ADDRESS + 1, SOFTWARE_VERSION[0]);
  EEPROM.write(IDENTITY_ADDRESS + 2, SOFTWARE_VERSION[1]);
  EEPROM.write(IDENTITY_ADDRESS + 3, SOFTWARE_VERSION[2]);

  return getId() == id;
}

int getId() {
  return EEPROM.read(IDENTITY_ADDRESS);
}

void sendMessage(Message* msg, int identity) {
  if (identity != 0 && msg->destination == identity)
    return;
  else if (msg->destination == MASTER)
    openBackwardTx();
  else
    openForwardTx();
  if (sendSynAndWaitForAck()) {
    Serial.write(msg->destination);
    Serial.write(msg->command);
    Serial.write(msg->arg);
  }
}

void openForwardRx() {
  digitalWrite(FORWARD_TX_PIN, HIGH);
  digitalWrite(BACKWARD_TX_PIN, LOW);
}

void openBackwardRx() {
  digitalWrite(FORWARD_TX_PIN, LOW);
  digitalWrite(BACKWARD_TX_PIN, HIGH);
}

void openForwardTx() {
  digitalWrite(FORWARD_RX_PIN, HIGH);
  digitalWrite(BACKWARD_RX_PIN, LOW);
}

void openBackwardTx() {
  digitalWrite(FORWARD_TX_PIN, LOW);
  digitalWrite(BACKWARD_TX_PIN, HIGH);
}

int* readBytes(int numBytesToRead) {
//  int message[numBytesToRead];
  int* message = (int*)malloc(sizeof(int)*numBytesToRead);
  
  int numBytesReceived = 0;
  while (numBytesReceived < numBytesToRead) {
    if (Serial.available() > 0) {
      message[numBytesReceived++] = Serial.read();
    }
  }
  return message;
}

bool sendSynAndWaitForAck() {
  int tryCount = 0;
  do {
    Serial.write(SYN);
    delay(200);
    if (tryCount == 10) {
      return false;
    }
    tryCount++;
  } while (!Serial.available() || Serial.read() != ACK);
  return true;
}

int waitForSynAndSendAck() {
  bool forward = true;
  do {
    if (forward) {
      openForwardRx();
    } else {
      openBackwardRx();
    }
    forward = !forward;
    delay(300);
  } while (!Serial.available() || Serial.read() != SYN);
  Serial.write(ACK);
}

bool messageIsForMe(int destination, int id) {
  return destination == id || destination == EVERYONE;
}

bool shouldForwardMessage(int destination, int id) {
  return destination == EVERYONE || destination == MASTER || destination != id;
}

void executeCommand(Message* message, int id) {
  switch (message->command) {
      case SET_ID:
        if (setId(message->arg)) {
          sendMessage(new Message(MASTER, 0x01, message->arg), id);
        } else {
          sendMessage(new Message(MASTER, 0x01, 0x00), id);
        }
        break;
      case OPEN_VALVE:
        Serial.println("OPEN VALVE");
        break;
      case CLOSE_VALVE:
        Serial.println("CLOSE VALVE");
        break;
      case IDENTIFY:
          sendMessage(new Message(MASTER, 0x01, id), id);
        break;
      default:
        Serial.println("UNRECOGNIZED COMMAND");
    }
}

void processMessage(Message* message) {
  int id = hasId() ? getId() : 0;
  if (messageIsForMe(message->destination, id)) {
    executeCommand(message, id);
  }
  if (shouldForwardMessage(message->destination, id)) {
    sendMessage(message, id);
  }
}

// Won't speak unless spoken to.
void loop() {
    waitForSynAndSendAck();
    int* messageBytes = readBytes(MAX_MSG_LEN);
    Message* message = Message::parse(messageBytes);
    free(messageBytes);
    processMessage(message);
    delete message;
}
