#ifndef BUS_H__
#define BUS_H__

#include "lib.h"
#include <queue>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

// ======================================================================================
//                           BUS
// Lien entre tous les Component
// Méthodes pertinentes :
//   - simulate() : déplace les données pending -> ready et lit la source
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
    BUS(const std::string& lbl = "BUS");
    virtual ~BUS();

    void bindSource(const std::string& sourceLabel);
    std::string getSourceLabel() const;

    void simulate() override;
    DataValue read() override;
    void printInfo() const override;

    bool loadFromFile(const std::string& filename) override;
};

#endif
