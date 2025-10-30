#ifndef BUS_H__
#define BUS_H__

#include "lib.h"
#include <queue>
#include <fstream>

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

        void bindSource(ReadableComponent* src) {
            source = src;
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

        void printInfo() const override {
            std::cout << "BUS label=\"" << label
                      << "\" width=" << width
                      << " source=\"" << getSourceLabel()
                      << "\" ready=" << ready.size()
                      << " pending=" << pending.size()
                      << " reads=" << readCount
                      << std::endl;
        }

        bool loadFile(std::string& filepath){ 
            std::ifstream file(filepath);
            if (!file.is_open()) {
                std::cerr << "Error: Could not open " << filepath << std::endl;
                return false;
            }

            std::string line, key, value, type;

            


        }
};

#endif