#include <stdio.h>
#include <stdlib.h>
#include <portaudio.h>

#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (64)
#define NUM_CHANNELS    (2)

typedef struct
{
    FILE *file;
}
paUserData;

static int paCallback(const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void *userData)
{
    paUserData *data = (paUserData*)userData;
    float *out = (float*)outputBuffer;
    (void) inputBuffer;

    if (data->file == NULL) {
        printf("File not available.\n");
        return paComplete;
    }

    size_t bytesRead = fread(out, sizeof(float), framesPerBuffer * NUM_CHANNELS, data->file);

    if (bytesRead < framesPerBuffer * NUM_CHANNELS) {
        printf("End of file reached.\n");
        fclose(data->file);
        data->file = NULL;
    }

    return paContinue;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s <audio_file>\n", argv[0]);
        return 1;
    }

    PaError err;
    paUserData data = {0};
    PaStream *stream;
    err = Pa_Initialize();
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        return 1;
    }

    data.file = fopen(argv[1], "rb");
    if (data.file == NULL) {
        printf("Error opening file %s\n", argv[1]);
        return 1;
    }

    err = Pa_OpenDefaultStream(&stream,
                               0,
                               NUM_CHANNELS,
                               paFloat32,
                               SAMPLE_RATE,
                               FRAMES_PER_BUFFER,
                               paCallback,
                               &data);
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        return 1;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        return 1;
    }

    printf("Playing file %s...\n", argv[1]);
    printf("Press Ctrl+C to stop.\n");

    while (1) {
        Pa_Sleep(1000);
    }

    err = Pa_StopStream(stream);
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        return 1;
    }

    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        return 1;
    }

    Pa_Terminate();
    fclose(data.file);

    return 0;
}
