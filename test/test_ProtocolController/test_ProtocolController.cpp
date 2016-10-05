#line 2 "test_ProtocolController.cpp"
#include <ArduinoUnit.h>
#include "ProtocolController.h"
#include "constants.h"

#ifdef UNIT_TEST

test(constructor) {
    FakeStream fake1 = FakeStream();
    FakeStream fake2 = FakeStream();
    ProtocolController* controller = new ProtocolController(&fake1, &fake2);
    assertTrue(controller->getFrontComm());
    assertTrue(controller->getBackComm());
}

test(readBytes) {
    FakeStream fakeStream = FakeStream();
    ProtocolController* controller = new ProtocolController(&fakeStream, NULL);
    fakeStream.write(1);
    fakeStream.write(2);
    fakeStream.write(3);
    int* bytes = controller->readBytes(&fakeStream, 3);
    assertEqual(bytes[0], 1);
    assertEqual(bytes[1], 2);
    assertEqual(bytes[2], 3);
}

test(sendSynAndWaitForAck_withResponse) {
    FakeStream fakeStream = FakeStream();
    fakeStream.write(ACK);
    ProtocolController* controller = new ProtocolController(&fakeStream, NULL);
    // There will already be an ACK waiting, but whatever
    assertTrue(controller->sendSynAndWaitForAck(&fakeStream));
}

test(sendSynAndWaitForAck_noResponse) {
    FakeStream fakeStream = FakeStream();
    ProtocolController* controller = new ProtocolController(&fakeStream, NULL);
    assertFalse(controller->sendSynAndWaitForAck(&fakeStream));
}

test(waitForSynAndSendAck_should_write_ACK_and_eat_2_extra_SYNs_and_return_fakeStream2) {
    FakeStream fakeStream1 = FakeStream();
    FakeStream fakeStream2 = FakeStream();
    ProtocolController* controller = new ProtocolController(&fakeStream1, &fakeStream2);
    fakeStream2.write(SYN);
    fakeStream2.write(SYN);
    fakeStream2.write(SYN);

    Stream* activeStream = controller->waitForSynAndSendAck();
    assertEqual((int)activeStream, (int)(&fakeStream2));
    assertEqual(fakeStream2.available(), 1);
    assertEqual(fakeStream2.read(), ACK);
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
