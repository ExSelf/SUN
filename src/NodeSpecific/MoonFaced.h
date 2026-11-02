#pragma once
#include <stdint.h>

class MoonFacedClass {
public:

    // MoonFaced Specific methods
    void setupNode(uint8_t nodeNumber);

    void executeCommand(uint8_t nodeNumber);
};

extern class MoonFacedClass MoonFaced;