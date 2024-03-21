//
// Created by vaige on 6.3.2024.
//

#ifndef FPSFROMSCRATCH_AUDIO_H
#define FPSFROMSCRATCH_AUDIO_H


#include <memory>
#include <map>
#include <iostream>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include "Util.h"

class AudioManager
{
public:
    static AudioManager& instance()
    {
        static std::unique_ptr<AudioManager> self;
        if (self == nullptr) {
            self = std::unique_ptr<AudioManager>(new AudioManager{});
        }
        return *self;
    }

    ~AudioManager()
    {
        alDeleteSources(1, &mSource);

        if (!alutExit()) {
            ALenum error = alutGetError();
            std::cerr << "Errors exiting alut: " << alutGetErrorString(error);
        }
    }

    [[nodiscard]] GLuint audioBuffer(const std::string& name) const
    {
        return mAudioBuffers.at(name);
    }

    static void setTransform(const Transform& transform)
    {
        alListenerfv(AL_POSITION, &transform.position[0]);
        glm::vec3 orientation[2];
        orientation[0] = transform.front();
        orientation[1] = transform.up();
        alListenerfv(AL_ORIENTATION, &orientation[0][0]);
    }

private:
    AudioManager()
    {
        if (!alutInit(nullptr, nullptr)) {
            ALenum error = alutGetError();
            throw std::runtime_error(std::string("Failed to initialize alut: ") + alutGetErrorString(error));
        }

        alGenSources(1, &mSource);

        loadBuffers();
    }

    void loadBuffers()
    {
        mAudioBuffers["ak47Fire"] = alutCreateBufferFromFile(Util::getAssetPath("audio/ak47.wav").c_str());
        mAudioBuffers["theme"] = alutCreateBufferFromFile(Util::getAssetPath("audio/halloween-theme.wav").c_str());

        ALenum error = alGetError();
        if (error != ALUT_ERROR_NO_ERROR) {
            std::cerr << "Unable to load buffers: " << alutGetErrorString(error) << std::endl;
        }
    }

    std::map<std::string, ALuint> mAudioBuffers;
    ALuint mSource{};
};


#endif //FPSFROMSCRATCH_AUDIO_H
