#ifndef COMBATCOMPONENT_H
#define COMBATCOMPONENT_H

#include <string>

class CombatComponent {
private:
    int damage;
    float attackSpeed;
    float attackRange;
    std::string ownerType;
    uint32_t ownerId;
    float lastAttackTime;

public:
    CombatComponent(int damage = 10, float attackSpeed = 1.0f,
                    float attackRange = 100.0f, const std::string& ownerType = "",
                    uint32_t ownerId = 0);

    bool CanAttack() const;
    void Attack();
    void SetOwner(const std::string& type, uint32_t id);

    // Геттеры
    int GetDamage() const { return damage; }
    float GetAttackSpeed() const { return attackSpeed; }
    float GetAttackRange() const { return attackRange; }
    std::string GetOwnerType() const { return ownerType; }
    uint32_t GetOwnerId() const { return ownerId; }

    // Сеттеры для улучшений
    void UpgradeDamage(int bonus) { damage += bonus; }
    void UpgradeAttackSpeed(float multiplier) { attackSpeed *= multiplier; }
    void UpgradeAttackRange(float bonus) { attackRange += bonus; }
};

#endif