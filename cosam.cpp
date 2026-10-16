#include <SDL2/SDL.h>

#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <cmath>

using namespace std;

constexpr double PI = 3.14159265358979323846;
constexpr int SAMPLE_RATE = 44100;

SDL_AudioDeviceID device = 0;

struct AudioState {
    double frequency = 440.0;
    double phase = 0.0;
    double volume = 0.20;
};

AudioState audio;

void audioCallback(void* userdata, Uint8* stream, int len) {
    auto* state = static_cast<AudioState*>(userdata);

    float* samples = reinterpret_cast<float*>(stream);
    int count = len / sizeof(float);

    for (int i = 0; i < count; i++) {
        double time = state->phase / (2.0 * PI);
        double envelope = 1.0;

        (void)time;

        samples[i] = static_cast<float>(
            state->volume *
            envelope *
            sin(state->phase)
        );

        state->phase +=
            2.0 * PI * state->frequency / SAMPLE_RATE;

        if (state->phase >= 2.0 * PI)
            state->phase -= 2.0 * PI;
    }
}

void playTone(double frequency, int duration = 180) {
    if (device == 0)
        return;

    SDL_LockAudioDevice(device);

    audio.frequency = frequency;
    audio.phase = 0.0;

    SDL_UnlockAudioDevice(device);

    SDL_PauseAudioDevice(device, 0);
    SDL_Delay(duration);
    SDL_PauseAudioDevice(device, 1);
}

const double melody[] = {
    293.66, // D4
    349.23, // F4
    440.00, // A4
    392.00, // G4
    440.00, // A4
    523.25, // C5
    587.33, // D5
    493.88, // B4
    440.00, // A4
    392.00, // G4
    349.23, // F4
    329.63  // E4
};

constexpr int MELODY_SIZE =
    sizeof(melody) / sizeof(melody[0]);

void typeText(const string& text, bool red = false, bool green = false, int delayMs = 180) {
    stringstream ss(text);
    string word;
    int index = 0;

    while (ss >> word) {
        if (red) {
            cout << "\033[31m" << word << "\033[0m";
        } else if (green) {
            cout << "\033[92m" << word << "\033[0m";
        } else {
            cout << word;
        }

        cout << " " << flush;

        double frequency = melody[index % MELODY_SIZE];

        if (red)
            frequency *= 2.0;

        if (green)
            frequency *= 0.5;

        playTone(frequency, 130);

        this_thread::sleep_for(
            chrono::milliseconds(delayMs)
        );

        index++;
    }

    cout << endl;
}

void printInstant(const string& text, bool red = false, bool green = false) {
    if (red) {
        cout << "\033[31m" << text << "\033[0m" << endl;
    } else if (green) {
        cout << "\033[92m" << text << "\033[0m" << endl;
    } else {
        cout << text << endl;
    }
}

void pauseText(int milliseconds) {
    this_thread::sleep_for(
        chrono::milliseconds(milliseconds)
    );
}

int main() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        cerr << "SDL error: "
             << SDL_GetError() << endl;
        return 1;
    }

    SDL_AudioSpec want{};

    want.freq = SAMPLE_RATE;
    want.format = AUDIO_F32SYS;
    want.channels = 1;
    want.samples = 512;
    want.callback = audioCallback;
    want.userdata = &audio;

    device = SDL_OpenAudioDevice(
        nullptr,
        0,
        &want,
        nullptr,
        0
    );

    if (device == 0) {
        cerr << "Audio error: "
             << SDL_GetError() << endl;

        SDL_Quit();
        return 1;
    }
    pauseText(500);

    typeText("What makes You think You're God?");
    typeText("What makes You think You're God?");
    typeText("Do You think I'll applaud?");

    pauseText(300);

    typeText("Be-e-cause I am.", true);
    typeText("Are you a fan?", true);

    pauseText(600);

    printInstant(
        "(If God talks, how can you be sure "
        "it's not Him? You can't, so harden "
        "not your heart)",
        false,
        true
    );

    pauseText(1000);

    typeText("What makes You think You're God?");
    typeText("What makes You think You're God?");
    typeText("Do You think I'll applaud?");

    pauseText(300);

    typeText("Be-e-cause I am.", true);
    typeText("Are you a fan?", true);

    SDL_CloseAudioDevice(device);
    SDL_Quit();

    return 0;
}
