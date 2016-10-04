#ifndef MESSAGE_H_
#define MESSAGE_H_

class Message {
  public:
    int destination;
    int command;
    int arg;

    Message(int destination, int command, int arg);
    bool isForId(int id);
    bool isForIdOtherThan(int id);

    static Message* parse(int* intArray) {
      return new Message(intArray[0], intArray[1], intArray[2]);
    }
};

#endif
