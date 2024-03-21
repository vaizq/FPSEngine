//
// Created by vaige on 6.3.2024.
//

#ifndef FPSFROMSCRATCH_AUDIOSOURCE_H
#define FPSFROMSCRATCH_AUDIOSOURCE_H

#include "AudioManager.h"
#include "Transform.h"


class AudioSource
{
public:
    AudioSource()
    {
        alGenSources(1, &mID);
    }

    ~AudioSource()
    {
        alDeleteSources(1, &mID);
    }

    void playAudio(const std::string& name, bool loop = false) const
    {
        alSourcei(mID, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
        alSourcei(mID, AL_BUFFER, AudioManager::instance().audioBuffer(name));
        alSourcePlay(mID);
    }

    void stopAudio() const
    {
        if (isPlaying()) {
            alSourceStop(mID);
        }
    }


    void setTransform(const Transform& transform) const
    {
        alGetError();
        alSourcefv(mID, AL_POSITION, &transform.position[0]);
        glm::vec3 orientation[2];
        orientation[0] = transform.front();
        orientation[1] = transform.right();
        alSourcefv(mID, AL_ORIENTATION, &orientation[0][0]);

        ALenum error = alGetError();
        if (error != AL_NO_ERROR) {
            displayAlError("Unable set transform", error);
        }
    }

    [[nodiscard]] bool isPlaying() const
    {
        ALint state{};
        alGetSourcei(mID, AL_SOURCE_STATE, &state);
        return state == AL_PLAYING;
    }

private:
    static void displayAlError(const std::string_view message, ALenum error)
    {
        std::cerr << message << " : " << alGetString(error) << '\n';
    }

    ALuint mID{};
};


#endif //FPSFROMSCRATCH_AUDIOSOURCE_H
