#ifndef GAMECONFIG_HPP
#define GAMECONFIG_HPP

#include <map>
#include <vector>

class Property;

class TaxConfig {
private:
    int pphFlat;
    int pphPercent;
    int pbmFlat;
public:
    TaxConfig() : pphFlat(0), pphPercent(0), pbmFlat(0) {}
    TaxConfig(int pphFlat, int pphPercent, int pbmFlat) : pphFlat(pphFlat), pphPercent(pphPercent), pbmFlat(pbmFlat) {}
    int getPphFlat() const { return pphFlat; }
    int getPphPercent() const { return pphPercent; }
    int getPbmFlat() const { return pbmFlat; }
};

class SpecialConfig {
private:
    int goSalary;
    int jailFine;
public:
    SpecialConfig() : goSalary(0), jailFine(0) {}
    SpecialConfig(int goSalary, int jailFine) : goSalary(goSalary), jailFine(jailFine) {}
    int getGoSalary() const { return goSalary; }
    int getJailFine() const { return jailFine; }
};

class MiscConfig {
private:
    int maxTurn;
    int initialBalance;
public:
    MiscConfig() : maxTurn(0), initialBalance(0) {}
    MiscConfig(int maxTurn, int initialBalance) : maxTurn(maxTurn), initialBalance(initialBalance) {}
    int getMaxTurn() const { return maxTurn; }
    int getInitialBalance() const { return initialBalance; }
};

class GameConfig {
private:
    std::vector<Property*> properties;
    std::map<int, int> railroadRents;
    std::map<int, int> utilityMultipliers;
    TaxConfig tax;
    SpecialConfig special;
    MiscConfig misc;
public:
    GameConfig() {}
    
    std::vector<Property*> getProperties() const { return properties; }
    const std::map<int, int>& getRailroadRents() const { return railroadRents; }
    const std::map<int, int>& getUtilityMultipliers() const { return utilityMultipliers; }
    const TaxConfig& getTax() const { return tax; }
    const SpecialConfig& getSpecial() const { return special; }
    const MiscConfig& getMisc() const { return misc; }

    void setProperties(std::vector<Property*> p) { properties = p; }
    void setRailroadRents(std::map<int, int> rents) { railroadRents = rents; }
    void setUtilityMultipliers(std::map<int, int> multipliers) { utilityMultipliers = multipliers; }
    void setTax(TaxConfig t) { tax = t; }
    void setSpecial(SpecialConfig s) { special = s; }
    void setMisc(MiscConfig m) { misc = m; }
};

#endif