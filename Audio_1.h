
#pragma once
#ifndef AUDIO_H
#define AUDIO_H

// ---------------------------------------------------------------------------
//  SOUND  --  background music AND hit sound effects
//
//  iGraphics has no sound of its own, so this uses two different bits of
//  Windows, on purpose:
//
//    MUSIC  -> PlaySound.  It loops natively with SND_LOOP and needs no
//              device to be opened, which makes it perfect for a long track.
//              The catch: PlaySound plays only ONE sound at a time per
//              program, so it cannot be used for the hits as well - every
//              punch would silence the music.
//
//    HITS   -> MCI (mciSendString).  MCI opens its own device, so an MCI
//              sound plays ON TOP of the PlaySound music instead of
//              replacing it. Each sound gets a few "voices" so two hits
//              close together overlap instead of cutting each other off.
//
//  EVERY FILE MUST BE A REAL PCM .wav.  Renaming an .mp3, .mpeg or .aac to
//  .wav does not work - the bytes have to be converted.
// ---------------------------------------------------------------------------

#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include <cstdio>
#include <cstdlib>

// ===========================================================================
//  BACKGROUND MUSIC
// ===========================================================================
enum MusicTrack
{
    MUSIC_NONE = 0,
    MUSIC_MENU,        // MenuMusic.wav
    MUSIC_LEVEL,       // Level12Music.wav  - layer 1 and the start of layer 2
    MUSIC_WARDEN,      // WardenMusic.wav   - from the moment the Warden appears
    MUSIC_LEVEL3,      // Level3Music.wav   - the whole of layer 3
    MUSIC_COUNT
};

static unsigned char* g_musicData[MUSIC_COUNT] = { 0, 0, 0, 0, 0 };
static int            g_musicCurrent           = MUSIC_NONE;
static int            g_musicVolumePercent     = 60;   // 0 .. 100

inline bool fileThere(const char* path)
{
    FILE* f = fopen(path, "rb");
    if (f == 0) return false;
    fclose(f);
    return true;
}

// Reads a whole WAV into memory. PlaySound with SND_MEMORY wants the complete
// file, header included, which is exactly what this returns.
inline unsigned char* readWholeFile(const char* path)
{
    FILE* f = fopen(path, "rb");
    if (f == 0) return 0;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size <= 44)          // smaller than a WAV header: not a usable file
    {
        fclose(f);
        return 0;
    }

    unsigned char* buf = (unsigned char*)malloc((size_t)size);
    if (buf == 0) { fclose(f); return 0; }

    size_t got = fread(buf, 1, (size_t)size, f);
    fclose(f);

    if (got != (size_t)size) { free(buf); return 0; }
    return buf;
}

// Loads the first of the candidate paths that exists.
inline bool musicLoadTrack(int track, const char* p1, const char* p2,
                           const char* p3, const char* p4)
{
    if (track <= MUSIC_NONE || track >= MUSIC_COUNT) return false;

    const char* path = 0;
    if      (p1 && fileThere(p1)) path = p1;
    else if (p2 && fileThere(p2)) path = p2;
    else if (p3 && fileThere(p3)) path = p3;
    else if (p4 && fileThere(p4)) path = p4;
    if (path == 0) return false;

    g_musicData[track] = readWholeFile(path);
    return g_musicData[track] != 0;
}

// Did this track's file actually load? Used to fall back to another track.
inline bool musicHasTrack(int track)
{
    if (track <= MUSIC_NONE || track >= MUSIC_COUNT) return false;
    return g_musicData[track] != 0;
}

inline void musicStopAll()
{
    PlaySoundA(0, 0, 0);       // stops whatever is playing
    g_musicCurrent = MUSIC_NONE;
}

// Switches tracks. Calling it with the track already playing does nothing, so
// it is safe to call every tick.
inline void musicPlay(int track)
{
    if (track == g_musicCurrent) return;
    g_musicCurrent = track;

    if (track <= MUSIC_NONE || g_musicData[track] == 0)
    {
        PlaySoundA(0, 0, 0);
        return;
    }

    // SND_LOOP repeats until something else is played, SND_ASYNC returns at
    // once so the game keeps running.
    PlaySoundA((LPCSTR)g_musicData[track], 0,
                SND_MEMORY | SND_ASYNC | SND_LOOP);
}

// SND_LOOP handles repeating on its own, so there is nothing to poll. Kept so
// the call in fixedUpdate stays valid.
inline void musicTick()
{
}

// PlaySound has no volume of its own; this sets the level for this program's
// wave output. On Windows Vista and later that is per application, so it does
// not touch the volume of anything else. It covers the MCI hits too.
inline void musicSetVolume(int percent)
{
    if (percent < 0)   percent = 0;
    if (percent > 100) percent = 100;
    g_musicVolumePercent = percent;

    unsigned short level = (unsigned short)((65535L * percent) / 100);
    DWORD both = ((DWORD)level << 16) | level;     // right channel | left
    waveOutSetVolume(0, both);
}


