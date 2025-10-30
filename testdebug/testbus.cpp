#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <cmath>

#include "../include/bus.h"
#include "../include/lib.h"

// ======================================================================================
//                           TEST BUS
// Généré à l'aide de l'IA pour aller plus vite
// Verbose pour faciliter le debug
// Procédure :
// Créé le ReadableComponentRegistry et ReadableComponent (FakeSource) pour simuler des sources
// Lit les fichiers de bus pour extraire les labels des sources
// Crée des FakeSource pour chaque label extrait
// Charge chaque BUS depuis son fichier
// Simule deux cycles de chaque BUS
// Lit toutes les valeurs prêtes de chaque BUS et vérifie l'ordre et la continuité
// ======================================================================================


static std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

// Fake source implémentant ReadableComponent pour les tests.
// On suppose l'existence de ReadableComponentRegistry::registerComponent.
class FakeSource : public ReadableComponent {
public:
    std::vector<DataValue> seq;
    size_t idx = 0;

    FakeSource(const std::string& lbl, const std::vector<DataValue>& s)
        : ReadableComponent(lbl), seq(s)
    {
        std::cout << "[FakeSource] creating source '" << lbl << "' with " << seq.size() << " values\n";
        ReadableComponentRegistry::registerComponent(this);
        std::cout << "[FakeSource] registered source '" << lbl << "'\n";
    }

    DataValue read() override {
        if (idx >= seq.size()) {
            // debug print commented out to avoid too much spam; enable if needed
            // std::cout << "[FakeSource:" << getLabel() << "] read -> INVALID\n";
            return DataValue{0.0, false};
        }
        DataValue dv = seq[idx++];
        std::cout << "[FakeSource:" << getLabel() << "] read -> " << dv.value << " (valid)\n";
        return dv;
    }

    void simulate() override {
        // pas de comportement de simulation pour la source factice
    }

    void printInfo() const override {
        std::cout << "[FakeSource] label=\"" << getLabel() << "\" remaining=" << (seq.size() - idx) << "\n";
    }
};

int main() {
    std::cout << "TESTBUS: start\n";

    std::vector<std::string> busFiles = {"data/bus1.txt", "data/bus2.txt"};
    std::set<std::string> sourceLabels;

    std::cout << "Reading bus files to extract SOURCE labels...\n";
    // 1) Extraire les SOURCE: des fichiers bus existants
    for (auto &f : busFiles) {
        std::cout << " - open " << f << " ... ";
        std::ifstream ifs(f);
        if (!ifs.is_open()) {
            std::cerr << "Cannot open " << f << "\n";
            return 2;
        }
        std::cout << "OK\n";
        std::string line;
        while (std::getline(ifs, line)) {
            auto pos = line.find(':');
            if (pos == std::string::npos) continue;
            std::string key = trim(line.substr(0, pos));
            std::string val = trim(line.substr(pos + 1));
            if (key == "SOURCE") {
                std::cout << "   found SOURCE: '" << val << "' in " << f << "\n";
                sourceLabels.insert(val);
            }
        }
    }

    if (sourceLabels.empty()) {
        std::cerr << "No SOURCE fields found in bus files\n";
        return 3;
    }

    std::cout << "Unique source labels found:\n";
    for (auto &s : sourceLabels) std::cout << "  * " << s << "\n";

    // 2) Créer des FakeSource pour chaque label extrait
    std::unordered_map<std::string, double> baseForLabel;
    double base1 = 1.0;
    double base2 = 10.0;
    bool useFirst = true;
    for (auto &lbl : sourceLabels) {
        double base = useFirst ? base1 : base2;
        useFirst = !useFirst;
        baseForLabel[lbl] = base;
        std::vector<DataValue> seq;
        for (int i = 0; i < 20; ++i) seq.push_back(DataValue{base + i, true});
        // crée la source factice (elle s'enregistre dans la registry)
        new FakeSource(lbl, seq); // leak intentionnel pour test short-lived
    }

    // DEBUG: vérifier que la registry contient bien les composants créés
    std::cout << "DEBUG: checking registry entries for extracted source labels...\n";
    for (const auto &lbl : sourceLabels) {
        ReadableComponent* found = ReadableComponentRegistry::getComponentByLabel(lbl);
        std::cout << "  look up '" << lbl << "': "
                  << (found ? "FOUND" : "NOT FOUND");
        if (found) std::cout << " (address=" << static_cast<const void*>(found) << ", getLabel()='" << found->getLabel() << "')";
        std::cout << "\n";
    }

    // 3) Charger chaque BUS depuis son fichier et simuler
    std::vector<BUS> buses;
    std::cout << "Loading BUS from files...\n";
    for (auto &f : busFiles) {
        std::cout << " - loading " << f << " ... ";
        BUS b; // constructeur par défaut qui prend label "BUS"
        bool okLoad = false;
        // try both possible function names (loadFromFile) to be robust
        // assume BUS::loadFromFile exists
        okLoad = b.loadFromFile(f);
        if (!okLoad) {
            std::cerr << "FAILED to load " << f << "\n";
            return 4;
        }
        std::cout << "OK\n";
        std::cout << "   BUS label after load: '" << b.getLabel() << "'\n";
        std::cout << "   BUS bound source (currently): '" << b.getSourceLabel() << "'\n";
        buses.push_back(std::move(b));
    }

    // 4) Simuler deux cycles (comme décrit dans la spec)
    std::cout << "Simulating buses (2 cycles each)...\n";
    for (size_t i = 0; i < buses.size(); ++i) {
        auto &b = buses[i];
        std::cout << " > BUS[" << i << "] '" << b.getLabel() << "' - cycle 1\n";
        b.simulate(); // lecture -> pending
        std::cout << " > BUS[" << i << "] '" << b.getLabel() << "' - cycle 2\n";
        b.simulate(); // pending -> ready + lecture
    }

    // 5) Lire toutes les valeurs prêtes et vérifier ordre/continuité
    std::cout << "Reading ready values from buses...\n";
    bool ok = true;
    for (size_t bi = 0; bi < buses.size(); ++bi) {
        auto &b = buses[bi];
        std::vector<double> got;
        std::cout << " - BUS[" << bi << "] '" << b.getLabel() << "' (source '" << b.getSourceLabel() << "')\n";
        for (;;) {
            DataValue dv = b.read();
            if (!dv.valid) {
                std::cout << "    read -> INVALID (no more ready values)\n";
                break;
            }
            std::cout << "    read -> " << dv.value << "\n";
            got.push_back(dv.value);
            if (got.size() > 1000) {
                std::cout << "    safety cap reached\n";
                break;
            }
        }
        if (got.empty()) {
            std::cerr << "BUS #" << bi << " returned no ready values (unexpected)\n";
            ok = false;
            continue;
        }
        // vérifier que les valeurs sont consécutives d'environ 1.0
        for (size_t i = 1; i < got.size(); ++i) {
            double diff = got[i] - got[i-1];
            if (std::fabs(diff - 1.0) > 1e-6) {
                std::cerr << "BUS #" << bi << " values not consecutive: " << got[i-1] << ", " << got[i] << "\n";
                ok = false;
                break;
            }
        }
    }

    if (ok) {
        std::cout << "TEST PASS\n";
        return 0;
    } else {
        std::cout << "TEST FAIL\n";
        return 1;
    }
}