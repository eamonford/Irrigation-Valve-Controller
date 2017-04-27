#ifndef _HARDWARE_CONTROLLER_H_
#define _HARDWARE_CONTROLLER_H_

class HardwareController {
private:
  int valveControlPin;
  
public:
  HardwareController(int valveControlPin);
  bool hasId();
  int getIdOrElse(int otherOption);
  bool setId(int id);
  bool datagramIsForMe(int destination);
  void openValve();
  void closeValve();
};

#endif
