#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <cmath>

#include "../include/mem.h"
#include "../include/lib.h"

// ======================================================================================
//                           TEST MEMORY
// Inspiré du test BUS.
// Procédure :
// - Crée des FakeSource enregistrées dans le registre global
// - Charge les mémoires depuis leurs fichiers (data/memX.txt)
// - Simule plusieurs cycles pour tester le comportement selon accessTime
// - Vérifie le stockage circulaire (capacity) et la restitution dans l'ordre FIFO
// ======================================================================================

class FakeSource : public ReadableComponent {
public:
    std::vector<DataValue> seq;
    size_t idx = 0;

    FakeSource(const std::string& lbl, const std::vector<DataValue>& s)
        : ReadableComponent(lbl), seq(s)
    {
        ReadableComponentRegistry::registerComponent(this);
        std::cout << "[FakeSource] registered '" << lbl << "' (" << seq.size() << " values)\n";
    }

    DataValue read() override {
        if (idx >= seq.size()) return DataValue{0.0, false};
        DataValue dv = seq[idx++];
        std::cout << "[FakeSource:" << getLabel() << "] -> " << dv.value << "\n";
        return dv;
    }

    void simulate() override {}
    void printInfo() const override {
        std::cout << "[FakeSource] label=" << getLabel() << " remaining=" << (seq.size() - idx) << "\n";
    }
};

int main() {
    std::cout << "==================== TEST MEMORY ====================\n";

    std::vector<std::string> memFiles = {"data/mem1.txt", "data/mem2.txt"};
    std::set<std::string> sourceLabels;

    std::cout << "Reading memory files to extract SOURCE labels...\n";
    for (auto &f : memFiles) {
        std::ifstream ifs(f);
        if (!ifs.is_open()) {
            std::cerr << "Cannot open " << f << "\n";
            return 2;
        }
        std::string line;
        while (std::getline(ifs, line)) {
            auto pos = line.find(':');
            if (pos == std::string::npos) continue;
            std::string key = trim(line.substr(0, pos));
            std::string val = trim(line.substr(pos + 1));
            if (key == "SOURCE") {
                sourceLabels.insert(val);
                std::cout << "  found SOURCE: " << val << " in " << f << "\n";
            }
        }
    }

    if (sourceLabels.empty()) {
        std::cerr << "No SOURCE labels found in memory files.\n";
        return 3;
    }

    std::cout << "Creating FakeSources...\n";
    bool toggle = false;
    for (auto &lbl : sourceLabels) {
        std::vector<DataValue> seq;
        double base = toggle ? 100.0 : 1.0;
        toggle = !toggle;
        for (int i = 0; i < 10; ++i)
            seq.push_back(DataValue{base + i, true});
        new FakeSource(lbl, seq);
    }

    std::vector<Memory> memories;
    for (auto &f : memFiles) {
        std::cout << "Loading " << f << " ... ";
        Memory m;
        if (!m.loadFromFile(f)) {
            std::cerr << "FAILED\n";
            return 4;
        }
        std::cout << "OK (" << m.getLabel() << ")\n";
        memories.push_back(std::move(m));
    }

    const int totalCycles = 6;
    std::cout << "\nSimulating " << totalCycles << " cycles...\n";
    for (int cycle = 1; cycle <= totalCycles; ++cycle) {
        std::cout << "\n-- Cycle " << cycle << " --\n";
        for (auto &m : memories) {
            std::cout << "Simulating memory '" << m.getLabel() << "'\n";
            m.simulate();
            m.printInfo();
        }
    }

    for(auto &m: memories){
        m.showMemoryContent();
    }

    std::cout << "\nTEST MEMORY: completed.\n";
    return 0;
}
