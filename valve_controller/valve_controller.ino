#include <avr/io.h>

#define MSG_LENGTH 6
#define MAX_MSG_LEN 20
#define FORWARD_RX_PIN A1
#define BACKWARD_RX_PIN A2
#define FORWARD_TX_PIN A3
#define BACKWARD_TX_PIN A4
#define ACK 0x10
#define DEVICE_ID_LEN 3

#define PEER 0x08
#define MYSELF 0x02
#define MASTER 0x04

// Addresses
const char master[] = {0x03, 0x03, 0x03};
const char identity[] = {0x04, 0x04, 0x04};
const char everyone[] = {0x01, 0x01, 0x01};

// Commands
const char SET_ID[] = {0x05};
const char OPEN_VALVE[] = {0x06};
const char CLOSE_VALVE[] = {0x07};
const char IDENTIFY[] = {0x08};

class Message {
  public:
    char* destination;
    char* message;
    int messageLength;
    
    Message(char* destination, char* message, int len) {
      this->destination = destination;
      this->message = message;
      this->messageLength = len;
    }

    ~Message(void) {
      free(this->destination);
      free(this->message);
    }
    
    static Message* parse(char* charArray) {
      int totalLen = 0;
      while (charArray[totalLen] != 0x00) totalLen++;
      
      char* destination = (char*)malloc(sizeof(char)*DEVICE_ID_LEN);
      char* message = (char*)malloc(sizeof(char)*(totalLen-DEVICE_ID_LEN));
      memcpy(destination, charArray, DEVICE_ID_LEN);
      memcpy(message, charArray+DEVICE_ID_LEN, totalLen-DEVICE_ID_LEN);
      return new Message(destination, message, totalLen-DEVICE_ID_LEN);
    }
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}


void sendMessage(Message* msg) { 
  if (compareData(msg->destination, identity, DEVICE_ID_LEN)) {
    return;
  }
  if (compareData(msg->destination, master, DEVICE_ID_LEN)) {
    openBackwardTx();
  } else {
    openForwardTx();
  }
  Serial.write(msg->destination, DEVICE_ID_LEN);
  Serial.write(msg->message, msg->messageLength);
}

bool compareData(char* a, char* b, int len) {
  for (int i = 0; i < len; i++) {
    if (a[i] != b[i]) {
      return false;
    }
  }
  return true;
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
  Serial.write(ACK);
}

char* readMessage() {
  char message[MAX_MSG_LEN];
  char lastByteReceived = 1;
  int numBytesReceived = 0;
  while (true) {
    if (Serial.available() > 0) {
      lastByteReceived = Serial.read();
      message[numBytesReceived++] = lastByteReceived;
      if (lastByteReceived == 0x0 || numBytesReceived == MAX_MSG_LEN - 1) {
        return message;
      }
    }
  }
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

char getDestinationGroup(char* destination) {
  if (compareData(destination, everyone, DEVICE_ID_LEN)) {
    return MYSELF | PEER;
  }
  if (compareData(destination, master, DEVICE_ID_LEN)) {
    return MASTER;
  }
  // Is this message for me, for someone else, or for everyone?
  if (compareData(destination, identity, DEVICE_ID_LEN)) {
    return MYSELF;
  }
  return PEER;
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
    if (compareData(message->message, SET_ID, message->messageLength)) {
      Serial.println("SET ID");
    } else if (compareData(message->message, OPEN_VALVE, message->messageLength)) {
      Serial.println("OPEN VALVE");      
    } else if (compareData(message->message, CLOSE_VALVE, message->messageLength)) {
      Serial.println("CLOSE VALVE");      
    } else if (compareData(message->message, IDENTIFY, message->messageLength)) {
      Serial.println("IDENTIFY");   
    }
  }
}

void loop() {
    waitForSynAndLockPort();
    sendAck();
    char* messageBytes = readMessage();
    Message* message = Message::parse(messageBytes);
    processMessage(message);
    delete message;
}

