#pragma once
#include <stdint.h>

class OrigamiClass {
public:

    // Origami Specific methods
    void setupNode(uint8_t nodeNumber);

    void executeCommand(uint8_t nodeNumber);
};

extern class OrigamiClass Origami;