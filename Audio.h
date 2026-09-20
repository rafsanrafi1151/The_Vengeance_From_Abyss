
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
//    HITS   -> waveOut.  It opens its own output, so a hit plays ON TOP of
//              the PlaySound music instead of replacing it, and waveOutWrite
//              is asynchronous so it never holds the game up. Each sound gets
//              a few "voices" so two hits close together overlap instead of
//              cutting each other off.
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
    MUSIC_LEVEL3,      // level3BGM.wav     - the whole of layer 3
    MUSIC_LEVEL4,      // level4BGmusic.wav - the whole of layer 4
    MUSIC_COUNT
};

static unsigned char* g_musicData[MUSIC_COUNT] = { 0, 0, 0, 0, 0, 0 };
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
//  HISTORY, SO NOBODY REINTRODUCES EITHER BUG:
//
//   1. These used to play through MCI (mciSendString) on the game thread.
//      MCI is SYNCHRONOUS, so every punch froze the game for about a second.
//   2. Moving MCI onto a background thread fixed the freeze but broke the
//      game completely: MCI needs a message pump on the thread that calls it,
//      a worker thread has none, so opening a device blocked forever while
//      holding a winmm lock - and the game never even got a window.
//
//  So MCI is gone. These now use waveOut, which is asynchronous by design:
//  waveOutWrite hands the buffer to the driver and returns immediately. No
//  thread, no message pump, nothing to block on, and it mixes on top of the
//  PlaySound music instead of replacing it.
//
//  TO CHANGE A HIT SOUND: drop your .wav in the Music folder under the name
//  listed in sfxInit() below, or add your own name to that list.
// ===========================================================================
enum SoundId
{
    SFX_PLAYER_HIT = 0,   // playerHits.wav - the hero's kick connects
    SFX_ENEMY_HIT,        // enemyHits.wav  - an enemy connects with the hero
    SFX_COUNT
};

// How many copies of each sound can overlap. Three is plenty: hit the third
// enemy before the first sound has finished and it still plays.
const int SFX_VOICES     = 3;
const int SFX_CANDIDATES = 3;   // file names tried, in order, for each sound

struct WavData
{
    WAVEFORMATEX   fmt;
    unsigned char* pcm;
    unsigned long  bytes;
};

static WavData  g_sfxWav  [SFX_COUNT];
static HWAVEOUT g_sfxDev  [SFX_COUNT][SFX_VOICES];
static WAVEHDR  g_sfxHdr  [SFX_COUNT][SFX_VOICES];
static bool     g_sfxReady[SFX_COUNT][SFX_VOICES];
static int      g_sfxNextVoice[SFX_COUNT];
static const char* g_sfxPaths[SFX_COUNT][SFX_CANDIDATES];
static bool     g_sfxInitDone = false;

// Reads a .wav and pulls out its format and its raw samples. Walking the
// chunks properly matters: plenty of WAVs carry extra chunks before the
// samples, and assuming a flat 44-byte header would play those as noise.
inline bool wavLoad(const char* path, WavData& out)
{
    out.pcm = 0;
    out.bytes = 0;

    FILE* f = fopen(path, "rb");
    if (f == 0) return false;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size < 44) { fclose(f); return false; }

    unsigned char* buf = (unsigned char*)malloc((size_t)size);
    if (buf == 0) { fclose(f); return false; }
    size_t got = fread(buf, 1, (size_t)size, f);
    fclose(f);
    if (got != (size_t)size) { free(buf); return false; }

    if (memcmp(buf, "RIFF", 4) != 0 || memcmp(buf + 8, "WAVE", 4) != 0)
    { free(buf); return false; }          // renamed mp3 / aac lands here

    bool haveFmt = false;
    long p = 12;
    while (p + 8 <= size)
    {
        // read the 4-byte chunk size a byte at a time. memcpy into an
        // unsigned long would leave the top bytes uninitialised anywhere
        // unsigned long is wider than 4, and hand back a garbage length.
        unsigned long sz = (unsigned long)buf[p + 4]
                          | ((unsigned long)buf[p + 5] << 8)
                          | ((unsigned long)buf[p + 6] << 16)
                          | ((unsigned long)buf[p + 7] << 24);
        unsigned char* body = buf + p + 8;

        if (memcmp(buf + p, "fmt ", 4) == 0 && sz >= 16)
        {
            memset(&out.fmt, 0, sizeof(out.fmt));
            unsigned long n = (sz < sizeof(WAVEFORMATEX)) ? sz : sizeof(WAVEFORMATEX);
            memcpy(&out.fmt, body, n);
            out.fmt.cbSize = 0;
            haveFmt = true;
        }
        else if (memcmp(buf + p, "data", 4) == 0)
        {
            if (!haveFmt) break;
            unsigned long avail = (unsigned long)(size - (p + 8));
            out.bytes = (sz < avail) ? sz : avail;
            out.pcm = (unsigned char*)malloc(out.bytes);
            if (out.pcm == 0) { free(buf); return false; }
            memcpy(out.pcm, body, out.bytes);
            free(buf);
            return out.bytes > 0;
        }
        if (sz > (unsigned long)size) break;   // corrupt header, give up
        p += 8 + sz + (sz & 1);           // chunks are word aligned
    }

    free(buf);
    return false;
}

