#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "raylib.h"
#include <vector>
#include <functional>

struct Particle {
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    Color color;
    Color startColor;
    Color endColor;
    float size;
    float startSize;
    float endSize;
    float rotation;
    float rotationSpeed;
    float lifetime;
    float maxLifetime;
    bool active;

    std::function<void(Particle&, float)> updateBehavior;
};

enum class ParticleEffect {
    EXPLOSION,
    ENGINE_THRUST,
    DAMAGE,
    HEALING,
    LEVEL_UP,
    RESOURCE_COLLECT
};

class ParticleSystem {
private:
    std::vector<Particle> particles;
    size_t maxParticles;
    Texture2D particleTexture;

public:
    ParticleSystem(size_t maxParticles = 10000);
    ~ParticleSystem();

    void Initialize();
    void Cleanup();

    void Update(float deltaTime);
    void Draw();

    void Emit(ParticleEffect effect, Vector2 position, int count = 1,
              Color color = WHITE, float size = 1.0f);
    void Clear();

    size_t GetActiveParticleCount() const;

private:
    void CreateExplosionEffect(Vector2 position, int count, Color color, float size);
    void CreateEngineThrustEffect(Vector2 position, int count, Color color, float size);
    void CreateDamageEffect(Vector2 position, int count, Color color, float size);
    void CreateHealingEffect(Vector2 position, int count, Color color, float size);
    void CreateLevelUpEffect(Vector2 position, int count, Color color, float size);
    void CreateResourceCollectEffect(Vector2 position, int count, Color color, float size);

    Particle CreateParticle(Vector2 position, Color startColor, Color endColor,
                            float startSize, float endSize, float lifetime);

    void UpdateExplosionParticle(Particle& particle, float deltaTime);
    void UpdateEngineParticle(Particle& particle, float deltaTime);
    void UpdateDamageParticle(Particle& particle, float deltaTime);
    void UpdateHealingParticle(Particle& particle, float deltaTime);
    void UpdateLevelUpParticle(Particle& particle, float deltaTime);
    void UpdateResourceParticle(Particle& particle, float deltaTime);
};

#endif