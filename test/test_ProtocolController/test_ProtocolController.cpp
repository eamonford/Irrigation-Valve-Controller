#line 2 "test_ProtocolController.cpp"
#include <ArduinoUnit.h>
#include "ProtocolController.h"

#ifdef UNIT_TEST

test(constructor) {
    FakeStream fake1 = FakeStream();
    FakeStream fake2 = FakeStream();
    ProtocolController* controller = new ProtocolController(&fake1, &fake2);
    assertTrue(controller->getFrontComm());
    assertTrue(controller->getBackComm());
}

void setup()
{
  Serial.begin(9600);
  while(!Serial); // for the Arduino Leonardo/Micro only
}

void loop()
{
  Test::run();
}

#endif
