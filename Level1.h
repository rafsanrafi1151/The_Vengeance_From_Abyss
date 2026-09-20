
#pragma once
#ifndef LEVEL1_H
#define LEVEL1_H

#include "GameUtility.h"
#include "Player.h"
#include "Enemy.h"

extern int bgImageLv1;
extern unsigned int bgImageLv2;
extern unsigned int bgImageLv3;   // defined in Level3.h
extern unsigned int bgImageLv4;   // defined in Level4.h

// ---------------------------------------------------------------------------
//  WHAT KARL SAYS WHEN HE LANDS IN THE ABYSS
//
//  Played once, before the first wave walks in. Edit the words here; add or
//  remove a { SPK_KARL, "..." } line and the count looks after itself.
//  Plain keyboard characters only - write " ' and ... , never the curly kind.
// ---------------------------------------------------------------------------
const DialogueLine LEVEL1_DIALOGUE[] =
{
    { SPK_KARL, "So... this is the Abyss." },
    { SPK_KARL, "They threw me here to die." },
    { SPK_KARL, "But I'm not dying here." },
    { SPK_KARL, "I'll find the truth." },
    { SPK_KARL, "And whoever did this will pay." },
};
const int LEVEL1_DIALOGUE_COUNT =
    (int)(sizeof(LEVEL1_DIALOGUE) / sizeof(LEVEL1_DIALOGUE[0]));


enum WaveState { WAVE_ACTIVE, WAVE_CLEARED_PAUSE, WAVE_LEVEL_DONE };

int       waveIndex   = 0;
WaveState waveState   = WAVE_ACTIVE;
int       waveClearTimer = 0;
bool      level1Opened   = false;   // has the opening conversation finished?

Button pauseButtons[2] = {
    { (SCREEN_WIDTH - 250) / 2.0, 296, 250, 54, "RESUME", false },
    { (SCREEN_WIDTH - 250) / 2.0, 228, 250, 54, "MAIN MENU", false }
};
Button gameOverButtons[2] = {
    { (SCREEN_WIDTH - 250) / 2.0, 216, 250, 54, "RETRY", false },
    { (SCREEN_WIDTH - 250) / 2.0, 148, 250, 54, "MAIN MENU", false }
};
Button levelCompleteButtons[2] = {
    { (SCREEN_WIDTH - 250) / 2.0, 160, 250, 54, "CONTINUE", false },
    { (SCREEN_WIDTH - 250) / 2.0,  92, 250, 54, "MAIN MENU", false }
};


void resetGame()
{
    currentLevel = 1;
    resetPlayer();
    killCount = 0;
    resetRunTimer();
    waveIndex = 0;
    waveState = WAVE_ACTIVE;
    waveClearTimer = 0;
    hitPauseTimer = 0;

    // Karl speaks first and the arena stays empty while he does. The wave is
    // spawned by updateLevel1 the moment the conversation closes, so the
    // enemies cannot walk in over the top of it.
    enemies.clear();
    clearWaveQueue();
    level1Opened = false;
    startDialogue(LEVEL1_DIALOGUE, LEVEL1_DIALOGUE_COUNT);
}

void startNewGameFromMenu()
{
    resetGame();
    gameState = STATE_LEVEL1;

    // START_LEVEL in LevelSettings.h lets you jump straight to a later layer
    // while you are working on it. It is 1 for a normal run.
    if (START_LEVEL == 2)      startLevel2();
    else if (START_LEVEL == 3) startLevel3();
    else if (START_LEVEL >= 4) startLevel4();
}


void updateLevel1()
{
    // a conversation freezes the layer: nothing moves, nothing can hit you
    if (dialogueActive()) { updateDialogue(); return; }

    if (!level1Opened)
    {
        level1Opened = true;
        queueWave(waveIndex);       // the first wave, now that Karl has spoken
        return;
    }

    updatePlayerInput();
    updateEnemies();

    if (gameState != STATE_LEVEL1)
        return; 

    if (waveState == WAVE_ACTIVE)
    {
        // let the next one of this wave in when there is room for him
        updateWaveFeed();

        if (waveFinished())
        {
            if (waveIndex + 1 < LEVEL1_WAVE_COUNT)
            {
                waveState = WAVE_CLEARED_PAUSE;
                waveClearTimer = WAVE_CLEAR_DELAY_TICKS;
            }
            else
            {
                // the layer is over. Hold for one second so the last kill
                // lands before the result screen takes over - layer 1 used to
                // cut away the instant the enemy fell.
                waveState      = WAVE_LEVEL_DONE;
                waveClearTimer = RESULT_SCREEN_DELAY_TICKS;
            }
        }
    }
    else if (waveState == WAVE_LEVEL_DONE)
    {
        waveClearTimer--;
        if (waveClearTimer <= 0)
            gameState = STATE_LEVEL_COMPLETE;
    }
    else 
    {
        waveClearTimer--;
        if (waveClearTimer <= 0)
        {
            waveIndex++;
            queueWave(waveIndex);
            waveState = WAVE_ACTIVE;
        }
    }
}