// ===========================================================================
//  HIT SOUNDS
//
//  TO CHANGE A HIT SOUND: drop your .wav in the Music folder under the name
//  listed in sfxInit() below, or add your own name to that list.
// ===========================================================================
enum SoundId
{
    SFX_PLAYER_HIT = 0,   // playerHit.wav - the hero's kick connects
    SFX_ENEMY_HIT,        // enemyHit.wav  - an enemy connects with the hero
    SFX_COUNT
};

// How many copies of each sound can overlap. Three is plenty: hit the third
// enemy before the first sound has finished and it still plays.
const int SFX_VOICES = 3;

static char g_sfxAlias[SFX_COUNT][SFX_VOICES][24];
static bool g_sfxReady[SFX_COUNT][SFX_VOICES];
static int  g_sfxNextVoice[SFX_COUNT];
static bool g_sfxInitDone = false;

// Opens one file on SFX_VOICES separate MCI devices.
inline void sfxOpen(int id, const char* p1, const char* p2, const char* p3)
{
    g_sfxNextVoice[id] = 0;
    for (int v = 0; v < SFX_VOICES; v++) g_sfxReady[id][v] = false;

    const char* path = 0;
    if      (p1 && fileThere(p1)) path = p1;
    else if (p2 && fileThere(p2)) path = p2;
    else if (p3 && fileThere(p3)) path = p3;
    if (path == 0) return;          // no file: this sound stays silent

    for (int v = 0; v < SFX_VOICES; v++)
    {
        sprintf(g_sfxAlias[id][v], "gsfx%d_%d", id, v);

        char cmd[512];
        sprintf(cmd, "open \"%s\" type waveaudio alias %s",
                path, g_sfxAlias[id][v]);

        // 0 back from mciSendString means the device opened.
        g_sfxReady[id][v] = (mciSendStringA(cmd, 0, 0, 0) == 0);
    }
}

inline void sfxInit()
{
    if (g_sfxInitDone) return;
    g_sfxInitDone = true;

    // the hero's kick connecting - the same sound in every level
    sfxOpen(SFX_PLAYER_HIT,
            "Music\\playerHit.wav",
            "Music\\playerHits.wav",
            "Music\\PlayerHit.wav");

    // an enemy connecting with the hero - the same sound in every level
    sfxOpen(SFX_ENEMY_HIT,
            "Music\\enemyHit.wav",
            "Music\\enemyHits.wav",
            "Music\\EnemyHit.wav");
}

// Plays one hit. Rewinds to the start first, so holding down the kick key
// retriggers the sound instead of doing nothing.
inline void sfxPlay(int id)
{
    if (id < 0 || id >= SFX_COUNT) return;

    // try each voice in turn so overlapping hits do not cut each other off
    for (int tries = 0; tries < SFX_VOICES; tries++)
    {
        int v = g_sfxNextVoice[id];
        g_sfxNextVoice[id] = (v + 1) % SFX_VOICES;

        if (!g_sfxReady[id][v]) continue;

        char cmd[128];
        sprintf(cmd, "seek %s to start", g_sfxAlias[id][v]);
        mciSendStringA(cmd, 0, 0, 0);

        sprintf(cmd, "play %s", g_sfxAlias[id][v]);
        mciSendStringA(cmd, 0, 0, 0);
        return;
    }
}

inline void sfxShutdown()
{
    for (int id = 0; id < SFX_COUNT; id++)
        for (int v = 0; v < SFX_VOICES; v++)
            if (g_sfxReady[id][v])
            {
                char cmd[128];
                sprintf(cmd, "close %s", g_sfxAlias[id][v]);
                mciSendStringA(cmd, 0, 0, 0);
                g_sfxReady[id][v] = false;
            }
}


// ===========================================================================
//  STARTUP / SHUTDOWN
// ===========================================================================
inline void musicShutdown()
{
    musicStopAll();
    sfxShutdown();
    for (int t = MUSIC_MENU; t < MUSIC_COUNT; t++)
    {
        if (g_musicData[t]) { free(g_musicData[t]); g_musicData[t] = 0; }
    }
}

// TO CHANGE A TRACK: drop a WAV in the Music folder under one of these names,
// or add your own name to the list.
inline void musicInit()
{
    static bool done = false;
    if (done) return;
    done = true;

    musicLoadTrack(MUSIC_MENU,
                   "Music/MenuMusic.wav",
                   "Music/menuMusic.wav",
                   "Music/Menu.wav",
                   0);

    musicLoadTrack(MUSIC_LEVEL,
                   "Music/Level12Music.wav",
                   "Music/level12Music.wav",
                   "Music/LevelMusic.wav",
                   0);

    musicLoadTrack(MUSIC_WARDEN,
                   "Music/WardenMusic.wav",
                   "Music/wardenMusic.wav",
                   "Music/Warden.wav",
                   0);

    // LAYER 3's own theme
    musicLoadTrack(MUSIC_LEVEL3,
                   "Music/Level3Music.wav",
                   "Music/level3Music.wav",
                   "Music/level3BGM.wav",
                   0);

    sfxInit();

    musicSetVolume(g_musicVolumePercent);
    atexit(musicShutdown);
}

#endif
