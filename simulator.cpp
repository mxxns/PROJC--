#include "lib.h"
#include "platform.h"
#include "cpu.h"
#include "mem.h"
#include "bus.h"
#include "display.h"

// ======================================================================================
//                                 MAIN SIMULATOR
// ======================================================================================

const std::string RED    = "\033[31m";
const std::string GREEN  = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string RESET  = "\033[0m";
const std::string BLUE = "\033[34m";

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << RED << "Usage: " << argv[0] << " <platform_config_file>" << RESET << std::endl;
        return 1;
    }

    std::string configFile = argv[1];
    std::cout << "Config file: " << configFile << std::endl;
    Platform mainPlatform("NotLoadedPlatform");

    if (!mainPlatform.loadFromFile(configFile)) {
        std::cerr << RED << "Error: Failed to load platform configuration." << RESET << std::endl;
        return 1;
    }

    std::cout << GREEN << "Platform configuration loaded successfully, platform loaded: " 
              << mainPlatform.getLabel() << RESET << std::endl;

    int cycles{1};
    std::cout << YELLOW << "Enter number of simulation cycles: " << RESET;
    std::cin >> cycles;

    for(int i = 0; i < cycles; ++i) {
        std::cout << YELLOW << "=== Cycle " << (i + 1) << " ===" << RESET << std::endl;
        mainPlatform.simulate();
    }

    std::cout << GREEN << "Simulation completed after " << cycles << " cycles." << RESET << std::endl;
    std::cout << "Final Platform State:" << BLUE << std::endl;
    mainPlatform.printInfo();
    std::cout << RESET << std::endl;

    std::cout << BLUE << std::endl;
    auto& registry = mainPlatform.getRegistry();
    if (registry.isEmpty()) {
        std::cerr << RED << "Warning: No components registered in the platform!" << RESET << std::endl;
    } else {
        registry.printAllComponents();
    } std::cout << RESET << std::endl;

    return 0;
}