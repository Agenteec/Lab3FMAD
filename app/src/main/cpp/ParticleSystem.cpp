#include "ParticleSystem.h"
#include <cmath>
#include <algorithm>

ParticleSystem::ParticleSystem(size_t maxParticles)
        : maxParticles(maxParticles) {
    particles.resize(maxParticles);
    Initialize();
}

ParticleSystem::~ParticleSystem() {
    Cleanup();
}

void ParticleSystem::Initialize() {
    // Создаем простую текстуру для частиц (белый квадрат)
    Image particleImage = GenImageColor(32, 32, WHITE);
    particleTexture = LoadTextureFromImage(particleImage);
    UnloadImage(particleImage);

    // Инициализируем все частицы как неактивные
    for (auto& particle : particles) {
        particle.active = false;
    }
}

void ParticleSystem::Cleanup() {
    if (particleTexture.id != 0) {
        UnloadTexture(particleTexture);
    }
}

void ParticleSystem::Update(float deltaTime) {
    for (auto& particle : particles) {
        if (!particle.active) continue;

        particle.lifetime += deltaTime;

        if (particle.lifetime >= particle.maxLifetime) {
            particle.active = false;
            continue;
        }

        // Обновление физики
        particle.velocity.x += particle.acceleration.x * deltaTime;
        particle.velocity.y += particle.acceleration.y * deltaTime;
        particle.position.x += particle.velocity.x * deltaTime;
        particle.position.y += particle.velocity.y * deltaTime;
        particle.rotation += particle.rotationSpeed * deltaTime;

        // Интерполяция размера
        float lifeRatio = particle.lifetime / particle.maxLifetime;
        particle.size = particle.startSize + (particle.endSize - particle.startSize) * lifeRatio;

        // Интерполяция цвета
        particle.color.r = particle.startColor.r + (particle.endColor.r - particle.startColor.r) * lifeRatio;
        particle.color.g = particle.startColor.g + (particle.endColor.g - particle.startColor.g) * lifeRatio;
        particle.color.b = particle.startColor.b + (particle.endColor.b - particle.startColor.b) * lifeRatio;
        particle.color.a = particle.startColor.a + (particle.endColor.a - particle.startColor.a) * lifeRatio;

        // Кастомное поведение
        if (particle.updateBehavior) {
            particle.updateBehavior(particle, deltaTime);
        }
    }
}

void ParticleSystem::Draw() {
    for (const auto& particle : particles) {
        if (!particle.active) continue;

        Rectangle sourceRect = {0, 0, 32, 32};
        Rectangle destRect = {
                particle.position.x,
                particle.position.y,
                particle.size,
                particle.size
        };
        Vector2 origin = {particle.size / 2, particle.size / 2};

        DrawTexturePro(particleTexture, sourceRect, destRect, origin,
                       particle.rotation, particle.color);
    }
}

void ParticleSystem::Emit(ParticleEffect effect, Vector2 position, int count,
                          Color color, float size) {
    switch (effect) {
        case ParticleEffect::EXPLOSION:
            CreateExplosionEffect(position, count, color, size);
            break;
        case ParticleEffect::ENGINE_THRUST:
            CreateEngineThrustEffect(position, count, color, size);
            break;
        case ParticleEffect::DAMAGE:
            CreateDamageEffect(position, count, color, size);
            break;
        case ParticleEffect::HEALING:
            CreateHealingEffect(position, count, color, size);
            break;
        case ParticleEffect::LEVEL_UP:
            CreateLevelUpEffect(position, count, color, size);
            break;
        case ParticleEffect::RESOURCE_COLLECT:
            CreateResourceCollectEffect(position, count, color, size);
            break;
    }
}

