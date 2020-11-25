//
// Created by rajat on 25/11/20.
//
#include <assert.h>
#include <trace.h>
#include <logging_macros.h>
#include <inttypes.h>
#include "PlayAudioEngine1.h"
/**
 * Every time the playback stream requires data this method will be called.
 *
 * @param stream the audio stream which is requesting data, this is the playStream_ object
 * @param userData the context in which the function is being called, in this case it will be the
 * PlayAudioEngine instance
 * @param audioData an empty buffer into which we can write our audio data
 * @param numFrames the number of audio frames which are required
 * @return Either AAUDIO_CALLBACK_RESULT_CONTINUE if the stream should continue requesting data
 * or AAUDIO_CALLBACK_RESULT_STOP if the stream should stop.
 *
 * @see PlayAudioEngine#dataCallback
 */
aaudio_data_callback_result_t dataCallback_temp(AAudioStream *stream, void *userData,
                                           void *audioData, int32_t numFrames) {
    assert(userData && audioData);
    PlayAudioEngine1 *audioEngine = reinterpret_cast<PlayAudioEngine1 *>(userData);
    return audioEngine->dataCallback(stream, audioData, numFrames);
}
aaudio_data_callback_result_t dataCallback_temp1(AAudioStream *stream, void *userData,
                                            void *audioData, int32_t numFrames) {
    assert(userData && audioData);
    PlayAudioEngine1 *audioEngine = reinterpret_cast<PlayAudioEngine1 *>(userData);
    return audioEngine->dataCallback1(stream, audioData, numFrames);
}
aaudio_data_callback_result_t dataCallback_temp2(AAudioStream *stream, void *userData,
                                                void *audioData, int32_t numFrames) {
    assert(userData && audioData);
    PlayAudioEngine1 *audioEngine = reinterpret_cast<PlayAudioEngine1 *>(userData);
    return audioEngine->dataCallback2(stream, audioData, numFrames);
}
aaudio_data_callback_result_t dataCallback_temp3(AAudioStream *stream, void *userData,
                                                 void *audioData, int32_t numFrames) {
    assert(userData && audioData);
    PlayAudioEngine1 *audioEngine = reinterpret_cast<PlayAudioEngine1 *>(userData);
    return audioEngine->dataCallback3(stream, audioData, numFrames);
}


/**
 * If there is an error with a stream this function will be called. A common example of an error
 * is when an audio device (such as headphones) is disconnected. In this case you should not
 * restart the stream within the callback, instead use a separate thread to perform the stream
 * recreation and restart.
 *
 * @param stream the stream with the error
 * @param userData the context in which the function is being called, in this case it will be the
 * PlayAudioEngine instance
 * @param error the error which occured, a human readable string can be obtained using
 * AAudio_convertResultToText(error);
 *
 * @see PlayAudioEngine#errorCallback
 */
void errorCallback_temp(AAudioStream *stream,
                   void *userData,
                   aaudio_result_t error) {
    assert(userData);
    PlayAudioEngine1 *audioEngine = reinterpret_cast<PlayAudioEngine1 *>(userData);
    audioEngine->errorCallback(stream, error);
}
void errorCallback_temp1(AAudioStream *stream,
                    void *userData,
                    aaudio_result_t error) {
    assert(userData);
    PlayAudioEngine1 *audioEngine = reinterpret_cast<PlayAudioEngine1 *>(userData);
    audioEngine->errorCallback1(stream, error);
}
void errorCallback_temp2(AAudioStream *stream,
                         void *userData,
                         aaudio_result_t error) {
    assert(userData);
    PlayAudioEngine1 *audioEngine = reinterpret_cast<PlayAudioEngine1 *>(userData);
    audioEngine->errorCallback2(stream, error);
}
void errorCallback_temp3(AAudioStream *stream,
                         void *userData,
                         aaudio_result_t error) {
    assert(userData);
    PlayAudioEngine1 *audioEngine = reinterpret_cast<PlayAudioEngine1 *>(userData);
    audioEngine->errorCallback3(stream, error);
}

PlayAudioEngine1::PlayAudioEngine1() {

    // Initialize the trace functions, this enables you to output trace statements without
    // blocking. See https://developer.android.com/studio/profile/systrace-commandline.html
    Trace::initialize();

    sampleChannels_ = kStereoChannelCount;
    sampleFormat_ = AAUDIO_FORMAT_PCM_FLOAT;

    // Create the output stream. By not specifying an audio device id we are telling AAudio that
    // we want the stream to be created using the default playback audio device.
    createPlaybackStream();
}

PlayAudioEngine1::~PlayAudioEngine1(){

    closeOutputStream();
    delete sineOscLeft_;
    delete sineOscRight_;
    delete sineOscLeft_1;
    delete sineOscRight_1;
    delete sineOscLeft_2;
    delete sineOscRight_2;
    delete sineOscLeft_3;
    delete sineOscRight_3;
}

