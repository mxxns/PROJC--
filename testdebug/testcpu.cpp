#include "cpu.h"
#include <iostream>
#include <fstream>
#include <cassert>

// Fonction utilitaire pour créer un fichier de programme de test
void createTestProgram(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "ADD 5.0 3.0\n";
        file << "SUB 10.0 2.0\n";
        file << "MUL 4.0 2.5\n";
        file << "DIV 8.0 2.0\n";
        file << "ADD 1.0 1.0\n";
        file.close();
        std::cout << "Fichier programme cree: " << filename << std::endl;
    } else {
        std::cerr << "Erreur: Impossible de creer le fichier programme" << std::endl;
    }
}

// Fonction utilitaire pour créer un fichier de configuration CPU
void createCPUConfig(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "TYPE: CPU\n";
        file << "LABEL: CPU_Test\n";
        file << "N_CORES: 4\n";
        file << "FREQUENCY: 1000\n";
        file.close();
        std::cout << "Fichier configuration CPU cree: " << filename << std::endl;
    } else {
        std::cerr << "Erreur: Impossible de creer le fichier configuration" << std::endl;
    }
}

// Test 1: Test des instructions individuelles
void testInstructions() {
    std::cout << "\n=== Test 1: Instructions individuelles ===" << std::endl;
    
    Instruction add(ADD, 5.0, 3.0);
    Instruction sub(SUB, 10.0, 2.0);
    Instruction mul(MUL, 4.0, 2.5);
    Instruction div(DIV, 8.0, 2.0);
    Instruction nop(NOP);
    
    std::cout << "ADD 5.0 + 3.0 = " << add.compute() << " (attendu: 8.0)" << std::endl;
    std::cout << "SUB 10.0 - 2.0 = " << sub.compute() << " (attendu: 8.0)" << std::endl;
    std::cout << "MUL 4.0 * 2.5 = " << mul.compute() << " (attendu: 10.0)" << std::endl;
    std::cout << "DIV 8.0 / 2.0 = " << div.compute() << " (attendu: 4.0)" << std::endl;
    std::cout << "NOP = " << nop.compute() << " (attendu: 0.0)" << std::endl;
    
    // Vérifications
    assert(add.compute() == 8.0);
    assert(sub.compute() == 8.0);
    assert(mul.compute() == 10.0);
    assert(div.compute() == 4.0);
    assert(nop.compute() == 0.0);
    
    std::cout << "Tous les tests d'instructions passes!" << std::endl;
}

// Test 2: Test du registre FIFO
void testRegister() {
    std::cout << "\n=== Test 2: Registre FIFO ===" << std::endl;
    
    Register reg;
    
    // Test push et peek
    reg.push(DataValue(1.0, true));
    reg.push(DataValue(2.0, true));
    reg.push(DataValue(3.0, true));
    
    const DataValue* peeked = reg.peek();
    if (peeked) {
        std::cout << "Peek: " << peeked->value << " (attendu: 1.0)" << std::endl;
        assert(peeked->value == 1.0);
    }
    
    // Test pop
    DataValue val1 = reg.pop();
    DataValue val2 = reg.pop();
    
    std::cout << "Pop 1: " << val1.value << " (attendu: 1.0)" << std::endl;
    std::cout << "Pop 2: " << val2.value << " (attendu: 2.0)" << std::endl;
    
    assert(val1.value == 1.0);
    assert(val2.value == 2.0);
    
    // Vérifier le peek après pops
    peeked = reg.peek();
    if (peeked) {
        std::cout << "Peek apres pops: " << peeked->value << " (attendu: 3.0)" << std::endl;
        assert(peeked->value == 3.0);
    }
    
    std::cout << "Tous les tests de registre passes!" << std::endl;
}

// Test 3: Test du programme
void testProgram() {
    std::cout << "\n=== Test 3: Programme ===" << std::endl;
    
    Program prog;
    createTestProgram("test_program.txt");
    prog.load("test_program.txt");
    
    // Exécuter toutes les instructions
    for (int i = 0; i < 5; ++i) {
        Instruction instr = prog.compute();
        if (instr.opcode != NOP) {
            double result = instr.compute();
            std::cout << "Instruction " << i << " resultat: " << result << std::endl;
        }
    }
    
    // Test reset
    prog.reset();
    Instruction firstAfterReset = prog.compute();
    double result = firstAfterReset.compute();
    std::cout << "Premiere instruction apres reset: " << result << " (attendu: 8.0)" << std::endl;
    assert(result == 8.0); // ADD 5.0 + 3.0 = 8.0
    
    std::cout << "Tous les tests de programme passes!" << std::endl;
}

// Test 4: Test complet du CPU
void testCPU() {
    std::cout << "\n=== Test 4: CPU Complet ===" << std::endl;
    
    // Créer les fichiers de test
    createTestProgram("cpu_program.txt");
    createCPUConfig("cpu_config.txt");
    
    // Créer et configurer le CPU
    CPU cpu;
    
    // Charger la configuration
    if (cpu.loadFromFile("cpu_config.txt")) {
        std::cout << "Configuration CPU chargee avec succes" << std::endl;
    }
    
    // Charger le programme
    cpu.loadProgram("cpu_program.txt");
    std::cout << "Programme charge dans le CPU" << std::endl;
    
    // Afficher les infos du CPU
    cpu.printInfo();
    
    // Exécuter la simulation
    std::cout << "\nExecution de la simulation..." << std::endl;
    cpu.simulate();
    
    // Lire les résultats des registres
    std::cout << "\nLecture des resultats depuis les registres:" << std::endl;
    for (int i = 0; i < 5; ++i) {
        DataValue result = cpu.read();
        if (result.valid) {
            std::cout << "Resultat " << i << ": " << result.value << std::endl;
        }
    }
    
    std::cout << "Test CPU complet reussi!" << std::endl;
}

// Test 5: Test avec division par zéro
void testDivisionByZero() {
    std::cout << "\n=== Test 5: Division par zero ===" << std::endl;
    
    // Créer un programme avec division par zéro
    std::ofstream file("div_zero_program.txt");
    if (file.is_open()) {
        file << "DIV 5.0 0.0\n";
        file << "ADD 2.0 3.0\n";
        file.close();
    }
    
    Program prog;
    prog.load("div_zero_program.txt");
    
    // Exécuter les instructions
    Instruction instr1 = prog.compute();
    double result1 = instr1.compute(); // Doit afficher un message d'erreur mais retourner 0.0
    
    Instruction instr2 = prog.compute();
    double result2 = instr2.compute();
    
    std::cout << "Division par zero resultat: " << result1 << " (attendu: 0.0 avec erreur)" << std::endl;
    std::cout << "Instruction suivante résultat: " << result2 << " (attendu: 5.0)" << std::endl;
    
    assert(result1 == 0.0);
    assert(result2 == 5.0);
    
    std::cout << "Test division par zéro reussi!" << std::endl;
}

int main() {
    std::cout << "=== Debut du Testbench CPU ===" << std::endl;
    
    try {
        testInstructions();
        testRegister();
        testProgram();
        testCPU();
        testDivisionByZero();
        
        std::cout << "\n Tous les tests ont ete passes avec succes!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Erreur pendant les tests: " << e.what() << std::endl;
        return 1;
    }
    
    // Nettoyage des fichiers temporaires
    remove("test_program.txt");
    remove("cpu_program.txt");
    remove("cpu_config.txt");
    remove("div_zero_program.txt");
    
    std::cout << "=== Fin du Testbench CPU ===" << std::endl;
    return 0;
}