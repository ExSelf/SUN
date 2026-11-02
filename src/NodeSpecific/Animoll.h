#pragma once
#include <stdint.h>

class AnimollClass {
public:

    // Snow Specific methods
    void setupNode(uint8_t nodeNumber);
    void executeCommand(uint8_t nodeNumber);
};

extern class AnimollClass Animoll;