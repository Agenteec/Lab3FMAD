//
// Created by Admin on 04.11.2025.
//
#include "UpgradeSystem.h"

UpgradeSystem::UpgradeSystem() : availableResources(0) {
    // Базовая инициализация улучшений
}

void UpgradeSystem::AddUpgrade(const std::string& id, const Upgrade& upgrade) {
    upgrades[id] = upgrade;
}

bool UpgradeSystem::PurchaseUpgrade(const std::string& id) {
    auto it = upgrades.find(id);
    if (it != upgrades.end() && CanPurchase(id)) {
        Upgrade& upgrade = it->second;
        availableResources -= upgrade.cost;
        upgrade.currentLevel++;
        if (upgrade.applyUpgrade) {
            upgrade.applyUpgrade();
        }
        return true;
    }
    return false;
}

bool UpgradeSystem::CanPurchase(const std::string& id) const {
    auto it = upgrades.find(id);
    if (it != upgrades.end()) {
        const Upgrade& upgrade = it->second;
        return availableResources >= upgrade.cost && upgrade.currentLevel < upgrade.maxLevel;
    }
    return false;
}

void UpgradeSystem::AddResources(int amount) {
    availableResources += amount;
}