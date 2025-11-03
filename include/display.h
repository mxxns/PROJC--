#ifndef DISPLAY_H
#define DISPLAY_H

#include "lib.h"
#include <queue>
#include <sstream>
#include <fstream>
#include <string>
#include <iostream>

// ======================================================================================
//                                   DISPLAY
// Affiche les données d'une source selon refreshRate
// ======================================================================================

class Display : public Component {
private:
    int refreshRate{1};
    int callCounter{0};
    ReadableComponent* source{nullptr};

public:
    Display() = default;
    explicit Display(int rate);

    virtual ~Display();

    int getRefreshRate() const;
    void setRefreshRate(int rate);

    void bindSource(const std::string& sourceLabel);
    std::string getSourceLabel() const;

    void printInfo() const override;

    bool loadFromFile(const std::string& filename) override;

    void simulate() override;
};

#endif