void drawArenaBackdrop()
{
    iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImageLv1);

    // level1bg.png draws its own road and sky, so nothing is painted over it.
    // (The old grey floor strip and ceiling teeth lived here.)
}

void drawHUD()
{
    
    // The hero's bar is now the level4HPBar.png artwork, in EVERY layer.
    // If that art is missing it falls back to the old plain rectangle.
    double pct = clampd(player.health / PLAYER_MAX_HEALTH, 0.0, 1.0);
    if (!drawImageBar(heroBarArt, HERO_BAR_X, HERO_BAR_Y,
                       HERO_BAR_W, HERO_BAR_H, pct))
    {
        iSetColor(60, 60, 60);
        iFilledRectangle(20, SCREEN_HEIGHT - 40, 220, 22);
        iSetColor(180, 20, 20);
        iFilledRectangle(20, SCREEN_HEIGHT - 40, 220 * pct, 22);
        iSetColor(220, 220, 220);
        iRectangle(20, SCREEN_HEIGHT - 40, 220, 22);
    }

    char healthLabel[32];
    sprintf(healthLabel, "%d / %d", (int)player.health, (int)PLAYER_MAX_HEALTH);
    if (gameFontTex != 0)
        drawGameText(HERO_BAR_TEXT_X, HERO_BAR_TEXT_Y, healthLabel,
                      HUD_HP_SIZE, 232, 216, 200);
    else
    { iSetColor(225, 210, 200);
      iText(HERO_BAR_TEXT_X, HERO_BAR_TEXT_Y, healthLabel, GLUT_BITMAP_8_BY_13); }

    
    // what is on screen plus what is still queued behind them, so the
    // number means "left in this wave", not "left in the arena"
    int aliveCount = aliveEnemyCount() + waveStillToCome();

    char waveLabel[48];
    sprintf(waveLabel, "LAYER 1 -- WAVE %d / %d", waveIndex + 1, LEVEL1_WAVE_COUNT);
    drawStatusText(SCREEN_WIDTH / 2.0, HUD_TITLE_Y, waveLabel, HUD_TITLE_SIZE, 226, 210, 184);

    char remainLabel[32];
    if (waveState == WAVE_ACTIVE)
        sprintf(remainLabel, "hostiles remaining: %d", aliveCount);
    else
        sprintf(remainLabel, "wave cleared...");
    iSetColor(150, 150, 150);
    drawStatusText(SCREEN_WIDTH / 2.0, HUD_SUB_Y, remainLabel, HUD_SUB_SIZE, 176, 164, 150);

    
    char killLabel[32];
    sprintf(killLabel, "KILLS  %d", killCount);
    if (gameFontTex != 0)
        drawGameTextRight(HUD_KILLS_RIGHT, HUD_KILLS_Y, killLabel,
                           HUD_KILLS_SIZE, 214, 198, 172);
    else
    { iSetColor(200, 200, 200);
      iText(SCREEN_WIDTH - 130, SCREEN_HEIGHT - 30, killLabel, GLUT_BITMAP_8_BY_13); }
}

void drawHitBurst()
{
    if (hitPauseTimer <= 0) return;

    int elapsed = HIT_PAUSE_TICKS - hitPauseTimer;
    iSetColor(235, 235, 235);
    iCircle(lastHitX, lastHitY, 10 + elapsed * 6, 16);
    iCircle(lastHitX, lastHitY, 4 + elapsed * 3, 12);
}

void drawDamageVignette()
{
    if (player.hitFlashTimer <= 0) return;
    if ((player.hitFlashTimer / 2) % 2 != 0) return;

    iSetColor(140, 0, 0);
    for (int i = 0; i < 4; i++)
        iRectangle(4 + i, 4 + i, SCREEN_WIDTH - 8 - 2 * i, SCREEN_HEIGHT - 8 - 2 * i);
}

void drawLevel1()
{
    iClear();
    drawArenaBackdrop();

    drawAllEnemies();
    drawPlayer();
    drawHitBurst();

    drawHUD();
    drawDamageVignette();
    drawDialogue();
}

void drawPauseOverlay()
{
    // The "PAUSED" plate used to sit above the buttons. It is gone: the two
    // buttons are the whole pause screen now, over a dimmed level.
    dimScreen(0.62);

    // RESUME is drawn from Resume.png, which already has its own lettering,
    // so nothing is written over the top of it. Its rectangle is unchanged, so
    // the button is exactly where it has always been and the mouse still hits
    // it. MAIN MENU keeps the ordinary button.
    if (resumeButtonArt.tex != 0) drawButtonPicture(pauseButtons[0], resumeButtonArt);
    else                          drawButton(pauseButtons[0]);

    drawButton(pauseButtons[1]);
}