void ParticleSystem::CreateExplosionEffect(Vector2 position, int count, Color color, float size) {
    for (int i = 0; i < count; i++) {
        for (auto& particle : particles) {
            if (!particle.active) {
                particle = CreateParticle(position, color, Fade(color, 0.0f),
                                          size * 2.0f, 0.0f, 1.0f);

                // Случайное направление и скорость
                float angle = GetRandomValue(0, 360) * DEG2RAD;
                float speed = GetRandomValue(50, 200);
                particle.velocity = {
                        cosf(angle) * speed,
                        sinf(angle) * speed
                };

                // Замедление
                particle.acceleration = {
                        -particle.velocity.x * 2.0f,
                        -particle.velocity.y * 2.0f
                };

                particle.rotationSpeed = GetRandomValue(-180, 180);
                particle.updateBehavior = [this](Particle& p, float dt) {
                    UpdateExplosionParticle(p, dt);
                };
                break;
            }
        }
    }
}

void ParticleSystem::CreateEngineThrustEffect(Vector2 position, int count, Color color, float size) {
    for (int i = 0; i < count; i++) {
        for (auto& particle : particles) {
            if (!particle.active) {
                particle = CreateParticle(position, color, Fade(color, 0.0f),
                                          size, 0.0f, 0.5f);

                // Направление вниз с небольшим разбросом
                float angleVariation = GetRandomValue(-30, 30) * DEG2RAD;
                particle.velocity = {
                        sinf(angleVariation) * 50.0f,
                        cosf(angleVariation) * 100.0f
                };

                particle.rotationSpeed = GetRandomValue(-90, 90);
                particle.updateBehavior = [this](Particle& p, float dt) {
                    UpdateEngineParticle(p, dt);
                };
                break;
            }
        }
    }
}

// Реализация остальных эффектов...

Particle ParticleSystem::CreateParticle(Vector2 position, Color startColor, Color endColor,
                                        float startSize, float endSize, float lifetime) {
    return Particle{
            position,
            {0, 0},
            {0, 0},
            startColor,
            startColor,
            endColor,
            startSize,
            startSize,
            endSize,
            0.0f,
            0.0f,
            0.0f,
            lifetime,
            true,
            nullptr
    };
}

void ParticleSystem::UpdateExplosionParticle(Particle& particle, float deltaTime) {
    // Дополнительное поведение для взрыва
    float lifeRatio = particle.lifetime / particle.maxLifetime;
    if (lifeRatio > 0.5f) {
        particle.color.a = (unsigned char)(255 * (1.0f - (lifeRatio - 0.5f) * 2.0f));
    }
}

void ParticleSystem::UpdateEngineParticle(Particle& particle, float deltaTime) {
    // Частицы двигателя быстро исчезают
    float lifeRatio = particle.lifetime / particle.maxLifetime;
    particle.color.a = (unsigned char)(255 * (1.0f - lifeRatio));
}

void ParticleSystem::Clear() {
    for (auto& particle : particles) {
        particle.active = false;
    }
}

size_t ParticleSystem::GetActiveParticleCount() const {
    return std::count_if(particles.begin(), particles.end(),
                         [](const Particle& p) { return p.active; });
}
void ParticleSystem::CreateDamageEffect(Vector2 position, int count, Color color, float size) {
    for (int i = 0; i < count; i++) {
        for (auto& particle : particles) {
            if (!particle.active) {
                particle = CreateParticle(position, color, Fade(RED, 0.0f),
                                          size * 1.5f, 0.0f, 0.6f);

                float angle = GetRandomValue(0, 360) * DEG2RAD;
                float speed = GetRandomValue(80, 200);
                particle.velocity = {
                        cosf(angle) * speed,
                        sinf(angle) * speed
                };

                particle.rotationSpeed = GetRandomValue(-360, 360);
                particle.updateBehavior = [this](Particle& p, float dt) {
                    UpdateDamageParticle(p, dt);
                };
                break;
            }
        }
    }
}