// Opens SFX_VOICES output devices for one sound. Tries each candidate name in
// turn: a file merely EXISTING is not enough, because an .mp3 or .aac renamed
// to .wav is there but will not load, and we must fall through to the next.
inline void sfxOpenSound(int id, FILE* log)
{
    g_sfxNextVoice[id] = 0;
    for (int v = 0; v < SFX_VOICES; v++) g_sfxReady[id][v] = false;

    for (int c = 0; c < SFX_CANDIDATES; c++)
    {
        const char* path = g_sfxPaths[id][c];
        if (path == 0) continue;
        if (!fileThere(path))
        {
            if (log) fprintf(log, "  \"%s\" - not in the folder\n", path);
            continue;
        }
        if (!wavLoad(path, g_sfxWav[id]))
        {
            if (log) fprintf(log, "  \"%s\" - NOT a real PCM .wav, skipped\n", path);
            continue;
        }

        int opened = 0;
        for (int v = 0; v < SFX_VOICES; v++)
        {
            HWAVEOUT dev = 0;
            MMRESULT r = waveOutOpen(&dev, WAVE_MAPPER, &g_sfxWav[id].fmt,
                                      0, 0, CALLBACK_NULL);
            if (r != MMSYSERR_NOERROR) continue;

            memset(&g_sfxHdr[id][v], 0, sizeof(WAVEHDR));
            g_sfxHdr[id][v].lpData         = (LPSTR)g_sfxWav[id].pcm;
            g_sfxHdr[id][v].dwBufferLength = g_sfxWav[id].bytes;

            if (waveOutPrepareHeader(dev, &g_sfxHdr[id][v], sizeof(WAVEHDR))
                != MMSYSERR_NOERROR)
            { waveOutClose(dev); continue; }

            g_sfxDev[id][v]   = dev;
            g_sfxReady[id][v] = true;
            opened++;
        }

        if (opened > 0)
        {
            if (log) fprintf(log, "  using \"%s\"  (%lu Hz, %d ch, %d bit) on %d voice(s)\n",
                             path, (unsigned long)g_sfxWav[id].fmt.nSamplesPerSec,
                             (int)g_sfxWav[id].fmt.nChannels,
                             (int)g_sfxWav[id].fmt.wBitsPerSample, opened);
            return;
        }
        if (log) fprintf(log, "  \"%s\" - no sound device would take it\n", path);
    }

    if (log) fprintf(log, "  NO usable file - this sound will be silent\n");
}