/**
 * Set the audio device which should be used for playback. Can be set to AAUDIO_UNSPECIFIED if
 * you want to use the default playback device (which is usually the built-in speaker if
 * no other audio devices, such as headphones, are attached).
 *
 * @param deviceId the audio device id, can be obtained through an {@link AudioDeviceInfo} object
 * using Java/JNI.
 */
void PlayAudioEngine1::setDeviceId(int32_t deviceId){

    playbackDeviceId_ = deviceId;

    // If this is a different device from the one currently in use then restart the stream
    int32_t currentDeviceId = AAudioStream_getDeviceId(playStream_);
    if (deviceId != currentDeviceId) restartStream();
}

/**
 * Creates a stream builder which can be used to construct streams
 * @return a new stream builder object
 */
AAudioStreamBuilder* PlayAudioEngine1::createStreamBuilder() {

    AAudioStreamBuilder *builder = nullptr;
    aaudio_result_t result = AAudio_createStreamBuilder(&builder);
    if (result != AAUDIO_OK) {
        LOGE("Error creating stream builder: %s", AAudio_convertResultToText(result));
    }
    return builder;
}

/**
 * Creates an audio stream for playback. The audio device used will depend on playbackDeviceId_.
 */
void PlayAudioEngine1::createPlaybackStream(){

    AAudioStreamBuilder* builder = createStreamBuilder();

    if (builder != nullptr){

        setupPlaybackStreamParameters(builder);

        aaudio_result_t result = AAudioStreamBuilder_openStream(builder, &playStream_);

        if (result == AAUDIO_OK && playStream_ != nullptr){

            // check that we got PCM_FLOAT format
            if (sampleFormat_ != AAudioStream_getFormat(playStream_)) {
                LOGW("Sample format is not PCM_FLOAT");
            }

            sampleRate_ = AAudioStream_getSampleRate(playStream_);
            framesPerBurst_ = AAudioStream_getFramesPerBurst(playStream_);

            // Set the buffer size to the burst size - this will give us the minimum possible latency
            AAudioStream_setBufferSizeInFrames(playStream_, framesPerBurst_);
            bufSizeInFrames_ = framesPerBurst_;

            PrintAudioStreamInfo(playStream_);
            prepareOscillators();

            // Start the stream - the dataCallback function will start being called
            result = AAudioStream_requestStart(playStream_);
            if (result != AAUDIO_OK) {
                LOGE("Error starting stream. %s", AAudio_convertResultToText(result));
            }

            // Store the underrun count so we can tune the latency in the dataCallback
            playStreamUnderrunCount_ = AAudioStream_getXRunCount(playStream_);

        } else {
            LOGE("Failed to create stream. Error: %s", AAudio_convertResultToText(result));
        }

        AAudioStreamBuilder_delete(builder);

    } else {
        LOGE("Unable to obtain an AAudioStreamBuilder object");
    }

    AAudioStreamBuilder* builder1 = createStreamBuilder();

    if (builder1 != nullptr){

        setupPlaybackStreamParameters1(builder1);

        aaudio_result_t result = AAudioStreamBuilder_openStream(builder1, &playStream_1);

        if (result == AAUDIO_OK && playStream_1 != nullptr){

            // check that we got PCM_FLOAT format
            if (sampleFormat_ != AAudioStream_getFormat(playStream_1)) {
                LOGW("Sample format is not PCM_FLOAT");
            }

            sampleRate_ = AAudioStream_getSampleRate(playStream_1);
            framesPerBurst_1 = AAudioStream_getFramesPerBurst(playStream_1);

            // Set the buffer size to the burst size - this will give us the minimum possible latency
            AAudioStream_setBufferSizeInFrames(playStream_1, framesPerBurst_1);
            bufSizeInFrames_1 = framesPerBurst_1;

            PrintAudioStreamInfo(playStream_1);
            prepareOscillators1();

            // Start the stream - the dataCallback function will start being called
            result = AAudioStream_requestStart(playStream_1);
            if (result != AAUDIO_OK) {
                LOGE("Error starting stream. %s", AAudio_convertResultToText(result));
            }

            // Store the underrun count so we can tune the latency in the dataCallback
            playStreamUnderrunCount_1 = AAudioStream_getXRunCount(playStream_1);

        } else {
            LOGE("Failed to create stream. Error: %s", AAudio_convertResultToText(result));
        }

        AAudioStreamBuilder_delete(builder1);

    } else {
        LOGE("Unable to obtain an AAudioStreamBuilder object");
    }
    AAudioStreamBuilder* builder2 = createStreamBuilder();

    if (builder2 != nullptr){

        setupPlaybackStreamParameters2(builder2);

        aaudio_result_t result = AAudioStreamBuilder_openStream(builder2, &playStream_2);

        if (result == AAUDIO_OK && playStream_2 != nullptr){

            // check that we got PCM_FLOAT format
            if (sampleFormat_ != AAudioStream_getFormat(playStream_2)) {
                LOGW("Sample format is not PCM_FLOAT");
            }

            sampleRate_ = AAudioStream_getSampleRate(playStream_2);
            framesPerBurst_2 = AAudioStream_getFramesPerBurst(playStream_2);

            // Set the buffer size to the burst size - this will give us the minimum possible latency
            AAudioStream_setBufferSizeInFrames(playStream_2, framesPerBurst_2);
            bufSizeInFrames_2 = framesPerBurst_2;

            PrintAudioStreamInfo(playStream_2);
            prepareOscillators2();

            // Start the stream - the dataCallback function will start being called
            result = AAudioStream_requestStart(playStream_2);
            if (result != AAUDIO_OK) {
                LOGE("Error starting stream. %s", AAudio_convertResultToText(result));
            }

            // Store the underrun count so we can tune the latency in the dataCallback
            playStreamUnderrunCount_2 = AAudioStream_getXRunCount(playStream_2);

        } else {
            LOGE("Failed to create stream. Error: %s", AAudio_convertResultToText(result));
        }

        AAudioStreamBuilder_delete(builder2);

    } else {
        LOGE("Unable to obtain an AAudioStreamBuilder object");
    }
    AAudioStreamBuilder* builder3 = createStreamBuilder();

    if (builder3 != nullptr){

        setupPlaybackStreamParameters3(builder3);

        aaudio_result_t result = AAudioStreamBuilder_openStream(builder3, &playStream_3);

        if (result == AAUDIO_OK && playStream_3 != nullptr){

            // check that we got PCM_FLOAT format
            if (sampleFormat_ != AAudioStream_getFormat(playStream_3)) {
                LOGW("Sample format is not PCM_FLOAT");
            }

            sampleRate_ = AAudioStream_getSampleRate(playStream_3);
            framesPerBurst_3 = AAudioStream_getFramesPerBurst(playStream_3);

            // Set the buffer size to the burst size - this will give us the minimum possible latency
            AAudioStream_setBufferSizeInFrames(playStream_3, framesPerBurst_3);
            bufSizeInFrames_3 = framesPerBurst_3;

            PrintAudioStreamInfo(playStream_3);
            prepareOscillators3();

            // Start the stream - the dataCallback function will start being called
            result = AAudioStream_requestStart(playStream_3);
            if (result != AAUDIO_OK) {
                LOGE("Error starting stream. %s", AAudio_convertResultToText(result));
            }

            // Store the underrun count so we can tune the latency in the dataCallback
            playStreamUnderrunCount_3 = AAudioStream_getXRunCount(playStream_3);

        } else {
            LOGE("Failed to create stream. Error: %s", AAudio_convertResultToText(result));
        }

        AAudioStreamBuilder_delete(builder3);

    } else {
        LOGE("Unable to obtain an AAudioStreamBuilder object");
    }

}

