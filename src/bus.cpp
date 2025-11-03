#include "bus.h"

BUS::BUS(const std::string& lbl)
    : ReadableComponent(lbl)
{}

BUS::~BUS() = default;

void BUS::bindSource(const std::string& sourceLabel) {
    if (sourceLabel == getLabel()) {
        std::cerr << "Error: BUS '" << label << "' cannot bind to itself as source.\n";
        source = nullptr;
        return;
    }

    source = ReadableComponentRegistry::getComponentByLabel(sourceLabel);
    if (!source) {
        std::cerr << "Source with label \"" << sourceLabel << "\" not found\n";
    }
}

std::string BUS::getSourceLabel() const {
    return source ? source->getLabel() : "No source";
}

void BUS::simulate() {
    // Étape 1 : déplacer pending -> ready
    while (!pending.empty()) {
        ready.push(pending.front());
        pending.pop();
    }

    if (!source) return;

    // Étape 2 : lecture de la source
    for (int i = 0; i < width; ++i) {
        source->read();
        DataValue data = source->read();
        if (!data.valid) break; // arrêt si donnée invalide
        pending.push(data);
    }
}

DataValue BUS::read() {
    if (ready.empty()) return DataValue(0.0, false);
    DataValue data = ready.front();
    ready.pop();
    readCount++;
    return data;
}

void BUS::printInfo() const {
    std::cout << "BUS label=\"" << label
              << "\" width=" << width
              << " source=\"" << getSourceLabel()
              << "\" ready=" << ready.size()
              << " pending=" << pending.size()
              << " reads=" << readCount
              << std::endl;

    std::queue<DataValue> copy = ready;  // copie pour ne pas détruire l'original
    std::cout << "Ready: ";
    while (!copy.empty()) {
        const DataValue& dv = copy.front();
        std::cout << dv.value << " ";
        copy.pop();
    }
    std::cout << std::endl;
    std::queue<DataValue> copyPending = pending;  // copie pour ne pas détruire l'original
    std::cout << "Pending: ";
    while (!copyPending.empty()) {
        const DataValue& dv = copyPending.front();
        std::cout << dv.value << " ";
        copyPending.pop();
    }
    std::cout << std::endl;
}

bool BUS::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);

        std::string key, value;
        if (std::getline(iss, key, ':') && std::getline(iss, value)) {
            key = trim(key);
            value = trim(value);

            if (key == "TYPE") {
                if (value != "BUS") {
                    std::cerr << "Error: TYPE must be 'BUS', found '" << value << "' instead." << std::endl;
                    return false;
                }
            } else if (key == "LABEL") {
                setLabel(value);
            } else if (key == "WIDTH") {
                width = std::stoi(value);
            } else if (key == "SOURCE") {
                bindSource(value);
            }
        }
    }
    return true;
}
