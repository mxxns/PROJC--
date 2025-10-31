#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>

#include "../include/display.h"
#include "../include/lib.h"

// ======================================================================================
//                           TEST DISPLAY
// Inspiré des tests BUS et MEMORY.
// Procédure :
// - Crée des FakeSource enregistrées dans le registre global
// - Charge les Displays depuis leurs fichiers (data/displayX.txt)
// - Vérifie le bon chargement du TYPE, REFRESH et SOURCE
// - Simule plusieurs cycles et observe le comportement selon refreshRate
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
        if (idx >= seq.size()) {
            std::cout << "[FakeSource:" << getLabel() << "] -> INVALID\n";
            return DataValue{0.0, false};
        }
        DataValue dv = seq[idx++];
        std::cout << "[FakeSource:" << getLabel() << "] -> " << dv.value << "\n";
        return dv;
    }

    void simulate() override {}
    void printInfo() const override {
        std::cout << "[FakeSource] label=\"" << getLabel()
                  << "\" remaining=" << (seq.size() - idx) << "\n";
    }
};

int main() {
    std::cout << "==================== TEST DISPLAY ====================\n";

    // Liste des fichiers Display à charger
    std::vector<std::string> displayFiles = {"data/display.txt"};
    std::set<std::string> sourceLabels;

    std::cout << "Reading display files to extract SOURCE labels...\n";
    for (auto &f : displayFiles) {
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
        std::cerr << "No SOURCE labels found in display files.\n";
        return 3;
    }

    std::cout << "Creating FakeSources...\n";
    bool toggle = false;
    for (auto &lbl : sourceLabels) {
        std::vector<DataValue> seq;
        double base = toggle ? 100.0 : 1.0;
        toggle = !toggle;
        for (int i = 0; i < 5; ++i)
            seq.push_back(DataValue{base + i, true});
        new FakeSource(lbl, seq);
    }

    // Charger les displays
    std::vector<Display> displays;
    for (auto &f : displayFiles) {
        std::cout << "\nLoading " << f << " ... ";
        Display d;
        if (!d.loadFromFile(f)) {
            std::cerr << "FAILED\n";
            return 4;
        }
        std::cout << "OK\n";
        std::cout << "  Source: " << d.getSourceLabel()
                  << ", RefreshRate=" << d.getRefreshRate() << "\n";
        displays.push_back(std::move(d));
    }

    // Simulation de quelques cycles
    const int totalCycles = 45;
    std::cout << "\nSimulating " << totalCycles << " cycles...\n";
    for (int cycle = 1; cycle <= totalCycles; ++cycle) {
        std::cout << "\n-- Cycle " << cycle << " --\n";
        for (auto &d : displays) {
            std::cout << "[Cycle " << cycle << "] Simulating display (refreshRate="
                      << d.getRefreshRate() << ")\n";
            d.simulate();
        }
    }

    std::cout << "\nTEST DISPLAY: completed successfully.\n";
    return 0;
}
