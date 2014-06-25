#include "ZAudioManager.h"
#include "../ZEngine/ZEngine.h"

ZAudioAsset::ZAudioAsset (long int _aid, ZFilePath _path):
	ZAsset (_aid,_path) {
    type = ZAsset::Audio;
}

ZAudioAsset::~ZAudioAsset () {
    if (data != NULL)
        delete data;
    Mix_FreeMusic (sound);
}

void ZAudioAsset::load () {
    sound = Mix_LoadMUS (path.getPath().c_str());
    loaded = true;
}

void ZAudioAsset::sync () {
}

void ZAudioAsset::play () {
    if (loaded)
        Mix_PlayMusic (sound, 0);
}

void ZAudioAsset::loop () {
    if (loaded)
        Mix_PlayMusic (sound, -1);
}

ZAudioManager::ZAudioManager () {
    cout << "[INF] [OPENING AUDIO]" << endl;
    Mix_Init(1);
    if(Mix_OpenAudio(freq,MIX_DEFAULT_FORMAT,2,bufferSize) < 0) {
        cout << "[INF] [NO AUDIO SUPPORT!!]" << endl;
        support = false;
        return;
    }

    support = true;
    Mix_AllocateChannels(4);
}

ZAudioManager::~ZAudioManager () {
    cout << "[INF] [CLOSING AUDIO]" << endl;
    Mix_Quit();
}
