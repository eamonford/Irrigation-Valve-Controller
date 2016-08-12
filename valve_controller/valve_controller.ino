#include <avr/io.h>

#define MAX_MSG_LEN 3
#define FORWARD_RX_PIN A1
#define BACKWARD_RX_PIN A2
#define FORWARD_TX_PIN A3
#define BACKWARD_TX_PIN A4
#define ACK 0x10
#define DEVICE_ID_LEN 1

#define PEER 0x08
#define MYSELF 0x02
#define MASTER 0x04

// Addresses
const char master = 0x03;
const char identity = 0x04;
const char everyone = 0x01;

// Commands
const char SET_ID = 0x05;
const char OPEN_VALVE = 0x06;
const char CLOSE_VALVE = 0x07;
const char IDENTIFY = 0x08;

class Message {
  public:
    char destination;
    char command;
    char arg;
    
    Message(char destination, char command, char arg) {
      this->destination = destination;
      this->command = command;
      this->arg = arg;
    }
    
    static Message* parse(char* charArray) {
      return new Message(charArray[0], charArray[1], charArray[2]);      
    }
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}


void sendMessage(Message* msg) { 
  switch (msg->destination) {
    case identity:
      return;
    case master:
      openBackwardTx();
      break;
    default:
      openForwardTx();
  }
  Serial.write(msg->destination);
  Serial.write(msg->command);
  Serial.write(msg->arg);
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


void sendAck() {
  sendMessage(new Message(master, ACK, 0x00));
}

char* readBytes(int numBytesToRead) {
//  char message[numBytesToRead];
  char* message = (char*)malloc(sizeof(char)*numBytesToRead);
  
  int numBytesReceived = 0;
  while (numBytesReceived < numBytesToRead) {
    if (Serial.available() > 0) {
      message[numBytesReceived++] = Serial.read();
    }
  }
  return message;
}

int waitForSynAndLockPort() {
  while (true) {
    openForwardRx();
    delay(500);
    if (Serial.available()) {
      Serial.read();
      return;
    }
    openBackwardRx();
    if (Serial.available()) {
      Serial.read();
      return;
    }
  }
}

char getDestinationGroup(char destination) {
  switch (destination) {
    case everyone:
      return MYSELF | PEER;
    case master:
      return MASTER;
    case identity:
      return MYSELF;
    default:
      return PEER;  
  }
}

void processMessage(Message* message) {
  char destinationGroup = getDestinationGroup(message->destination);
  if (destinationGroup & PEER) {
    // pass it along
    sendMessage(message);
  }
  if (destinationGroup & MASTER) {
    // send it to master
    sendMessage(message);
  }
  if (destinationGroup & MYSELF) {
    switch (message->command) {
      case SET_ID:
        Serial.println("SET ID");
        break;
      case OPEN_VALVE:
        Serial.println("OPEN VALVE");
        break;
      case CLOSE_VALVE:
        Serial.println("CLOSE VALVE");
        break;
      case IDENTIFY:
        Serial.println("IDENTIFY");
        break;
      default:
        Serial.println("UNRECOGNIZED COMMAND");
    }
  }
}

void loop() {
    waitForSynAndLockPort();
    sendAck();
    char* messageBytes = readBytes(MAX_MSG_LEN);
    Message* message = Message::parse(messageBytes);
    free(messageBytes);
    processMessage(message);
    delete message;
}
