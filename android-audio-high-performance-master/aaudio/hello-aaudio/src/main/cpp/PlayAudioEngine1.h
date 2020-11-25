//
// Created by rajat on 25/11/20.
//

#ifndef AAUDIO_PLAYAUDIOENGINE1_H
#define AAUDIO_PLAYAUDIOENGINE1_H
#include <thread>
#include "audio_common.h"
#include "SineGenerator.h"
#define BUFFER_SIZE_AUTOMATIC 0


class PlayAudioEngine1 {
public:
    PlayAudioEngine1();
    ~PlayAudioEngine1();
    void setDeviceId(int32_t deviceId);
    void setToneOn(bool isToneOn);
    void setBufferSizeInBursts(int32_t numBursts);
    aaudio_data_callback_result_t dataCallback(AAudioStream *stream,
                                               void *audioData,
                                               int32_t numFrames);
    void errorCallback(AAudioStream *stream,
                       aaudio_result_t  __unused error);
    aaudio_data_callback_result_t dataCallback1(AAudioStream *stream,
                                                void *audioData,
                                                int32_t numFrames);
    void errorCallback1(AAudioStream *stream,
                        aaudio_result_t  __unused error);
    aaudio_data_callback_result_t dataCallback2(AAudioStream *stream,
                                                void *audioData,
                                                int32_t numFrames);
    void errorCallback2(AAudioStream *stream,
                        aaudio_result_t  __unused error);
    aaudio_data_callback_result_t dataCallback3(AAudioStream *stream,
                                                void *audioData,
                                                int32_t numFrames);
    void errorCallback3(AAudioStream *stream,
                        aaudio_result_t  __unused error);
    double getCurrentOutputLatencyMillis();

private:

    int32_t playbackDeviceId_ = AAUDIO_UNSPECIFIED;
    int32_t sampleRate_;
    int16_t sampleChannels_;
    aaudio_format_t sampleFormat_;

    SineGenerator *sineOscLeft_;
    SineGenerator *sineOscRight_;

    SineGenerator *sineOscLeft_1;
    SineGenerator *sineOscRight_1;
    SineGenerator *sineOscLeft_2;
    SineGenerator *sineOscRight_2;
    SineGenerator *sineOscLeft_3;
    SineGenerator *sineOscRight_3;

    AAudioStream *playStream_;
    AAudioStream *playStream_1;
    AAudioStream *playStream_2;
    AAudioStream *playStream_3;
    bool isToneOn_ = false;

    int32_t playStreamUnderrunCount_;
    int32_t bufSizeInFrames_;
    int32_t framesPerBurst_;
    double currentOutputLatencyMillis_ = 0;
    int32_t bufferSizeSelection_ = BUFFER_SIZE_AUTOMATIC;

    int32_t playStreamUnderrunCount_1;
    int32_t bufSizeInFrames_1;
    int32_t framesPerBurst_1;
    int32_t bufferSizeSelection_1 = BUFFER_SIZE_AUTOMATIC;

    int32_t playStreamUnderrunCount_2;
    int32_t bufSizeInFrames_2;
    int32_t framesPerBurst_2;
    int32_t bufferSizeSelection_2 = BUFFER_SIZE_AUTOMATIC;

    int32_t playStreamUnderrunCount_3;
    int32_t bufSizeInFrames_3;
    int32_t framesPerBurst_3;
    int32_t bufferSizeSelection_3 = BUFFER_SIZE_AUTOMATIC;

private:

    std::mutex restartingLock_;

    void createPlaybackStream();
    void closeOutputStream();
    void restartStream();

    AAudioStreamBuilder* createStreamBuilder();
    void setupPlaybackStreamParameters(AAudioStreamBuilder *builder);
    void prepareOscillators();

    void setupPlaybackStreamParameters1(AAudioStreamBuilder *builder);
    void prepareOscillators1();

    void setupPlaybackStreamParameters2(AAudioStreamBuilder *builder);
    void prepareOscillators2();

    void setupPlaybackStreamParameters3(AAudioStreamBuilder *builder);
    void prepareOscillators3();


    aaudio_result_t calculateCurrentOutputLatencyMillis(AAudioStream *stream, double *latencyMillis);

};


#endif //AAUDIO_PLAYAUDIOENGINE1_H
