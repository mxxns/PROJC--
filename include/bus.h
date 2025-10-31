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
// Méthodes pertinentes :
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
                std::cerr << "Source with label \"" << sourceLabel << "\" not found\n";
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

        //S'assure qu'on charge bien un BUS
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
                        if (value != "BUS") {
                            std::cerr << "Error: TYPE must be 'BUS', found '" << value << "' instead." << std::endl;
                            return false;
                        }
                    }
                    else if (key == "LABEL") setLabel(value);
                    else if (key == "WIDTH") width = std::stoi(value);
                    else if (key == "SOURCE") bindSource(value);
                }
            }

            // Enregistrer le bus dans le registre si aucun composant avec ce label n'existe encore
            ReadableComponent* existing = ReadableComponentRegistry::getComponentByLabel(getLabel());
            if (existing == nullptr) {
                ReadableComponentRegistry::registerComponent(this);
            } else if (existing != this) {
                std::cerr << "A bus with label '" << getLabel() << "' is already registered\n";
            }

            return true;
        }
};

#endif