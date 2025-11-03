#ifndef DISPLAY_H
#define DISPLAY_H

#include "lib.h"
#include <queue>
#include <sstream>
#include <fstream>
#include <string>

// ======================================================================================
//                                   DISPLAY
// Affiche
// ======================================================================================
class Display : public Component {
private:
    int refreshRate{1};
    int callCounter{0};
    ReadableComponent* source{nullptr};
public:
    Display() = default;
    explicit Display(int rate) : refreshRate(rate) {}
    
    virtual ~Display() = default;

    int getRefreshRate() const { return refreshRate; }
    void setRefreshRate(int rate) { refreshRate = rate; }

    void bindSource(const std::string& sourceLabel) {
        source = ReadableComponentRegistry::getComponentByLabel(sourceLabel);
        if (!source) {
            std::cerr << "Source with label \"" << sourceLabel << "\" not found\n";
        }
    }

    std::string getSourceLabel() const { return source ? source->getLabel() : "No source"; }
    
    void printInfo() const override {
        std::cout << "DISPLAY info: "
                  << "\" refreshRate=" << refreshRate
                  << " callCounter=" << callCounter
                  << " source=\"" << getSourceLabel() << "\""
                  << std::endl;
    }

    bool loadFromFile(const std::string& filename) override{
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Error: Could not open " << filename << std::endl;
                return false;
            }

            std::string line, key, value, type;

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
                    }
                    else if (key == "REFRESH") setRefreshRate(stoi(value));
                    else if (key == "SOURCE") bindSource(value);
                }
            }

            return true;
    }

    void simulate() override {
        if (!source)
            return;

        callCounter++;
        if (callCounter < refreshRate)
            return;

        callCounter = 0;

        std::cout << "[DISPLAY] Source: " << getSourceLabel() << " -> ";

        while (true) {
            DataValue val = source->read();
            if (!val.valid)
                break;
            std::cout << val.value << " ";
        }

        std::cout << std::endl;
    }
};

#endif