inline void sfxInit()
{
    if (g_sfxInitDone) return;
    g_sfxInitDone = true;

    for (int i = 0; i < SFX_COUNT; i++)
    {
        g_sfxWav[i].pcm = 0;
        g_sfxWav[i].bytes = 0;
        for (int v = 0; v < SFX_VOICES; v++) { g_sfxDev[i][v] = 0; g_sfxReady[i][v] = false; }
    }

    // The hero's kick connecting - the same sound in every level.
    // Your own file name is tried FIRST; the converted copy is the backup.
    g_sfxPaths[SFX_PLAYER_HIT][0] = "Music\\playerHits.wav";   // <-- your file
    g_sfxPaths[SFX_PLAYER_HIT][1] = "Music\\playerHit.wav";    // backup
    g_sfxPaths[SFX_PLAYER_HIT][2] = "Music\\PlayerHit.wav";

    // An enemy connecting with the hero - the same sound in every level.
    g_sfxPaths[SFX_ENEMY_HIT][0]  = "Music\\enemyHits.wav";    // <-- your file
    g_sfxPaths[SFX_ENEMY_HIT][1]  = "Music\\enemyHit.wav";     // backup
    g_sfxPaths[SFX_ENEMY_HIT][2]  = "Music\\EnemyHit.wav";

    // A one-off note of what loaded, so silence is easy to diagnose.
    // Open Music\sound_log.txt after running the game.
    FILE* log = fopen("Music\\sound_log.txt", "w");
    if (log) fprintf(log, "HIT SOUNDS\n\nplayerHits:\n");
    sfxOpenSound(SFX_PLAYER_HIT, log);
    if (log) fprintf(log, "\nenemyHits:\n");
    sfxOpenSound(SFX_ENEMY_HIT, log);
    if (log) fclose(log);
}

// Called from the game when a hit lands. waveOutWrite queues the buffer with
// the driver and returns straight away, so this costs the frame nothing.
inline void sfxPlay(int id)
{
    if (id < 0 || id >= SFX_COUNT) return;

    // first choice: a voice that is not already playing, so hits overlap
    for (int v = 0; v < SFX_VOICES; v++)
    {
        if (!g_sfxReady[id][v]) continue;
        if (g_sfxHdr[id][v].dwFlags & WHDR_INQUEUE) continue;
        waveOutWrite(g_sfxDev[id][v], &g_sfxHdr[id][v], sizeof(WAVEHDR));
        return;
    }

    // every voice is busy: restart the next one in the rotation
    for (int tries = 0; tries < SFX_VOICES; tries++)
    {
        int v = g_sfxNextVoice[id];
        g_sfxNextVoice[id] = (v + 1) % SFX_VOICES;
        if (!g_sfxReady[id][v]) continue;
        waveOutReset(g_sfxDev[id][v]);          // returns the buffer to us
        waveOutWrite(g_sfxDev[id][v], &g_sfxHdr[id][v], sizeof(WAVEHDR));
        return;
    }
}

inline void sfxShutdown()
{
    for (int id = 0; id < SFX_COUNT; id++)
    {
        for (int v = 0; v < SFX_VOICES; v++)
        {
            if (!g_sfxReady[id][v]) continue;
            waveOutReset(g_sfxDev[id][v]);
            waveOutUnprepareHeader(g_sfxDev[id][v], &g_sfxHdr[id][v], sizeof(WAVEHDR));
            waveOutClose(g_sfxDev[id][v]);
            g_sfxReady[id][v] = false;
            g_sfxDev[id][v] = 0;
        }
        if (g_sfxWav[id].pcm) { free(g_sfxWav[id].pcm); g_sfxWav[id].pcm = 0; }
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

    // LAYER 3's own theme. Your own file name is tried FIRST.
    musicLoadTrack(MUSIC_LEVEL3,
                   "Music/level3BGM.wav",     // <-- your file
                   "Music/Level3Music.wav",   // backup, already in the folder
                   "Music/level3Music.wav",
                   0);

    // LAYER 4's own theme. Your own file name is tried FIRST.
    musicLoadTrack(MUSIC_LEVEL4,
                   "Music/level4BGmusic.wav",   // <-- your file
                   "Music/Level4Music.wav",     // backup, already in the folder
                   "Music/level4Music.wav",
                   0);

    sfxInit();

    musicSetVolume(g_musicVolumePercent);
    atexit(musicShutdown);
}

#endif