void drawGameOverScreen()
{
    iClear();
    unsigned int deathBg = (unsigned int)bgImageLv1;
    if (currentLevel == 2 && bgImageLv2 != 0) deathBg = bgImageLv2;
    if (currentLevel == 3 && bgImageLv3 != 0) deathBg = bgImageLv3;
    if (currentLevel == 4 && bgImageLv4 != 0) deathBg = bgImageLv4;
    iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, deathBg);
    dimScreen(0.74);

    double cx = SCREEN_WIDTH / 2.0;

    char runTime[16];
    formatRunTime(runTime);

    // ---- the plate ------------------------------------------------------
    // failedMessageMessage.png already carries the words YOU HAVE FALLEN INTO
    // THE ABYSS. What it cannot know is how the run actually went, so the
    // kills, the clock, the layer and the health left are written into the two
    // empty slots it was drawn with - the framed box in the middle, and the
    // line under it. See DEATH_BOX_* in GameUtility.h to move them.
    double flavourY;
    if (deathBarArt.tex != 0)
    {
        double bw = 720.0;
        double bh = bw / deathBarArt.aspect;
        if (bh > 250.0) { bh = 250.0; bw = bh * deathBarArt.aspect; }

        double bx = cx - bw / 2.0;
        double by = 330.0;

        drawSpriteUV(deathBarArt.tex, bx, by, bw, bh, false,
                      deathBarArt.u0, deathBarArt.v0,
                      deathBarArt.u1, deathBarArt.v1);

        // the framed box: what you took down, and how long you lasted
        double boxY0 = by + DEATH_BOX_V0 * bh;
        double boxY1 = by + DEATH_BOX_V1 * bh;
        double boxH  = boxY1 - boxY0;

        char inner[96];
        sprintf(inner, "KILLS  %d        TIME  %s", killCount, runTime);
        double innerSize = boxH * 0.60;
        if (innerSize > 28.0) innerSize = 28.0;
        drawStatusText(cx, boxY0 + (boxH - innerSize) / 2.0 + innerSize * 0.14,
                        inner, innerSize, 226, 208, 186);

        // the line under it: how deep you got, and the bar you died on
        char under[96];
        sprintf(under, "LAYER %d        0 / %d HEALTH",
                 currentLevel, (int)PLAYER_MAX_HEALTH);
        drawStatusText(cx, by + DEATH_SUB_V * bh, under, 21, 200, 152, 138);

        flavourY = by - 30.0;
    }
    else
    {
        double pw = 600, ph = 120;
        double px = cx - pw / 2.0, py = 420;
        drawPanel(px, py, pw, ph, 170, 30, 30);
        drawStatusText(cx + 2, py + ph - 74, "YOU HAVE FALLEN INTO THE ABYSS", 34, 0, 0, 0);
        drawStatusText(cx,     py + ph - 72, "YOU HAVE FALLEN INTO THE ABYSS", 34, 215, 60, 55);

        double barW = 300.0, barH = barW / 4.2857;
        drawImageBar(heroBarArt, cx - barW / 2.0, py - 10.0 - barH, barW, barH, 0.0);

        char line1[96];
        sprintf(line1, "LAYER %d        0 / %d HEALTH", currentLevel, (int)PLAYER_MAX_HEALTH);
        drawStatusText(cx, py - 44.0 - barH, line1, 24, 214, 196, 176);

        char line2[96];
        sprintf(line2, "KILLS  %d        TIME  %s", killCount, runTime);
        drawStatusText(cx, py - 74.0 - barH, line2, 24, 214, 196, 176);

        flavourY = py - 108.0 - barH;
    }

    // ---- one line of flavour, under the plate ---------------------------
    char flavour[96];
    if (currentLevel == 1)
        sprintf(flavour, "you fell on wave %d of %d", waveIndex + 1, LEVEL1_WAVE_COUNT);
    else
        sprintf(flavour, "the Abyss keeps what it takes");
    drawStatusText(cx, flavourY, flavour, 18, 150, 140, 132);

    for (int i = 0; i < 2; i++)
        drawButton(gameOverButtons[i]);
}

int levelCompleteHover = -1;

// The numbers that were printed into these pictures have been wiped off them,
// and the real ones from THIS run are written back in the same place. The
// positions are fractions of each picture, so they hold wherever it is drawn.
inline void drawLayer1ClearedStats()
{
    char t[16], line[80];
    formatRunTime(t);
    sprintf(line, "HOSTILES PURGED: %d      TIME %s", killCount, t);
    drawScreenArtText(artLayer1Cleared, 0.500, 0.600, line, 0.056, true);
}

