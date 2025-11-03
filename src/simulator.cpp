#include "lib.h"
#include "platform.h"
#include "cpu.h"
#include "mem.h"
#include "bus.h"
#include "display.h"

// ======================================================================================
//                                 MAIN SIMULATOR
// ======================================================================================

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <platform_config_file>" << std::endl;
        return 1;
    }

    std::string configFile = argv[1];
    Platform mainPlatform;

    if (!mainPlatform.loadFromFile(configFile)) {
        std::cerr << "Failed to load platform configuration." << std::endl;
        return 1;
    }

    std::cout << "Platform configuration loaded successfully, platform loaded : "<< mainPlatform.getLabel() << std::endl;

    int cycles{1};
    std::cout << "Enter number of simulation cycles : " << std::endl;
    std::cin >> cycles;

    for(int i = 0; i < cycles; ++i) {
        mainPlatform.simulate();
    }

    std::cout << "Simulation completed after " << cycles << " cycles." << std::endl;
    std::cout << "Final Platform State:" << std::endl;
    mainPlatform.printInfo();
    std::cout << "Registered Components:" << std::endl;
    mainPlatform.getRegistry().printAllComponents();

    return 0;
}