#include <AltSoftSerial.h>

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

AltSoftSerial backPort;

// Addresses
const int MASTER = 0x01;
const int EVERYONE = 0x00;
const int FIRST_UNIDENTIFIED = 0x02;

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

AltSoftSerial* activePort;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  backPort.begin(9600);
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
  if (identity != 0x00 && msg->destination == identity)
    return;
  else if (msg->destination == MASTER)
    openBackwardTx();
  else
    openForwardTx();
  if (sendSynAndWaitForAck()) {
    activePort->write(msg->destination);
    activePort->write(msg->command);
    activePort->write(msg->arg);
  }
}

void openForwardRx() {
    activePort = &((AltSoftSerial&)Serial);
}

void openBackwardRx() {
    activePort = &backPort;
}

void openForwardTx() {
    activePort = &((AltSoftSerial&)Serial);
}

void openBackwardTx() {
    activePort = &backPort;
}

int* readBytes(int numBytesToRead) {
//  int message[numBytesToRead];
  int* message = (int*)malloc(sizeof(int)*numBytesToRead);
  
  int numBytesReceived = 0;
  while (numBytesReceived < numBytesToRead) {
    if (activePort->available() > 0) {
      message[numBytesReceived++] = activePort->read();
    }
  }
  return message;
}

bool sendSynAndWaitForAck() {
  int tryCount = 0;
  do {
    activePort->write(SYN);
    delay(300);
    if (tryCount == 30) {
      return false;
    }
    tryCount++;
  } while (activePort->available() == 0 || activePort->read() != ACK);
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
    delay(100);
  } while (activePort->available() == 0 || activePort->read() != SYN);
  activePort->write(ACK);
}

bool messageIsForMe(int destination, int id) {
  return (id == 0x00 && destination == FIRST_UNIDENTIFIED) ||
         destination == id || 
         destination == EVERYONE;
}

bool shouldForwardMessage(int destination, int id) {
  return (destination == EVERYONE || 
         destination == MASTER || 
         destination != id) && !(id == 0x00 && destination == FIRST_UNIDENTIFIED);
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
        activePort->println("OPEN VALVE");
        break;
      case CLOSE_VALVE:
        activePort->println("CLOSE VALVE");
        break;
      case IDENTIFY:
          sendMessage(new Message(MASTER, 0x01, id), id);
        break;
      default:
        activePort->println("UNRECOGNIZED COMMAND");
    }
}

void processMessage(Message* message) {
  int id = hasId() ? getId() : 0x00;
  if (messageIsForMe(message->destination, id)) {
    executeCommand(message, id);
  }
  if (shouldForwardMessage(message->destination, id)) {
    sendMessage(message, id);
  }
}

// Won't speak unless spoken to.
void loop() {
  while (true) {
    waitForSynAndSendAck();
    int* messageBytes = readBytes(MAX_MSG_LEN);
    Message* message = Message::parse(messageBytes);
    free(messageBytes);
    processMessage(message);
    delete message;
  }
}