inline void drawLayer3UnlockedStats()
{
    char t[16], line[80];
    formatRunTime(t);
    sprintf(line, "HOSTILES PURGED: %d      TIME %s", killCount, t);
    drawScreenArtText(artLayer3Unlocked, 0.500, 0.572, line, 0.064, true);
}

inline void drawLayer4UnlockedStats()
{
    char t[16], a[64], b[64];
    formatRunTime(t);
    sprintf(a, "%d       TIME %s", killCount, t);
    sprintf(b, "%d / %d", (int)player.health, (int)PLAYER_MAX_HEALTH);
    drawScreenArtText(artLayer4Unlocked, 0.395, 0.351, a, 0.060, false);
    drawScreenArtText(artLayer4Unlocked, 0.395, 0.392, b, 0.060, false);
}

inline void drawGameFinishedStats()
{
    char t[16], a[64], b[64];
    formatRunTime(t);
    sprintf(a, "%d       TIME %s", killCount, t);
    sprintf(b, "%d / %d", (int)player.health, (int)PLAYER_MAX_HEALTH);
    drawScreenArtText(artGameFinished, 0.373, 0.383, a, 0.068, false);
    drawScreenArtText(artGameFinished, 0.373, 0.449, b, 0.068, false);
}

void drawLevelCompleteScreen()
{
    iClear();

    // leyarr1Cleared.png is a finished screen, buttons and all, so it is drawn
    // whole instead of the painted panel that used to be here.
    if (artLayer1Cleared.art.tex != 0)
    {
        iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (unsigned int)bgImageLv1);
        dimScreen(0.80);
        drawScreenArt(artLayer1Cleared);
        drawScreenArtHover(artLayer1Cleared, levelCompleteHover);
        drawLayer1ClearedStats();
        return;
    }

    // ---- fallback: the old painted panel, if the picture is missing ------
    iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImageLv1);
    dimScreen(0.66);

    double pw = 640, ph = 300;
    double px = (SCREEN_WIDTH - pw) / 2.0, py = 250;
    drawPanel(px, py, pw, ph, 205, 55, 45);
    double cx = SCREEN_WIDTH / 2.0;

    drawStatusText(cx + 2, py + ph - 52, "LAYER ONE CLEARED", 36, 0, 0, 0);
    drawStatusText(cx,     py + ph - 50, "LAYER ONE CLEARED", 36, 240, 205, 105);
    fillRectAlpha(px + 90, py + ph - 66, pw - 180, 2, 205, 90, 60, 0.75);

    drawStatusText(cx, py + ph - 100, "LAYER 2 UNLOCKED  --  THE WARDEN'S GATE",
                    20, 225, 205, 200);

    char killLabel[48];
    sprintf(killLabel, "Hostiles purged: %d", killCount);
    drawStatusText(cx, py + 26, killLabel, 20, 190, 165, 120);

    for (int i = 0; i < 2; i++)
        drawButton(levelCompleteButtons[i]);
}


void updatePauseHover(int mx, int my)
{
    for (int i = 0; i < 2; i++)
        pauseButtons[i].hover = pointInButton(pauseButtons[i], mx, my);
}
void updateGameOverHover(int mx, int my)
{
    for (int i = 0; i < 2; i++)
        gameOverButtons[i].hover = pointInButton(gameOverButtons[i], mx, my);
}
void updateLevelCompleteHover(int mx, int my)
{
    if (artLayer1Cleared.art.tex != 0)
    {
        levelCompleteHover = -1;
        for (int i = 0; i < artLayer1Cleared.buttonCount; i++)
            if (screenArtHit(artLayer1Cleared, i, mx, my)) levelCompleteHover = i;
        return;
    }
    for (int i = 0; i < 2; i++)
        levelCompleteButtons[i].hover = pointInButton(levelCompleteButtons[i], mx, my);
}

void handlePauseClick(int mx, int my)
{
    if (pointInButton(pauseButtons[0], mx, my))      gameState = stateBeforePause;
    else if (pointInButton(pauseButtons[1], mx, my)) gameState = STATE_MENU;
}
void handleGameOverClick(int mx, int my)
{
    // RETRY restarts whichever layer you actually died on
    if (pointInButton(gameOverButtons[0], mx, my))      retryCurrentLevel();
    else if (pointInButton(gameOverButtons[1], mx, my)) gameState = STATE_MENU;
}
void handleLevelCompleteClick(int mx, int my)
{
    if (artLayer1Cleared.art.tex != 0)
    {
        if (screenArtHit(artLayer1Cleared, 0, mx, my))      startLevel2();
        else if (screenArtHit(artLayer1Cleared, 1, mx, my)) gameState = STATE_MENU;
        return;
    }
    if (pointInButton(levelCompleteButtons[0], mx, my))       startLevel2();
    else if (pointInButton(levelCompleteButtons[1], mx, my))  gameState = STATE_MENU;
}

#endif 
