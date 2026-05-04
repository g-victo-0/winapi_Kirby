#include "soundmanger.h"
#include "resource.h"
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

void PlayBGM(HINSTANCE hInst)
{
    // resource.h에 있는 실제 소리 ID가 다르면 IDR_WAVE1만 바꾸면 됨
    PlaySound(
        MAKEINTRESOURCE(IDR_WAVE1),
        hInst,
        SND_RESOURCE | SND_ASYNC | SND_LOOP
    );
}

void StopBGM()
{
    PlaySound(NULL, NULL, 0);
}
