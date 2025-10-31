#ifndef LIB_H
#define LIB_H

#include <iostream>
#include <string>
#include <vector>

// ======================================================================================
//                           DataValue
// Structure pour représenter une valeur de donnée avec son état de validité
// Utilisée pour les ReadableComponent
// ======================================================================================
struct DataValue {
    double value;
    bool valid;

    DataValue(double v = 0.0, bool ok = false)
        : value(v), valid(ok) {}
};

// ======================================================================================
//                           Component
// Classe de base, abstraite, que tous les composants dont tous les components hériteront
// ======================================================================================
class Component {
public:
    virtual ~Component() = default; //Destructeur virtuel pour une meilleure gestion de la mémoire

    virtual void simulate() = 0; // Méthode virtuelle pure que tous les components doivent implémenter

    virtual void printInfo() const = 0; //Utile pour debug, "const" permet de s'assurer que la méthode ne modifie pas l'objet
};

// ======================================================================================
//                        ReadableComponent
// Hérite de Component, représente tout composant avec une méthode read()
// ======================================================================================
class ReadableComponent : public Component {
protected:
    std::string label; //Privé pour éviter modification extérieure

public:
    ReadableComponent(const std::string& lbl = "")
        : label(lbl) {} //Constructeur avec possibilité d'initialiser le label dès l'instanciation
                        //<< const std::string &lbl = "" >> permet d'initialiser aussi sans nom si pas d'argument
    //Utilisation : ReadableComponent Composant("LabelComposant");

    virtual ~ReadableComponent() = default;

    std::string getLabel() const { return label; }

    void setLabel(const std::string& lbl) { label = lbl; }

    virtual DataValue read() = 0;

    void printInfo() const override = 0;
    //PrintInfo reste virtuelle pure et sera à implémenter pour chaque classe dérivée
};

// ======================================================================================
//                        Registre de ReadableComponent
// Recense tous les ReadableComponent créés pour permettre leur recherche par label
// Usage : lors de la construction d'un ReadableComponent, l'enregistrer dans la registry
//         via ReadableComponentRegistry::registerComponent(this);
//         pour retrouver un composant par son label, utiliser
//         ReadableComponentRegistry::getComponentByLabel(label); (BUS en a besoin)
// ======================================================================================
class ReadableComponentRegistry {
    private:
        static inline std::vector<ReadableComponent*> registry;

    public:
        static void registerComponent(ReadableComponent* comp) {
            registry.push_back(comp);
        }
        static ReadableComponent* getComponentByLabel(const std::string& lbl) {
            for (auto comp : registry) {
                if (comp->getLabel() == lbl) {
                    return comp;
                }
            }
            return nullptr; // Retourne nullptr si aucun composant trouvé
        }

};

// ======================================================================================
//                        Fonctions utilitaires
// ======================================================================================

// Trim : supprime les espaces en début et fin de chaîne
static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");
    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}


#endif
