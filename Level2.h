
#pragma once
#ifndef LEVEL2_H
#define LEVEL2_H

#include "GameUtility.h"
#include "Player.h"
#include "Enemy.h"
#include "Level1.h"

// ---------------------------------------------------------------------------
//  LAYER 2 -- THE WARDEN'S GATE
//
//  Flow:
//    1. three level-1 style enemies walk in
//    2. once they are down there is a short beat  ("THE WARDEN APPROACHES")
//    3. the new enemy character arrives with 200 health
//    4. put him down and the run is complete
//
//  The player always enters this level on FULL health (startLevel2 calls
//  resetPlayer, which restores PLAYER_MAX_HEALTH).
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  THE WARDEN AT THE GATE
//
//  Played once, after the waves are cleared and the Warden has walked in. The
//  fight is frozen while it runs and starts the moment it closes.
//  Plain keyboard characters only - write " ' and ... , never the curly kind.
// ---------------------------------------------------------------------------
const DialogueLine LEVEL2_DIALOGUE[] =
{
    { SPK_WARDEN, "So... you've made it this far." },
    { SPK_KARL,   "Move aside." },
    { SPK_WARDEN, "You think you can simply pass through?" },
    { SPK_KARL,   "I didn't come here to stop." },
    { SPK_WARDEN, "Beyond this gate lies a place few have ever returned from." },
    { SPK_KARL,   "Then I'll be the first." },
    { SPK_WARDEN, "You'll regret taking another step." },
    { SPK_KARL,   "We'll see." },
};
const int LEVEL2_DIALOGUE_COUNT =
    (int)(sizeof(LEVEL2_DIALOGUE) / sizeof(LEVEL2_DIALOGUE[0]));


enum Level2Phase
{
    L2_PHASE_GRUNTS,      // the three ordinary enemies
    L2_PHASE_BOSS_INTRO,  // short pause before the Warden arrives
    L2_PHASE_BOSS_TALK,   // he is on screen, the two of them speak
    L2_PHASE_BOSS,        // fighting the 200 HP enemy
    L2_PHASE_DONE         // he is down, running the victory beat
};

Level2Phase level2Phase   = L2_PHASE_GRUNTS;
int         level2Timer   = 0;
int         level2Tick    = 0;    // free-running, used for the intro flicker
int         level2Wave    = 0;    // which row of LEVEL2_WAVES we are on
int         level2WaveGap = 0;    // countdown between waves

extern int bgImageLv1;
unsigned int bgImageLv2 = 0;      // optional Images/gameplay2.png

// This screen is no longer the end of the run - it now leads into layer 3.
Button gameCompleteButtons[2] = {
    { (SCREEN_WIDTH - 250) / 2.0, 158, 250, 54, "CONTINUE", false },
    { (SCREEN_WIDTH - 250) / 2.0,  92, 250, 54, "MAIN MENU", false }
};

void loadLevel2Images()
{
    // TO CHANGE THE LEVEL 2 BACKGROUND: put your PNG in the Images folder
    // using any of these names, or add your own file name to this list.
    // If none of them exist, the level-1 backdrop is reused, darkened.
    bgImageLv2 = iLoadImageIfExists("Images/level2bg.png");
    if (bgImageLv2 == 0) bgImageLv2 = iLoadImageIfExists("Images/bglevel2.png");
    if (bgImageLv2 == 0) bgImageLv2 = iLoadImageIfExists("Images/gameplay2.png");
    if (bgImageLv2 == 0) bgImageLv2 = iLoadImageIfExists("Images/level2.png");
    if (bgImageLv2 == 0) bgImageLv2 = iLoadImageIfExists("Images/Level2.png");
}


// ---------------------------------------------------------------------------
//  ENTRY POINTS
// ---------------------------------------------------------------------------
void startLevel2()
{
    currentLevel = 2;

    resetPlayer();          // <-- full health on entering level 2
    enemies.clear();
    endDialogue();          // drop anything left over from the layer before

    level2Phase   = L2_PHASE_GRUNTS;
    level2Timer   = 0;
    level2Tick    = 0;
    level2Wave    = 0;
    level2WaveGap = 0;
    hitPauseTimer = 0;

    // the opening enemies come from LEVEL2_WAVES in LevelSettings.h. They are
    // queued rather than spawned, so they walk in a couple at a time.
    queueWaveFrom(LEVEL2_WAVES, LEVEL2_WAVE_COUNT, 0);

    gameState = STATE_LEVEL2;
}

