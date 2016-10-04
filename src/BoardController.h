#ifndef BOARD_CONTROLLER_H_
#define BOARD_CONTROLLER_H_

class BoardController {
    public:
        int valvePin;

        BoardController(int valvePin);
        void openValve();
        void closeValve();
        bool setId(int id);
        bool hasId();
        int getId();
};

#endif
