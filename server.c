#include <stdio.h>
#include <stdlib.h>
#include <portaudio.h>

#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (64)
#define NUM_CHANNELS    (2)
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;

typedef struct {
    FILE *file;
    int frameIndex;
    int totalFrames;
} paTestData;

static int paCallback(const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void *userData)
{
    paTestData *data = (paTestData*)userData;
    SAMPLE *out = (SAMPLE*)outputBuffer;
    (void) inputBuffer; /* Prevent unused variable warning. */

    size_t framesToRead = framesPerBuffer;
    size_t framesRead = fread(out, sizeof(SAMPLE), framesToRead * NUM_CHANNELS, data->file) / NUM_CHANNELS;
    
    if (framesRead < framesToRead) {
        printf("Reached end of file.\n");
        return paComplete;
    }

    return paContinue;
}

int main(int argc, char *argv[])
{
    PaStream *stream;
    PaError err;
    paTestData data;
    const char *filename = "example.wav";

    data.file = fopen(filename, "rb");
    if (!data.file) {
        printf("Could not open file %s\n", filename);
        return 1;
    }

    fseek(data.file, 0, SEEK_END);
    data.totalFrames = ftell(data.file) / (NUM_CHANNELS * sizeof(SAMPLE));
    fseek(data.file, 0, SEEK_SET);

    err = Pa_Initialize();
    if (err != paNoError) goto error;

    err = Pa_OpenDefaultStream(&stream,
                                0,          /* no input channels */
                                NUM_CHANNELS,
                                PA_SAMPLE_TYPE,
                                SAMPLE_RATE,
                                FRAMES_PER_BUFFER,        /* frames per buffer */
                                paCallback,
                                &data);
    if (err != paNoError) goto error;

    err = Pa_StartStream(stream);
    if (err != paNoError) goto error;

    printf("Playing file %s ...\n", filename);
    while (Pa_IsStreamActive(stream)) {
        Pa_Sleep(100);
    }

    err = Pa_CloseStream(stream);
    if (err != paNoError) goto error;

    Pa_Terminate();
    fclose(data.file);
    return 0;

error:
    fprintf(stderr, "An error occured while using the portaudio stream\n");
    fprintf(stderr, "Error number: %d\n", err);
    fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
    if (stream) {
        Pa_AbortStream(stream);
        Pa_CloseStream(stream);
    }
    Pa_Terminate();
    if (data.file) fclose(data.file);
    return -1;
}

