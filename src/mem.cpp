#include "mem.h"
#include "lib.h"
#include <algorithm>

Memory::Memory(const std::string& lbl)
    : ReadableComponent(lbl)
{
    buffer.resize(capacity);
}

Memory::~Memory() = default;

void Memory::pushValue(const DataValue& dv) {
    if (capacity == 0) return;
    buffer[tail] = dv;
    tail = (tail + 1) % capacity;
    if (count < capacity) {
        ++count;
    } else {
        head = tail;
    }
}

void Memory::setSize(std::size_t s) {
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

void Memory::setAccessTime(int a) {
    if (a <= 0) a = 1;
    accessTime = a;
}

void Memory::bindSource(const std::string& lbl) {
    if (lbl == getLabel()) {
        std::cerr << "Error: BUS '" << label << "' cannot bind to itself as source.\n";
        source = nullptr;
        return;
    }

    source = ReadableComponentRegistry::getComponentByLabel(lbl);
    if (!source) {
        std::cerr << "Source with label \"" << lbl << "\" not found\n";
    }
}

std::string Memory::getSourceLabel() const {
    return source ? source->getLabel() : (sourceLabelStored.empty() ? "No source" : sourceLabelStored);
}

bool Memory::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[MEM] Error : could not load " << filename << "\n";
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
    return true;
}

void Memory::simulate() {
    ++cycleCounter;
    if (!source && !sourceLabelStored.empty()) {
        source = ReadableComponentRegistry::getComponentByLabel(sourceLabelStored);
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

// ========================= Read =========================
DataValue Memory::read() {
    if (count == 0) return DataValue{0.0, false};
    DataValue dv = buffer[head];
    head = (head + 1) % capacity;
    count--;
    return dv;
}

// ========================= Print Info =========================
void Memory::printInfo() const {
    std::cout << "MEMORY label=\"" << label
              << "\" size=" << capacity
              << " access=" << accessTime
              << " source=\"" << getSourceLabel() << "\""
              << " stored=" << count
              << " head=" << head << " tail=" << tail
              << std::endl;
}

// ========================= Show Memory Content =========================
void Memory::showMemoryContent() {
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