// used by the RETRY button so you restart the layer you actually died on
void retryCurrentLevel()
{
    if (currentLevel == 4)      startLevel4();
    else if (currentLevel == 3) startLevel3();
    else if (currentLevel == 2) startLevel2();
    else                        startNewGameFromMenu();
}


// ---------------------------------------------------------------------------
//  UPDATE
// ---------------------------------------------------------------------------
void updateLevel2()
{
    level2Tick++;

    // a conversation freezes the layer: nothing moves, nothing can hit you
    if (dialogueActive()) { updateDialogue(); return; }

    // the conversation has just closed - the duel begins
    if (level2Phase == L2_PHASE_BOSS_TALK)
        level2Phase = L2_PHASE_BOSS;

    updatePlayerInput();
    updateEnemies();

    if (gameState != STATE_LEVEL2)
        return;                       // the player died mid-frame

    switch (level2Phase)
    {
    case L2_PHASE_GRUNTS:
        if (level2WaveGap > 0)
        {
            // short breather, then the next row of LEVEL2_WAVES walks in
            level2WaveGap--;
            if (level2WaveGap == 0)
            {
                level2Wave++;
                queueWaveFrom(LEVEL2_WAVES, LEVEL2_WAVE_COUNT, level2Wave);
            }
        }
        else
        {
            // let the next one of this wave in when there is room for him
            updateWaveFeed();

            if (waveFinished())
            {
                if (level2Wave + 1 < LEVEL2_WAVE_COUNT)
                {
                    level2WaveGap = WAVE_CLEAR_DELAY_TICKS;
                }
                else
                {
                    level2Phase = L2_PHASE_BOSS_INTRO;
                    level2Timer = L2_BOSS_INTRO_TICKS;
                }
            }
        }
        break;

    case L2_PHASE_BOSS_INTRO:
        level2Timer--;
        if (level2Timer <= 0)
        {
            enemies.clear();          // drop the corpses before the duel
            spawnBrute();             // the new enemy character, 200 HP

            // He normally walks in from off screen, which would put his half
            // of the conversation off the edge of the picture. For the talk he
            // is placed where he can be seen; he moves normally again the
            // moment the fight starts.
            for (size_t i = 0; i < enemies.size(); i++)
                if (enemies[i].type == ENEMY_BRUTE)
                {
                    enemies[i].x = ARENA_RIGHT - 170.0;
                    enemies[i].facingRight = false;
                }

            level2Phase = L2_PHASE_BOSS_TALK;
            startDialogue(LEVEL2_DIALOGUE, LEVEL2_DIALOGUE_COUNT);
        }
        break;

    case L2_PHASE_BOSS_TALK:
        break;                        // handled above, before the layer updates

    case L2_PHASE_BOSS:
        if (!bossIsAlive())
        {
            level2Phase = L2_PHASE_DONE;
            level2Timer = L2_BOSS_DEATH_TICKS;
        }
        break;

    case L2_PHASE_DONE:
        level2Timer--;
        if (level2Timer <= 0)
            gameState = STATE_GAME_COMPLETE;
        break;
    }
}


