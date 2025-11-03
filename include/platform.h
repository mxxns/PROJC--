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
#include <filesystem>
#include <memory>

// ======================================================================================
//                                 PLATFORM
// ======================================================================================

class Platform : public ReadableComponent {
    private:
        std::vector<CPU> cpus;
        std::vector<Memory> memories;
        std::vector<BUS> buses;
        std::vector<Display> displays;
        std::vector<Platform> platforms;
        ReadableComponentRegistry registry;
    public:
    Platform(const std::string& lbl = "PLATFORM")
        : ReadableComponent(lbl) {}
    
    virtual ~Platform() = default;

    bool loadFromFile(const std::string& filename) override{
        std::cout << "Loading platform configuration from " << filename << std::endl;

        std::ifstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Error: Could not open " << filename << std::endl;
                return false;
            }

            std::string line, pathto, toload;

            while (std::getline(file, line)) {
                if (line.empty()) continue;
                std::istringstream iss(line);
                
                std::string line, key, value, type;

                while (std::getline(file, line)) {
                    if (line.empty()) continue;
                    std::istringstream iss(line);

                    if (std::getline(iss, key, ':') && std::getline(iss, value)) {
                        key = trim(key);
                        value = trim(value);

                        if (key == "TYPE") {
                            if (value != "PLATFORM") {
                                std::cerr << "Error: TYPE must be 'PLATFORM', found '" << value << "' instead." << std::endl;
                                return false;
                            }
                        }
                        else if (key == "LABEL") setLabel(value);
                        else if (key == "COMPONENT") {
                            std::istringstream isp(value);
                            if (std::getline(isp, pathto, '/') && std::getline(isp, toload)) {
                                if (toload.find("cpu") != std::string::npos) {
                                    CPU processor;
                                    if (processor.loadFromFile(line)) {
                                        cpus.push_back(std::move(processor));
                                        registry.registerComponent(&processor);
                                    } else {
                                        std::cerr << "Error loading CPU from " << line << std::endl;
                                    }
                                }
                                else if (toload.find("mem") != std::string::npos) {
                                    Memory mem;
                                    if (mem.loadFromFile(line)) {
                                        memories.push_back(std::move(mem));
                                        registry.registerComponent(&mem);
                                    } else {
                                        std::cerr << "Error loading Memory from " << line << std::endl;
                                    }
                                }
                                else if (toload.find("bus") != std::string::npos) {
                                    BUS bus;
                                    if (bus.loadFromFile(line)) {
                                        buses.push_back(std::move(bus));
                                        registry.registerComponent(&bus);
                                    } else {
                                        std::cerr << "Error loading BUS from " << line << std::endl;
                                    }
                                }
                                else if (toload.find("display") != std::string::npos) {
                                    Display display;
                                    if (display.loadFromFile(line)) {
                                        displays.push_back(std::move(display));
                                    } else {
                                        std::cerr << "Error loading Display from " << line << std::endl;
                                    }
                                }
                                else if (toload.find("platform") != std::string::npos) {
                                    Platform subplatform;
                                    if (subplatform.loadFromFile(line)) {
                                        platforms.push_back(std::move(subplatform));
                                    } else {
                                        std::cerr << "Error loading Platform from " << line << std::endl;
                                    }
                                }
                                else {
                                    std::cerr << "Error: Unknown component type in " << toload << std::endl;
                                }
                            }
                        }
                    }
                }
                
            }

            return true;
    }

    void printInfo() const override {
        std::cout << "PLATFORM info: "
                  << " label=\"" << getLabel() << "\""
                  << " CPUs=" << cpus.size()
                  << " Memories=" << memories.size()
                  << " Buses=" << buses.size()
                  << " Displays=" << displays.size()
                  << " Subplatforms=" << platforms.size()
                  << std::endl;
    }

    DataValue read() override { //Fonction absolument inutile mais implémentée pour respecter l'interface
        return DataValue{0.0, false};
    }

    void simulate() override {
        for (auto& cpu : cpus) {
            cpu.simulate();
        }
        for (auto& mem : memories) {
            mem.simulate();
        }
        for (auto& bus : buses) {
            bus.simulate();
        }
        for (auto& display : displays) {
            display.simulate();
        }
        for (auto& platform : platforms) {
            platform.simulate();
        }
    }
};

#endif