void PlayAudioEngine1::prepareOscillators() {
    sineOscLeft_ = new SineGenerator();
    sineOscLeft_->setup(440.0, sampleRate_, 0.25);
    sineOscRight_ = new SineGenerator();
    sineOscRight_->setup(660.0, sampleRate_, 0.25);
}
void PlayAudioEngine1::prepareOscillators1() {
    sineOscLeft_1 = new SineGenerator();
    sineOscLeft_1->setup(440.0, sampleRate_, 0.25);
    sineOscRight_1 = new SineGenerator();
    sineOscRight_1->setup(660.0, sampleRate_, 0.25);
}
void PlayAudioEngine1::prepareOscillators2() {
    sineOscLeft_2 = new SineGenerator();
    sineOscLeft_2->setup(440.0, sampleRate_, 0.25);
    sineOscRight_2 = new SineGenerator();
    sineOscRight_2->setup(660.0, sampleRate_, 0.25);
}
void PlayAudioEngine1::prepareOscillators3() {
    sineOscLeft_3 = new SineGenerator();
    sineOscLeft_3->setup(440.0, sampleRate_, 0.25);
    sineOscRight_3 = new SineGenerator();
    sineOscRight_3->setup(660.0, sampleRate_, 0.25);
}

/**
 * Sets the stream parameters which are specific to playback, including device id and the
 * dataCallback function, which must be set for low latency playback.
 * @param builder The playback stream builder
 */
