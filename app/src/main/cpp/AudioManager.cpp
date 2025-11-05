#include "AudioManager.h"
#include <iostream>

AudioManager::AudioManager()
        : masterVolume(1.0f),
          soundVolume(1.0f),
          musicVolume(1.0f),
          muted(false) {
    Initialize();
}

AudioManager::~AudioManager() {
    Cleanup();
}

void AudioManager::Initialize() {
    InitAudioDevice();

    // Загрузка стандартных звуков
    LoadSound("shoot", "audio/laser_shoot.wav");
    LoadSound("explosion", "audio/explosion.wav");
    LoadSound("damage", "audio/damage.wav");
    LoadSound("level_up", "audio/level_up.wav");
    LoadSound("button_click", "audio/button_click.wav");

    LoadMusic("main_theme", "audio/main_theme.ogg");
    LoadMusic("battle_theme", "audio/battle_theme.ogg");
}

void AudioManager::Cleanup() {
    for (auto& pair : sounds) {
        // Используем ::UnloadSound, чтобы вызвать глобальную функцию Raylib
        ::UnloadSound(pair.second);
    }
    sounds.clear();

    for (auto& pair : music) {
        UnloadMusicStream(pair.second);
    }
    music.clear();

    CloseAudioDevice();
}

void AudioManager::LoadSound(const std::string& name, const std::string& filename) {
    if (sounds.find(name) != sounds.end()) {
        // Используем ::UnloadSound, чтобы вызвать глобальную функцию Raylib
        ::UnloadSound(sounds[name]);
    }

    // ИСПРАВЛЕНО: Используем ::LoadSound для вызова глобальной функции Raylib
    Sound sound = ::LoadSound(filename.c_str());
    if (sound.frameCount > 0) {
        sounds[name] = sound;
    } else {
        std::cerr << "Failed to load sound: " << filename << std::endl;
    }
}

void AudioManager::LoadMusic(const std::string& name, const std::string& filename) {
    if (music.find(name) != music.end()) {
        UnloadMusicStream(music[name]);
    }

    Music mus = LoadMusicStream(filename.c_str());
    if (mus.frameCount > 0) {
        music[name] = mus;
    } else {
        std::cerr << "Failed to load music: " << filename << std::endl;
    }
}

void AudioManager::PlaySound(const std::string& name, float volume) {
    if (muted || sounds.find(name) == sounds.end()) return;

    // ИСПРАВЛЕНО: Используем ::SetSoundVolume, чтобы избежать рекурсии
    ::SetSoundVolume(sounds[name], volume * soundVolume * masterVolume);
    // Используем ::PlaySound для вызова глобальной функции Raylib
    ::PlaySound(sounds[name]);
}

void AudioManager::PlayMusic(const std::string& name, bool loop) {
    if (muted || music.find(name) == music.end()) return;

    StopMusic();

    currentMusic = music[name];
    currentMusicName = name;

    // ИСПРАВЛЕНО: Используем ::SetMusicVolume, чтобы избежать рекурсии
    ::SetMusicVolume(currentMusic, musicVolume * masterVolume);
    PlayMusicStream(currentMusic);

    if (loop) {
        // Музыка будет зациклена автоматически через Update
    }
}

void AudioManager::StopMusic() {
    if (currentMusic.frameCount > 0) {
        StopMusicStream(currentMusic);
        currentMusic = {};
        currentMusicName.clear();
    }
}

void AudioManager::PauseMusic() {
    if (currentMusic.frameCount > 0) {
        PauseMusicStream(currentMusic);
    }
}

void AudioManager::ResumeMusic() {
    if (currentMusic.frameCount > 0) {
        ResumeMusicStream(currentMusic);
    }
}

void AudioManager::SetMasterVolume(float volume) {
    masterVolume = volume;
    UpdateVolumes();
}

void AudioManager::SetSoundVolume(float volume) {
    soundVolume = volume;
    UpdateVolumes();
}

void AudioManager::SetMusicVolume(float volume) {
    musicVolume = volume;
    UpdateVolumes();
}

void AudioManager::SetMuted(bool muted) {
    this->muted = muted;
    if (muted) {
        PauseMusic();
    } else {
        ResumeMusic();
    }
}

void AudioManager::Update() {
    if (currentMusic.frameCount > 0 && !muted) {
        UpdateMusicStream(currentMusic);

        // Автоповтор музыки
        if (!IsMusicStreamPlaying(currentMusic)) {
            PlayMusicStream(currentMusic);
        }
    }
}

void AudioManager::UpdateVolumes() {
    // Обновление громкости всех звуков
    for (auto& pair : sounds) {
        // ИСПРАВЛЕНО: Используем ::SetSoundVolume
        ::SetSoundVolume(pair.second, soundVolume * masterVolume);
    }

    // Обновление громкости музыки
    if (currentMusic.frameCount > 0) {
        // ИСПРАВЛЕНО: Используем ::SetMusicVolume
        ::SetMusicVolume(currentMusic, musicVolume * masterVolume);
    }
}