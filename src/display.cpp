#include "display.h"

Display::Display(int rate)
    : refreshRate(rate)
{}

Display::~Display() = default;

int Display::getRefreshRate() const {
    return refreshRate;
}

void Display::setRefreshRate(int rate) {
    refreshRate = rate;
}

void Display::bindSource(const std::string& sourceLabel) {
    source = ReadableComponentRegistry::getComponentByLabel(sourceLabel);
    if (!source) {
        std::cerr << "Source with label \"" << sourceLabel << "\" not found\n";
    }
}

std::string Display::getSourceLabel() const {
    return source ? source->getLabel() : "No source";
}

void Display::printInfo() const {
    std::cout << "DISPLAY info: "
              << " refreshRate=" << refreshRate
              << " callCounter=" << callCounter
              << " source=\"" << getSourceLabel() << "\""
              << std::endl;
}

bool Display::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << filename << std::endl;
        return false;
    }

    std::string line, key, value;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        if (std::getline(iss, key, ':') && std::getline(iss, value)) {
            key = trim(key);
            value = trim(value);

            if (key == "TYPE") {
                if (value != "DISPLAY") {
                    std::cerr << "Error: TYPE must be 'DISPLAY', found '" << value << "' instead." << std::endl;
                    return false;
                }
            } else if (key == "REFRESH") {
                setRefreshRate(std::stoi(value));
            } else if (key == "SOURCE") {
                bindSource(value);
            }
        }
    }

    return true;
}

void Display::simulate() {
    if (!source) return;

    callCounter++;
    if (callCounter < refreshRate) return;

    callCounter = 0;

    std::cout << "[DISPLAY] Source: " << getSourceLabel() << " -> ";

    while (true) {
        DataValue val = source->read();
        if (!val.valid) break;
        std::cout << val.value << " ";
    }

    std::cout << std::endl;
}
