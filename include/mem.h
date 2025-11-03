#ifndef MEM_H__
#define MEM_H__

#include "lib.h"
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

// ======================================================================================
//                           MEMORY
// Stocke des valeurs DataValue lues depuis une source, buffer circulaire de taille capacity
// Méthodes pertinentes :
//   - simulate() cf code
//   - read() cf code
//   - printInfo() + showMemoryContent() pour debug
// ======================================================================================
class Memory : public ReadableComponent {
private:
    std::size_t capacity{1};
    int accessTime{1};
    int cycleCounter{0};
    ReadableComponent* source{nullptr};
    std::string sourceLabelStored;

    std::vector<DataValue> buffer;
    std::size_t head{0};
    std::size_t tail{0};
    std::size_t count{0};

    void pushValue(const DataValue& dv) {
        if (capacity == 0) return;
        buffer[tail] = dv;
        tail = (tail + 1) % capacity;
        if (count < capacity) {
            ++count;
        } else {
            head = tail;
        }
    }

public:
    Memory(const std::string& lbl = "MEMORY")
        : ReadableComponent(lbl)
    {
        buffer.resize(capacity);
    }

    virtual ~Memory() = default;

    void setSize(std::size_t s) {
        if (s == 0) s = 1;
        std::vector<DataValue> newbuf(s);
        std::size_t idx = head;
        for (std::size_t i = 0; i < std::min(s, count); ++i) {
            newbuf[i] = buffer[idx];
            idx = (idx + 1) % capacity;
        }
        buffer.swap(newbuf);
        capacity = s;
        head = 0;
        tail = std::min(count, s) % capacity;
        count = std::min(count, s);
    }

    void setAccessTime(int a) {
        if (a <= 0) a = 1;
        accessTime = a;
    }

    void bindSource(const std::string& lbl) {
        source = ReadableComponentRegistry::getComponentByLabel(lbl);
        if (!source) {
            std::cerr << "Source with label \"" << lbl << "\" not found\n";
        }
    }

    std::string getSourceLabel() const { return source ? source->getLabel() : "No source"; }

    //Reprise de bus.h et s'assure qu'on charge bien une mémoire
    bool loadFromFile(const std::string& filename) override{
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error : could not load " << filename << "\n";
            return false;
        }

        std::string line;
        std::string key, value, type;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::istringstream iss(line);
            if (std::getline(iss, key, ':') && std::getline(iss, value)) {
                key = trim(key);
                value = trim(value);
                if (key == "TYPE") {
                    type = value;
                    if (value != "MEMORY") {
                        std::cerr << "Error: TYPE must be 'MEM', found '" << type << "' instead." << std::endl;
                        return false;
                    }
                } else if (key == "LABEL") {
                    setLabel(value);
                } else if (key == "SIZE") {
                    try { setSize(static_cast<std::size_t>(std::stoul(value))); }
                    catch (...) { setSize(1); }
                } else if (key == "ACCESS") {
                    try { setAccessTime(std::stoi(value)); }
                    catch (...) { setAccessTime(1); }
                } else if (key == "SOURCE") {
                    sourceLabelStored = value;
                    bindSource(value);
                }
            }
        }

        // Enregistrer mémoire dans le registre de composant si aucun composant avec ce label n'existe encore
        ReadableComponent* existing = ReadableComponentRegistry::getComponentByLabel(getLabel());
        if (existing == nullptr) {
            ReadableComponentRegistry::registerComponent(this);
        } else if (existing != this) {
            std::cerr << "A memory with label '" << getLabel() << "' is already registered\n";
        }

        return true;
    }

    // simulate() réagit tous les accessTime simulations
    // Lorsqu'elle réagit, lit toutes les valeurs valides depuis la source et les stocke
    void simulate() override {
        ++cycleCounter;
        if (!source) {
            if (!sourceLabelStored.empty()) {
                source = ReadableComponentRegistry::getComponentByLabel(sourceLabelStored);
            }
        }

        if (accessTime <= 1 || (cycleCounter % accessTime) == 0) {
            if (!source) return;
            for (;;) {
                DataValue dv = source->read();
                if (!dv.valid) break;
                pushValue(dv);
            }
        }
    }

    // read() renvoie la plus ancienne DataValue stockée ou une DataValue invalide si vide
    DataValue read() override {
        if (count == 0) return DataValue{0.0, false};
        DataValue dv = buffer[head];
        head = (head + 1) % capacity;
        count--;
        return dv;
    }

    void printInfo() const override {
        std::cout << "MEMORY label=\"" << label
                  << "\" size=" << capacity
                  << " access=" << accessTime
                  << " source=\"" << (source ? source->getLabel() : (sourceLabelStored.empty() ? "No source" : sourceLabelStored)) << "\""
                  << " stored=" << count
                  << " head=" << head << " tail=" << tail
                  << std::endl;
    }

    void showMemoryContent() { //Debug supplémentaire
        std::cout << "[MEMORY DUMP] for " << getLabel() << ":\n";
        for (;;) {
            DataValue dv = read();
            if (!dv.valid) {
                std::cout << "  [no more data]\n";
                break;
            }
            std::cout << "  -> " << dv.value << "\n";
        }
    }
};

#endif