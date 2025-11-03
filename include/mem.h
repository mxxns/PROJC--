#ifndef MEM_H__
#define MEM_H__

#include "lib.h"

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

    void pushValue(const DataValue& dv);

public:
    Memory(const std::string& lbl = "MEMORY");
    virtual ~Memory();

    void setSize(std::size_t s);
    void setAccessTime(int a);
    void bindSource(const std::string& lbl);
    std::string getSourceLabel() const;

    bool loadFromFile(const std::string& filename) override;

    void simulate() override;
    DataValue read() override;
    void printInfo() const override;
    void showMemoryContent();
};

#endif
