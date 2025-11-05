#ifndef UPGRADESYSTEM_H
#define UPGRADESYSTEM_H

#include <string>
#include <unordered_map>

struct Upgrade {
    std::string name;
    std::string description;
    int cost;
    int maxLevel;
    int currentLevel;
    std::function<void()> applyUpgrade;
};

class UpgradeSystem {
private:
    std::unordered_map<std::string, Upgrade> upgrades;
    int availableResources;

public:
    UpgradeSystem();

    void AddUpgrade(const std::string& id, const Upgrade& upgrade);
    bool PurchaseUpgrade(const std::string& id);
    bool CanPurchase(const std::string& id) const;
    void AddResources(int amount);
    int GetAvailableResources() const { return availableResources; }

    const std::unordered_map<std::string, Upgrade>& GetUpgrades() const { return upgrades; }
};

#endif