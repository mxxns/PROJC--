#include "platform.h"

// ========================= Constructor / Destructor =========================
Platform::Platform(const std::string& lbl)
    : ReadableComponent(lbl)
{}

Platform::~Platform() = default;

// ========================= Registry Access =========================
ReadableComponentRegistry& Platform::getRegistry() {
    return registry;
}

// ========================= Load from File =========================
bool Platform::loadFromFile(const std::string& filename) {
    std::cout << "Loading platform configuration from " << filename << std::endl;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, ':') && std::getline(iss, value)) {
            key = trim(key);
            value = trim(value);

            if (key == "TYPE") {
                if (value != "PLATFORM") {
                    std::cerr << "Error: TYPE must be 'PLATFORM', found '" << value << "' instead." << std::endl;
                    return false;
                }
            } else if (key == "LABEL") {
                setLabel(value);
            } else if (key == "COMPONENT") {
                std::string pathto, toload;

                std::istringstream isp(value);
                if (std::getline(isp, pathto, '/') && std::getline(isp, toload)) {
                    if (toload.find("cpu") != std::string::npos) {
                        auto processor = std::make_unique<CPU>();
                        if (processor->loadFromFile(value)) {
                            registry.registerComponent(processor.get());
                            cpus.push_back(std::move(processor));
                        } else {
                            std::cerr << "Error loading CPU from " << line << std::endl;
                        }
                    } else if (toload.find("mem") != std::string::npos) {
                        auto mem = std::make_unique<Memory>();
                        if (mem->loadFromFile(value)) {
                            registry.registerComponent(mem.get());
                            memories.push_back(std::move(mem));
                        } else {
                            std::cerr << "Error loading Memory from " << line << std::endl;
                        }
                    } else if (toload.find("bus") != std::string::npos) {
                        auto bus = std::make_unique<BUS>();
                        if (bus->loadFromFile(value)) {
                            registry.registerComponent(bus.get());
                            buses.push_back(std::move(bus));
                        } else {
                            std::cerr << "Error loading BUS from " << line << std::endl;
                        }
                    } else if (toload.find("display") != std::string::npos) {
                        auto display = std::make_unique<Display>();
                        if (display->loadFromFile(value)) {
                            displays.push_back(std::move(display));
                        } else {
                            std::cerr << "Error loading Display from " << line << std::endl;
                        }
                    } else if (toload.find("platform") != std::string::npos) {
                        auto subplatform = std::make_unique<Platform>();
                        if (subplatform->loadFromFile(value)) {
                            platforms.push_back(std::move(subplatform));
                        } else {
                            std::cerr << "Error loading Platform from " << line << std::endl;
                        }
                    } else {
                        std::cerr << "Error: Unknown component type in " << toload << std::endl;
                    }
                }
            }
        }
    }

    return true;
}

// ========================= Print Info =========================
void Platform::printInfo() const {
    std::cout << "PLATFORM info: "
              << " label=\"" << getLabel() << "\""
              << " CPUs=" << cpus.size()
              << " Memories=" << memories.size()
              << " Buses=" << buses.size()
              << " Displays=" << displays.size()
              << " Subplatforms=" << platforms.size()
              << std::endl;
}

// ========================= Read =========================
DataValue Platform::read() {
    // Implémentation dummy pour respecter l'interface
    return DataValue{0.0, false};
}

// ========================= Simulate =========================
void Platform::simulate() {
    for (auto& cpu : cpus) cpu->simulate();
    for (auto& mem : memories) mem->simulate();
    for (auto& bus : buses) bus->simulate();
    for (auto& display : displays) display->simulate();
    for (auto& platform : platforms) platform->simulate();
}
