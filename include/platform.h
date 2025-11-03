#ifndef PLATFORM_H
#define PLATFORM_H

#include "lib.h"
#include "cpu.h"
#include "bus.h"
#include "mem.h"
#include "display.h"

#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <iostream>

// ======================================================================================
//                                 PLATFORM
// ======================================================================================

class Platform : public ReadableComponent {
private:
    std::vector<std::unique_ptr<CPU>> cpus;
    std::vector<std::unique_ptr<Memory>> memories;
    std::vector<std::unique_ptr<BUS>> buses;
    std::vector<std::unique_ptr<Display>> displays;
    std::vector<std::unique_ptr<Platform>> platforms;
    ReadableComponentRegistry registry;

public:
    Platform(const std::string& lbl = "PLATFORM");
    virtual ~Platform();

    ReadableComponentRegistry& getRegistry();

    bool loadFromFile(const std::string& filename) override;
    void printInfo() const override;
    DataValue read() override;
    void simulate() override;
};

#endif