// ---------------------------------------------------------------------------
//  DRAW
// ---------------------------------------------------------------------------
void drawLevel2Backdrop()
{
    if (bgImageLv2 != 0)
    {
        // a real level-2 image - show it exactly as drawn, nothing painted over
        iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImageLv2);
        return;
    }

    // ---- no level-2 art found: reuse level 1, darkened -------------------
    iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImageLv1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4d(0.05, 0.02, 0.10, 0.45);
    glBegin(GL_QUADS);
        glVertex2d(0, 0);
        glVertex2d(SCREEN_WIDTH, 0);
        glVertex2d(SCREEN_WIDTH, SCREEN_HEIGHT);
        glVertex2d(0, SCREEN_HEIGHT);
    glEnd();
    glColor4d(1, 1, 1, 1);
    glDisable(GL_BLEND);

    iSetColor(14, 12, 20);
    double topX[8] = { 0, 120, 240, 400, 560, 720, 880, SCREEN_WIDTH };
    double topY[8] = { SCREEN_HEIGHT, SCREEN_HEIGHT - 55, SCREEN_HEIGHT - 20,
                        SCREEN_HEIGHT - 70, SCREEN_HEIGHT - 28, SCREEN_HEIGHT - 75,
                        SCREEN_HEIGHT - 35, SCREEN_HEIGHT };
    iFilledPolygon(topX, topY, 8);

    iSetColor(38, 34, 46);
    iFilledRectangle(0, 0, SCREEN_WIDTH, ARENA_BOTTOM - 6);
    iSetColor(80, 60, 60);
    iLine(0, ARENA_BOTTOM - 6, SCREEN_WIDTH, ARENA_BOTTOM - 6);
}

void drawBossHealthBar()
{
    const Enemy* boss = 0;
    for (size_t i = 0; i < enemies.size(); i++)
        if (enemies[i].type == ENEMY_BRUTE) { boss = &enemies[i]; break; }

    if (boss == 0) return;

    // sizes come from ENEMY_BAR_SCALE in GameUtility.h
    double w = BOSS_BAR_W;
    double h = BOSS_BAR_H;
    double x = BOSS_BAR_X;
    double y = BOSS_BAR_Y;              // clear of the hero's bar above it
    double pct = clampd(boss->health / boss->maxHealth, 0.0, 1.0);

    // leve2BossHealth.png. The picture already carries his name.
    if (!drawImageBar(wardenBarArt, x, y, w, h, pct))
    {
        iSetColor(25, 25, 25);
        iFilledRectangle(x, y, w, 16);
        if (boss->enraged) iSetColor(240, 90, 40);
        else               iSetColor(170, 25, 25);
        iFilledRectangle(x, y, w * pct, 16);
        iSetColor(210, 210, 210);
        iRectangle(x, y, w, 16);
    }

    char label[64];
    sprintf(label, "%d / %d", (int)(boss->health + 0.5), (int)boss->maxHealth);
    drawStatusText(SCREEN_WIDTH / 2.0, y - 26, label, HUD_BOSSLBL_SIZE, 225, 205, 200);
}

void drawLevel2HUD()
{
    // player health
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

    // layer / phase
    drawStatusText(SCREEN_WIDTH / 2.0, HUD_TITLE_Y,
                    "LAYER 2 -- THE WARDEN'S GATE", HUD_TITLE_SIZE, 226, 210, 184);

    char remainLabel[64];
    if (level2Phase == L2_PHASE_GRUNTS)
        sprintf(remainLabel, "wave %d / %d   --   hostiles remaining: %d",
                level2Wave + 1, LEVEL2_WAVE_COUNT,
                aliveEnemyCount() + waveStillToCome());
    else if (level2Phase == L2_PHASE_BOSS_INTRO)
        sprintf(remainLabel, "something heavy is coming...");
    else if (level2Phase == L2_PHASE_BOSS_TALK)
        sprintf(remainLabel, "the Warden blocks the gate");
    else if (level2Phase == L2_PHASE_BOSS)
        sprintf(remainLabel, "bring him down");
    else
        sprintf(remainLabel, "the gate is open");

    drawStatusText(SCREEN_WIDTH / 2.0, HUD_SUB_Y, remainLabel, HUD_SUB_SIZE, 176, 164, 150);

    char killLabel[32];
    sprintf(killLabel, "KILLS  %d", killCount);
    if (gameFontTex != 0)
        drawGameTextRight(HUD_KILLS_RIGHT, HUD_KILLS_Y, killLabel,
                           HUD_KILLS_SIZE, 214, 198, 172);
    else
    { iSetColor(200, 200, 200);
      iText(SCREEN_WIDTH - 130, SCREEN_HEIGHT - 30, killLabel, GLUT_BITMAP_8_BY_13); }

    drawSpecialCue();   // "H  SPECIAL" / the cooldown, on its own line below

    if (level2Phase == L2_PHASE_BOSS_TALK || level2Phase == L2_PHASE_BOSS ||
        level2Phase == L2_PHASE_DONE)
        drawBossHealthBar();
}

