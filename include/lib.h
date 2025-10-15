#ifndef LIB_H
#define LIB_H

#include <iostream>
#include <string>

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

#endif