void PlayAudioEngine1::setupPlaybackStreamParameters(AAudioStreamBuilder *builder) {
    AAudioStreamBuilder_setDeviceId(builder, playbackDeviceId_);
    AAudioStreamBuilder_setFormat(builder, sampleFormat_);
    AAudioStreamBuilder_setChannelCount(builder, sampleChannels_);

    // We request EXCLUSIVE mode since this will give us the lowest possible latency.
    // If EXCLUSIVE mode isn't available the builder will fall back to SHARED mode.
    AAudioStreamBuilder_setSharingMode(builder, AAUDIO_SHARING_MODE_SHARED);
    AAudioStreamBuilder_setPerformanceMode(builder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
    AAudioStreamBuilder_setDirection(builder, AAUDIO_DIRECTION_OUTPUT);
    AAudioStreamBuilder_setDataCallback(builder, ::dataCallback_temp, this);
    AAudioStreamBuilder_setErrorCallback(builder, ::errorCallback_temp, this);
    AAudioStreamBuilder_setBufferCapacityInFrames(builder, 10000);
}
void PlayAudioEngine1::setupPlaybackStreamParameters1(AAudioStreamBuilder *builder) {
    AAudioStreamBuilder_setDeviceId(builder, playbackDeviceId_);
    AAudioStreamBuilder_setFormat(builder, sampleFormat_);
    AAudioStreamBuilder_setChannelCount(builder, sampleChannels_);

    // We request EXCLUSIVE mode since this will give us the lowest possible latency.
    // If EXCLUSIVE mode isn't available the builder will fall back to SHARED mode.
    AAudioStreamBuilder_setSharingMode(builder, AAUDIO_SHARING_MODE_SHARED);
    AAudioStreamBuilder_setPerformanceMode(builder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
    AAudioStreamBuilder_setDirection(builder, AAUDIO_DIRECTION_OUTPUT);
    AAudioStreamBuilder_setDataCallback(builder, ::dataCallback_temp1, this);
    AAudioStreamBuilder_setErrorCallback(builder, ::errorCallback_temp1, this);
    AAudioStreamBuilder_setBufferCapacityInFrames(builder, 10000);
}
void PlayAudioEngine1::setupPlaybackStreamParameters2(AAudioStreamBuilder *builder) {
    AAudioStreamBuilder_setDeviceId(builder, playbackDeviceId_);
    AAudioStreamBuilder_setFormat(builder, sampleFormat_);
    AAudioStreamBuilder_setChannelCount(builder, sampleChannels_);

    // We request EXCLUSIVE mode since this will give us the lowest possible latency.
    // If EXCLUSIVE mode isn't available the builder will fall back to SHARED mode.
    AAudioStreamBuilder_setSharingMode(builder, AAUDIO_SHARING_MODE_SHARED);
    AAudioStreamBuilder_setPerformanceMode(builder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
    AAudioStreamBuilder_setDirection(builder, AAUDIO_DIRECTION_OUTPUT);
    AAudioStreamBuilder_setDataCallback(builder, ::dataCallback_temp2, this);
    AAudioStreamBuilder_setErrorCallback(builder, ::errorCallback_temp2, this);
    AAudioStreamBuilder_setBufferCapacityInFrames(builder, 10000);
}
void PlayAudioEngine1::setupPlaybackStreamParameters3(AAudioStreamBuilder *builder) {
    AAudioStreamBuilder_setDeviceId(builder, playbackDeviceId_);
    AAudioStreamBuilder_setFormat(builder, sampleFormat_);
    AAudioStreamBuilder_setChannelCount(builder, sampleChannels_);

    // We request EXCLUSIVE mode since this will give us the lowest possible latency.
    // If EXCLUSIVE mode isn't available the builder will fall back to SHARED mode.
    AAudioStreamBuilder_setSharingMode(builder, AAUDIO_SHARING_MODE_SHARED);
    AAudioStreamBuilder_setPerformanceMode(builder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
    AAudioStreamBuilder_setDirection(builder, AAUDIO_DIRECTION_OUTPUT);
    AAudioStreamBuilder_setDataCallback(builder, ::dataCallback_temp3, this);
    AAudioStreamBuilder_setErrorCallback(builder, ::errorCallback_temp3, this);
    AAudioStreamBuilder_setBufferCapacityInFrames(builder, 10000);
}

void PlayAudioEngine1::closeOutputStream(){

    if (playStream_ != nullptr){
        aaudio_result_t result = AAudioStream_requestStop(playStream_);
        if (result != AAUDIO_OK){
            LOGE("Error stopping output stream. %s", AAudio_convertResultToText(result));
        }

        result = AAudioStream_close(playStream_);
        if (result != AAUDIO_OK){
            LOGE("Error closing output stream. %s", AAudio_convertResultToText(result));
        }
    }
    if (playStream_1 != nullptr){
        aaudio_result_t result = AAudioStream_requestStop(playStream_1);
        if (result != AAUDIO_OK){
            LOGE("Error stopping output stream. %s", AAudio_convertResultToText(result));
        }

        result = AAudioStream_close(playStream_1);
        if (result != AAUDIO_OK){
            LOGE("Error closing output stream. %s", AAudio_convertResultToText(result));
        }
    }
    if (playStream_2 != nullptr){
        aaudio_result_t result = AAudioStream_requestStop(playStream_2);
        if (result != AAUDIO_OK){
            LOGE("Error stopping output stream. %s", AAudio_convertResultToText(result));
        }

        result = AAudioStream_close(playStream_2);
        if (result != AAUDIO_OK){
            LOGE("Error closing output stream. %s", AAudio_convertResultToText(result));
        }
    }
    if (playStream_3 != nullptr){
        aaudio_result_t result = AAudioStream_requestStop(playStream_3);
        if (result != AAUDIO_OK){
            LOGE("Error stopping output stream. %s", AAudio_convertResultToText(result));
        }

        result = AAudioStream_close(playStream_3);
        if (result != AAUDIO_OK){
            LOGE("Error closing output stream. %s", AAudio_convertResultToText(result));
        }
    }
}

void PlayAudioEngine1::setToneOn(bool isToneOn){
    isToneOn_ = isToneOn;
}

/**
 * @see dataCallback function at top of this file
 */
aaudio_data_callback_result_t PlayAudioEngine1::dataCallback(AAudioStream *stream,
                                                            void *audioData,
                                                            int32_t numFrames) {
//  assert(stream == playStream_);

    int32_t underrunCount = AAudioStream_getXRunCount(playStream_);
    aaudio_result_t bufferSize = AAudioStream_getBufferSizeInFrames(playStream_);
    bool hasUnderrunCountIncreased = false;
    bool shouldChangeBufferSize = false;

    if (underrunCount > playStreamUnderrunCount_){
        playStreamUnderrunCount_ = underrunCount;
        hasUnderrunCountIncreased = true;
    }

    if (hasUnderrunCountIncreased && bufferSizeSelection_ == BUFFER_SIZE_AUTOMATIC){

        /**
         * This is a buffer size tuning algorithm. If the number of underruns (i.e. instances where
         * we were unable to supply sufficient data to the stream) has increased since the last callback
         * we will try to increase the buffer size by the burst size, which will give us more protection
         * against underruns in future, at the cost of additional latency.
         */
        bufferSize += framesPerBurst_; // Increase buffer size by one burst
        shouldChangeBufferSize = true;
    } else if (bufferSizeSelection_ > 0 && (bufferSizeSelection_ * framesPerBurst_) != bufferSize){

        // If the buffer size selection has changed then update it here
        bufferSize = bufferSizeSelection_ * framesPerBurst_;
        shouldChangeBufferSize = true;
    }

    if (shouldChangeBufferSize){
        LOGD("Setting buffer size to %d", bufferSize);
        bufferSize = AAudioStream_setBufferSizeInFrames(stream, bufferSize);
        if (bufferSize > 0) {
            bufSizeInFrames_ = bufferSize;
        } else {
            LOGE("Error setting buffer size: %s", AAudio_convertResultToText(bufferSize));
        }
    }

    /**
     * The following output can be seen by running a systrace. Tracing is preferable to logging
     * inside the callback since tracing does not block.
     *
     * See https://developer.android.com/studio/profile/systrace-commandline.html
     */
    Trace::beginSection("numFrames %d, Underruns %d, buffer size %d",
                        numFrames, underrunCount, bufferSize);

    int32_t samplesPerFrame = sampleChannels_;

    // If the tone is on we need to use our synthesizer to render the audio data for the sine waves
    if (isToneOn_) {
        sineOscRight_->render(static_cast<float *>(audioData),
                              samplesPerFrame, numFrames);
        if (sampleChannels_ == 2) {
            sineOscLeft_->render(static_cast<float *>(audioData) + 1,
                                 samplesPerFrame, numFrames);
        }
    } else {
        memset(static_cast<uint8_t *>(audioData), 0,
               sizeof(float) * samplesPerFrame * numFrames);
    }
    calculateCurrentOutputLatencyMillis(stream, &currentOutputLatencyMillis_);
    Trace::endSection();
    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}
aaudio_data_callback_result_t PlayAudioEngine1::dataCallback1(AAudioStream *stream,
                                                             void *audioData,
                                                             int32_t numFrames) {
//    assert(stream == playStream_);

    int32_t underrunCount = AAudioStream_getXRunCount(playStream_1);
    aaudio_result_t bufferSize = AAudioStream_getBufferSizeInFrames(playStream_1);
    bool hasUnderrunCountIncreased = false;
    bool shouldChangeBufferSize = false;

    if (underrunCount > playStreamUnderrunCount_1){
        playStreamUnderrunCount_1 = underrunCount;
        hasUnderrunCountIncreased = true;
    }

    if (hasUnderrunCountIncreased && bufferSizeSelection_1 == BUFFER_SIZE_AUTOMATIC){

        /**
         * This is a buffer size tuning algorithm. If the number of underruns (i.e. instances where
         * we were unable to supply sufficient data to the stream) has increased since the last callback
         * we will try to increase the buffer size by the burst size, which will give us more protection
         * against underruns in future, at the cost of additional latency.
         */
        bufferSize += framesPerBurst_1; // Increase buffer size by one burst
        shouldChangeBufferSize = true;
    } else if (bufferSizeSelection_1 > 0 && (bufferSizeSelection_1 * framesPerBurst_1) != bufferSize){

        // If the buffer size selection has changed then update it here
        bufferSize = bufferSizeSelection_1 * framesPerBurst_1;
        shouldChangeBufferSize = true;
    }

    if (shouldChangeBufferSize){
        LOGD("Setting buffer size 1 to %d", bufferSize);
        bufferSize = AAudioStream_setBufferSizeInFrames(stream, bufferSize);
        if (bufferSize > 0) {
            bufSizeInFrames_1 = bufferSize;
        } else {
            LOGE("Error setting buffer size 1 : %s", AAudio_convertResultToText(bufferSize));
        }
    }

    /**
     * The following output can be seen by running a systrace. Tracing is preferable to logging
     * inside the callback since tracing does not block.
     *
     * See https://developer.android.com/studio/profile/systrace-commandline.html
     */
    Trace::beginSection("numFrames 1 %d, Underruns 1 %d, buffer size 1 %d",
                        numFrames, underrunCount, bufferSize);

    int32_t samplesPerFrame = sampleChannels_;

    // If the tone is on we need to use our synthesizer to render the audio data for the sine waves
    if (isToneOn_) {
        sineOscRight_1->render(static_cast<float *>(audioData),
                               samplesPerFrame, numFrames);
        if (sampleChannels_ == 2) {
            sineOscLeft_1->render(static_cast<float *>(audioData) + 1,
                                  samplesPerFrame, numFrames);
        }
    } else {
        memset(static_cast<uint8_t *>(audioData), 0,
               sizeof(float) * samplesPerFrame * numFrames);
    }

//  calculateCurrentOutputLatencyMillis(stream, &currentOutputLatencyMillis_);

    Trace::endSection();
    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

aaudio_data_callback_result_t PlayAudioEngine1::dataCallback2(AAudioStream *stream,
                                                              void *audioData,
                                                              int32_t numFrames) {
//    assert(stream == playStream_);

    int32_t underrunCount = AAudioStream_getXRunCount(playStream_2);
    aaudio_result_t bufferSize = AAudioStream_getBufferSizeInFrames(playStream_2);
    bool hasUnderrunCountIncreased = false;
    bool shouldChangeBufferSize = false;

    if (underrunCount > playStreamUnderrunCount_2){
        playStreamUnderrunCount_2 = underrunCount;
        hasUnderrunCountIncreased = true;
    }

    if (hasUnderrunCountIncreased && bufferSizeSelection_2 == BUFFER_SIZE_AUTOMATIC){

        /**
         * This is a buffer size tuning algorithm. If the number of underruns (i.e. instances where
         * we were unable to supply sufficient data to the stream) has increased since the last callback
         * we will try to increase the buffer size by the burst size, which will give us more protection
         * against underruns in future, at the cost of additional latency.
         */
        bufferSize += framesPerBurst_2; // Increase buffer size by one burst
        shouldChangeBufferSize = true;
    } else if (bufferSizeSelection_2 > 0 && (bufferSizeSelection_2 * framesPerBurst_2) != bufferSize){

        // If the buffer size selection has changed then update it here
        bufferSize = bufferSizeSelection_2 * framesPerBurst_2;
        shouldChangeBufferSize = true;
    }

    if (shouldChangeBufferSize){
        LOGD("Setting buffer size 1 to %d", bufferSize);
        bufferSize = AAudioStream_setBufferSizeInFrames(stream, bufferSize);
        if (bufferSize > 0) {
            bufSizeInFrames_2 = bufferSize;
        } else {
            LOGE("Error setting buffer size 1 : %s", AAudio_convertResultToText(bufferSize));
        }
    }

    /**
     * The following output can be seen by running a systrace. Tracing is preferable to logging
     * inside the callback since tracing does not block.
     *
     * See https://developer.android.com/studio/profile/systrace-commandline.html
     */
    Trace::beginSection("numFrames 1 %d, Underruns 1 %d, buffer size 1 %d",
                        numFrames, underrunCount, bufferSize);

    int32_t samplesPerFrame = sampleChannels_;

    // If the tone is on we need to use our synthesizer to render the audio data for the sine waves
    if (isToneOn_) {
        sineOscRight_2->render(static_cast<float *>(audioData),
                               samplesPerFrame, numFrames);
        if (sampleChannels_ == 2) {
            sineOscLeft_2->render(static_cast<float *>(audioData) + 1,
                                  samplesPerFrame, numFrames);
        }
    } else {
        memset(static_cast<uint8_t *>(audioData), 0,
               sizeof(float) * samplesPerFrame * numFrames);
    }

//  calculateCurrentOutputLatencyMillis(stream, &currentOutputLatencyMillis_);

    Trace::endSection();
    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

aaudio_data_callback_result_t PlayAudioEngine1::dataCallback3(AAudioStream *stream,
                                                              void *audioData,
                                                              int32_t numFrames) {
//    assert(stream == playStream_);

    int32_t underrunCount = AAudioStream_getXRunCount(playStream_3);
    aaudio_result_t bufferSize = AAudioStream_getBufferSizeInFrames(playStream_3);
    bool hasUnderrunCountIncreased = false;
    bool shouldChangeBufferSize = false;

    if (underrunCount > playStreamUnderrunCount_3){
        playStreamUnderrunCount_3 = underrunCount;
        hasUnderrunCountIncreased = true;
    }

    if (hasUnderrunCountIncreased && bufferSizeSelection_3 == BUFFER_SIZE_AUTOMATIC){

        /**
         * This is a buffer size tuning algorithm. If the number of underruns (i.e. instances where
         * we were unable to supply sufficient data to the stream) has increased since the last callback
         * we will try to increase the buffer size by the burst size, which will give us more protection
         * against underruns in future, at the cost of additional latency.
         */
        bufferSize += framesPerBurst_3; // Increase buffer size by one burst
        shouldChangeBufferSize = true;
    } else if (bufferSizeSelection_3 > 0 && (bufferSizeSelection_3 * framesPerBurst_3) != bufferSize){

        // If the buffer size selection has changed then update it here
        bufferSize = bufferSizeSelection_3 * framesPerBurst_3;
        shouldChangeBufferSize = true;
    }

    if (shouldChangeBufferSize){
        LOGD("Setting buffer size 1 to %d", bufferSize);
        bufferSize = AAudioStream_setBufferSizeInFrames(stream, bufferSize);
        if (bufferSize > 0) {
            bufSizeInFrames_3 = bufferSize;
        } else {
            LOGE("Error setting buffer size 1 : %s", AAudio_convertResultToText(bufferSize));
        }
    }

    /**
     * The following output can be seen by running a systrace. Tracing is preferable to logging
     * inside the callback since tracing does not block.
     *
     * See https://developer.android.com/studio/profile/systrace-commandline.html
     */
    Trace::beginSection("numFrames 1 %d, Underruns 1 %d, buffer size 1 %d",
                        numFrames, underrunCount, bufferSize);

    int32_t samplesPerFrame = sampleChannels_;

    // If the tone is on we need to use our synthesizer to render the audio data for the sine waves
    if (isToneOn_) {
        sineOscRight_3->render(static_cast<float *>(audioData),
                               samplesPerFrame, numFrames);
        if (sampleChannels_ == 2) {
            sineOscLeft_3->render(static_cast<float *>(audioData) + 1,
                                  samplesPerFrame, numFrames);
        }
    } else {
        memset(static_cast<uint8_t *>(audioData), 0,
               sizeof(float) * samplesPerFrame * numFrames);
    }

//  calculateCurrentOutputLatencyMillis(stream, &currentOutputLatencyMillis_);

    Trace::endSection();
    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

/**
 * Calculate the current latency between writing a frame to the output stream and
 * the same frame being presented to the audio hardware.
 *
 * Here's how the calculation works:
 *
 * 1) Get the time a particular frame was presented to the audio hardware
 * @see AAudioStream_getTimestamp
 * 2) From this extrapolate the time which the *next* audio frame written to the stream
 * will be presented
 * 3) Assume that the next audio frame is written at the current time
 * 4) currentLatency = nextFramePresentationTime - nextFrameWriteTime
 *
 * @param stream The stream being written to
 * @param latencyMillis pointer to a variable to receive the latency in milliseconds between
 * writing a frame to the stream and that frame being presented to the audio hardware.
 * @return AAUDIO_OK or a negative error. It is normal to receive an error soon after a stream
 * has started because the timestamps are not yet available.
 */
aaudio_result_t
PlayAudioEngine1::calculateCurrentOutputLatencyMillis(AAudioStream *stream, double *latencyMillis) {

    // Get the time that a known audio frame was presented for playing
    int64_t existingFrameIndex;
    int64_t existingFramePresentationTime;
    aaudio_result_t result = AAudioStream_getTimestamp(stream,
                                                       CLOCK_MONOTONIC,
                                                       &existingFrameIndex,
                                                       &existingFramePresentationTime);

    if (result == AAUDIO_OK){

        // Get the write index for the next audio frame
        int64_t writeIndex = AAudioStream_getFramesWritten(stream);

        // Calculate the number of frames between our known frame and the write index
        int64_t frameIndexDelta = writeIndex - existingFrameIndex;

        // Calculate the time which the next frame will be presented
        int64_t frameTimeDelta = (frameIndexDelta * NANOS_PER_SECOND) / sampleRate_;
        int64_t nextFramePresentationTime = existingFramePresentationTime + frameTimeDelta;

        // Assume that the next frame will be written at the current time
        int64_t nextFrameWriteTime = get_time_nanoseconds(CLOCK_MONOTONIC);

        // Calculate the latency
        *latencyMillis = (double) (nextFramePresentationTime - nextFrameWriteTime)
                         / NANOS_PER_MILLISECOND;
    } else {
        LOGE("Error calculating latency: %s", AAudio_convertResultToText(result));
    }

    return result;
}

/**
 * @see errorCallback function at top of this file
 */
void PlayAudioEngine1::errorCallback(AAudioStream *stream,
                                    aaudio_result_t error){

//  assert(stream == playStream_);
    LOGD("errorCallback result: %s", AAudio_convertResultToText(error));

    aaudio_stream_state_t streamState = AAudioStream_getState(playStream_);
    if (streamState == AAUDIO_STREAM_STATE_DISCONNECTED){

        // Handle stream restart on a separate thread
        std::function<void(void)> restartStream = std::bind(&PlayAudioEngine1::restartStream, this);
        std::thread streamRestartThread(restartStream);
        streamRestartThread.detach();
    }
}
void PlayAudioEngine1::errorCallback1(AAudioStream *stream,
                                     aaudio_result_t error){

//  assert(stream == playStream_);
    LOGD("errorCallback result: %s", AAudio_convertResultToText(error));

    aaudio_stream_state_t streamState = AAudioStream_getState(playStream_1);
    if (streamState == AAUDIO_STREAM_STATE_DISCONNECTED){

        // Handle stream restart on a separate thread
        std::function<void(void)> restartStream = std::bind(&PlayAudioEngine1::restartStream, this);
        std::thread streamRestartThread(restartStream);
        streamRestartThread.detach();
    }
}
void PlayAudioEngine1::errorCallback2(AAudioStream *stream,
                                      aaudio_result_t error){

//  assert(stream == playStream_);
    LOGD("errorCallback result: %s", AAudio_convertResultToText(error));

    aaudio_stream_state_t streamState = AAudioStream_getState(playStream_2);
    if (streamState == AAUDIO_STREAM_STATE_DISCONNECTED){

        // Handle stream restart on a separate thread
        std::function<void(void)> restartStream = std::bind(&PlayAudioEngine1::restartStream, this);
        std::thread streamRestartThread(restartStream);
        streamRestartThread.detach();
    }
}
void PlayAudioEngine1::errorCallback3(AAudioStream *stream,
                                      aaudio_result_t error){

//  assert(stream == playStream_);
    LOGD("errorCallback result: %s", AAudio_convertResultToText(error));

    aaudio_stream_state_t streamState = AAudioStream_getState(playStream_3);
    if (streamState == AAUDIO_STREAM_STATE_DISCONNECTED){

        // Handle stream restart on a separate thread
        std::function<void(void)> restartStream = std::bind(&PlayAudioEngine1::restartStream, this);
        std::thread streamRestartThread(restartStream);
        streamRestartThread.detach();
    }
}

void PlayAudioEngine1::restartStream(){

    LOGI("Restarting stream");

    if (restartingLock_.try_lock()){
        closeOutputStream();
        createPlaybackStream();
        restartingLock_.unlock();
    } else {
        LOGW("Restart stream operation already in progress - ignoring this request");
        // We were unable to obtain the restarting lock which means the restart operation is currently
        // active. This is probably because we received successive "stream disconnected" events.
        // Internal issue b/63087953
    }
}

double PlayAudioEngine1::getCurrentOutputLatencyMillis() {
    return currentOutputLatencyMillis_;
}

void PlayAudioEngine1::setBufferSizeInBursts(int32_t numBursts) {
    PlayAudioEngine1::bufferSizeSelection_ = numBursts;
    PlayAudioEngine1::bufferSizeSelection_1 = numBursts;
    PlayAudioEngine1::bufferSizeSelection_2 = numBursts;
    PlayAudioEngine1::bufferSizeSelection_3 = numBursts;
}