void ParticleSystem::CreateHealingEffect(Vector2 position, int count, Color color, float size) {
    for (int i = 0; i < count; i++) {
        for (auto& particle : particles) {
            if (!particle.active) {
                particle = CreateParticle(position, GREEN, Fade(GREEN, 0.0f),
                                          size, 0.0f, 1.2f);

                float angle = GetRandomValue(0, 360) * DEG2RAD;
                float speed = GetRandomValue(20, 60);
                particle.velocity = {
                        cosf(angle) * speed,
                        sinf(angle) * speed
                };

                particle.acceleration = {0, -30}; // Всплывающий эффект
                particle.rotationSpeed = GetRandomValue(-180, 180);
                particle.updateBehavior = [this](Particle& p, float dt) {
                    UpdateHealingParticle(p, dt);
                };
                break;
            }
        }
    }
}

void ParticleSystem::CreateLevelUpEffect(Vector2 position, int count, Color color, float size) {
    for (int i = 0; i < count; i++) {
        for (auto& particle : particles) {
            if (!particle.active) {
                particle = CreateParticle(position, YELLOW, Fade(GOLD, 0.0f),
                                          size * 2.0f, size * 0.5f, 2.0f);

                // Спиральный эффект
                float baseAngle = GetRandomValue(0, 360) * DEG2RAD;
                particle.velocity = {0, 0};
                particle.acceleration = {0, 0};

                particle.updateBehavior = [&](Particle& p, float dt) {
                    float time = p.lifetime;
                    float radius = 50.0f * (1.0f - time/p.maxLifetime);
                    float angle = baseAngle + time * 5.0f;

                    p.position.x = position.x + cosf(angle) * radius;
                    p.position.y = position.y + sinf(angle) * radius;
                    UpdateLevelUpParticle(p, dt);
                };
                break;
            }
        }
    }
}

void ParticleSystem::CreateResourceCollectEffect(Vector2 position, int count, Color color, float size) {
    for (int i = 0; i < count; i++) {
        for (auto& particle : particles) {
            if (!particle.active) {
                particle = CreateParticle(position, GOLD, Fade(YELLOW, 0.0f),
                                          size, 0.0f, 0.8f);

                // Движение к центру
                Vector2 toCenter = {GetScreenWidth()/2 - position.x, GetScreenHeight()/2 - position.y};
                float length = sqrtf(toCenter.x * toCenter.x + toCenter.y * toCenter.y);
                if (length > 0) {
                    toCenter.x /= length;
                    toCenter.y /= length;
                }

                particle.velocity = {
                        toCenter.x * 150.0f,
                        toCenter.y * 150.0f
                };

                particle.updateBehavior = [this](Particle& p, float dt) {
                    UpdateResourceParticle(p, dt);
                };
                break;
            }
        }
    }
}

void ParticleSystem::UpdateDamageParticle(Particle& particle, float deltaTime) {
    // Быстрое затухание
    float lifeRatio = particle.lifetime / particle.maxLifetime;
    particle.color.a = (unsigned char)(255 * (1.0f - lifeRatio * lifeRatio));
}

void ParticleSystem::UpdateHealingParticle(Particle& particle, float deltaTime) {
    // Медленное всплытие и затухание
    float lifeRatio = particle.lifetime / particle.maxLifetime;
    particle.color.a = (unsigned char)(255 * (1.0f - lifeRatio));
}

void ParticleSystem::UpdateLevelUpParticle(Particle& particle, float deltaTime) {
    // Мерцание и пульсация
    float lifeRatio = particle.lifetime / particle.maxLifetime;
    float pulse = sinf(particle.lifetime * 10.0f) * 0.3f + 0.7f;
    particle.color.a = (unsigned char)(255 * (1.0f - lifeRatio) * pulse);
}

void ParticleSystem::UpdateResourceParticle(Particle& particle, float deltaTime) {
    // Ускорение к центру и исчезновение
    float lifeRatio = particle.lifetime / particle.maxLifetime;
    particle.velocity.x *= 1.1f;
    particle.velocity.y *= 1.1f;
    particle.color.a = (unsigned char)(255 * (1.0f - lifeRatio));
}