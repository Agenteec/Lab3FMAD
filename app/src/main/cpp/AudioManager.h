#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include "raylib.h"
#include <unordered_map>
#include <string>

class AudioManager {
private:
    std::unordered_map<std::string, Sound> sounds;
    std::unordered_map<std::string, Music> music;

    float masterVolume;
    float soundVolume;
    float musicVolume;
    bool muted;

    Music currentMusic;
    std::string currentMusicName;

public:
    AudioManager();
    ~AudioManager();

    void Initialize();
    void Cleanup();

    void LoadSound(const std::string& name, const std::string& filename);
    void LoadMusic(const std::string& name, const std::string& filename);

    void PlaySound(const std::string& name, float volume = 1.0f);
    void PlayMusic(const std::string& name, bool loop = true);
    void StopMusic();
    void PauseMusic();
    void ResumeMusic();

    void SetMasterVolume(float volume);
    void SetSoundVolume(float volume);
    void SetMusicVolume(float volume);
    void SetMuted(bool muted);

    void Update();

    float GetMasterVolume() const { return masterVolume; }
    float GetSoundVolume() const { return soundVolume; }
    float GetMusicVolume() const { return musicVolume; }
    bool IsMuted() const { return muted; }

private:
    void UpdateVolumes();
};

#endif