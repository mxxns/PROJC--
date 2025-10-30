#ifndef BUS_H__
#define BUS_H__

#include "lib.h"
#include <queue>
#include <sstream>
#include <fstream>
#include <string>

// ======================================================================================
//                           BUS
// Lien entre tous les Component
// Méthodes :
//   - bindSource(label) : lie une source ReadableComponent au BUS via son label,
//                        utilise ReadableComponentRegistry pour retrouver le pointeur
//   - simulate() : cf. code
//   - read() : lit une donnée prête depuis le BUS
//   - printInfo() : affiche les informations du BUS
// ======================================================================================

class BUS : public ReadableComponent {
    private:
        int width{1};
        int readCount{0};

        ReadableComponent* source{nullptr};

        std::queue<DataValue> pending;
        std::queue<DataValue> ready;

    public:
        BUS(const std::string& lbl = "BUS")
            : ReadableComponent(lbl) {} //Constructeur, label par défaut "BUS"

        virtual ~BUS() = default;

        void bindSource(const std::string& sourceLabel) {
            source = ReadableComponentRegistry::getComponentByLabel(sourceLabel);
            if (!source) {
                std::cerr << "BUS::bindSource: source with label \"" << sourceLabel << "\" not found\n";
            }
        }

        std::string getSourceLabel() const { return source ? source->getLabel() : "No source"; }

        void simulate() override {
            while(!pending.empty()) {//Etape 1 de la simu. : mouvement pending -> ready
                ready.push(pending.front());
                pending.pop();
            }

            if(!source) return;

            for(int i = 0; i < width; ++i) { //Etape 2 de la simu. : lecture de la source
                DataValue data = source->read();
                if (!data.valid) break; //Si donnée invalide, on arrête la lecture
                pending.push(data);
            }
        }

        DataValue read() override {
            if (ready.empty()) return DataValue(0.0, false); //Si aucune donnée prête, on retourne un DV invalide
            //Sinon,
            DataValue data = ready.front();
            ready.pop();
            readCount++;
            return data;
        }

        void printInfo() const override {
            std::cout << "BUS label=\"" << label
                      << "\" width=" << width
                      << " source=\"" << getSourceLabel()
                      << "\" ready=" << ready.size()
                      << " pending=" << pending.size()
                      << " reads=" << readCount
                      << std::endl;
        }
    
        static std::string trim(const std::string& s) {
            size_t start = s.find_first_not_of(" \t");
            size_t end = s.find_last_not_of(" \t");
            if (start == std::string::npos) return "";
            return s.substr(start, end - start + 1);
        }

        bool loadFromFile(const std::string& filename) {
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
                        type = value;
                        if (value != "BUS") {
                            std::cerr << "Error: TYPE must be 'BUS', found '" << type << "' instead." << std::endl;
                            return false;
                        }
                    }
                    else if (key == "LABEL") setLabel(value);
                    else if (key == "WIDTH") width = std::stoi(value);
                    else if (key == "SOURCE") bindSource(value);
                }
            }

            return true;
        }
};

#endif