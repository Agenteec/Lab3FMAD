#include "CombatComponent.h"
#include "raylib.h"

CombatComponent::CombatComponent(int damage, float attackSpeed,
                                 float attackRange, const std::string& ownerType,
                                 uint32_t ownerId)
        : damage(damage),
          attackSpeed(attackSpeed),
          attackRange(attackRange),
          ownerType(ownerType),
          ownerId(ownerId),
          lastAttackTime(0.0f) {
}

bool CombatComponent::CanAttack() const {
    float currentTime = GetTime();
    return (currentTime - lastAttackTime) >= (1.0f / attackSpeed);
}

void CombatComponent::Attack() {
    lastAttackTime = GetTime();
}

void CombatComponent::SetOwner(const std::string& type, uint32_t id) {
    ownerType = type;
    ownerId = id;
}