void drawBossIntroBanner()
{
    if (level2Phase != L2_PHASE_BOSS_INTRO) return;

    // pulse it so it reads as a warning
    double bR, bG, bB;
    if ((level2Tick / 8) % 2 == 0) { bR = 235; bG = 70;  bB = 55; }
    else                           { bR = 150; bG = 35;  bB = 28; }

    drawStatusText(SCREEN_WIDTH / 2.0, 380, "THE WARDEN APPROACHES", HUD_BANNER_SIZE, bR, bG, bB);

    drawStatusText(SCREEN_WIDTH / 2.0, 344,
                    "he winds up before he swings -- jump it", HUD_BANNER_SUB, 186, 172, 168);
}

void drawLevel2()
{
    iClear();
    drawLevel2Backdrop();

    drawAllEnemies();
    drawPlayer();
    drawHitBurst();

    drawBossIntroBanner();
    drawLevel2HUD();
    drawDialogue();
    drawDamageVignette();
}


// ---------------------------------------------------------------------------
//  RUN COMPLETE SCREEN
// ---------------------------------------------------------------------------
int gameCompleteHover = -1;

void drawGameCompleteScreen()
{
    iClear();

    // layer3Unlcoked.png: a finished screen with its own buttons
    if (artLayer3Unlocked.art.tex != 0)
    {
        if (bgImageLv2 != 0) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImageLv2);
        else                 iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (unsigned int)bgImageLv1);
        dimScreen(0.80);
        drawScreenArt(artLayer3Unlocked);
        drawScreenArtHover(artLayer3Unlocked, gameCompleteHover);
        drawLayer3UnlockedStats();
        return;
    }

    // ---- fallback: the old painted panel --------------------------------
    if (bgImageLv2 != 0) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImageLv2);
    else                 iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImageLv1);
    dimScreen(0.68);

    double pw = 660, ph = 300;
    double px = (SCREEN_WIDTH - pw) / 2.0, py = 250;
    drawPanel(px, py, pw, ph, 235, 175, 60);
    double cx = SCREEN_WIDTH / 2.0;

    drawStatusText(cx, py + ph - 50, "THE WARDEN HAS FALLEN", 36, 245, 200, 80);
    drawStatusText(cx, py + ph - 100, "LAYER 3 UNLOCKED  --  THE ROT AND THE HOUND",
                    20, 225, 215, 200);

    char killLabel[48];
    sprintf(killLabel, "Hostiles purged: %d", killCount);
    drawStatusText(cx, py + 26, killLabel, 20, 190, 165, 120);

    for (int i = 0; i < 2; i++)
        drawButton(gameCompleteButtons[i]);
}

void updateGameCompleteHover(int mx, int my)
{
    if (artLayer3Unlocked.art.tex != 0)
    {
        gameCompleteHover = -1;
        for (int i = 0; i < artLayer3Unlocked.buttonCount; i++)
            if (screenArtHit(artLayer3Unlocked, i, mx, my)) gameCompleteHover = i;
        return;
    }
    for (int i = 0; i < 2; i++)
        gameCompleteButtons[i].hover = pointInButton(gameCompleteButtons[i], mx, my);
}

void handleGameCompleteClick(int mx, int my)
{
    // CONTINUE now drops you into layer 3 instead of ending the run
    if (artLayer3Unlocked.art.tex != 0)
    {
        if (screenArtHit(artLayer3Unlocked, 0, mx, my)) startLevel3();
        else if (screenArtHit(artLayer3Unlocked, 1, mx, my))
        { currentLevel = 1; gameState = STATE_MENU; }
        return;
    }
    if (pointInButton(gameCompleteButtons[0], mx, my))
    {
        startLevel3();
    }
    else if (pointInButton(gameCompleteButtons[1], mx, my))
    {
        currentLevel = 1;
        gameState = STATE_MENU;
    }
}

#endif
