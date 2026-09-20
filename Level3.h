
#pragma once
#ifndef LEVEL3_H
#define LEVEL3_H

#include "GameUtility.h"
#include "Player.h"
#include "Enemy.h"
#include "Level1.h"
#include "Level2.h"

// ---------------------------------------------------------------------------
//  LAYER 3 -- THE ROT AND THE HOUND
//
//  Flow:
//    1. TWO WAVES OF WARSPAN walk in  (LEVEL3_WAVES in LevelSettings.h - change
//       the third number on a line for more or fewer, add a line for another
//       wave, and level3EnemyWarspan.png is the picture)
//    2. once they are down the SWAMP THING rises  (level3enemy.png, 150 HP)
//    3. put it down and the hero is healed back to FULL health
//    4. the GATE HOUND comes through  (level3enemyBoss.png, 260 HP)
//    5. put him down and the closing screen reads
//         "THE BOSS IS BEHIND THE DOOR"
//
//  The player enters this layer on full health as well, because startLevel3
//  calls resetPlayer().
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  WHAT KARL SAYS ON THE OTHER SIDE OF THE GATE
//
//  Played once, before the first wave of warspan walks in.
//  Plain keyboard characters only - write " ' and ... , never the curly kind.
// ---------------------------------------------------------------------------
const DialogueLine LEVEL3_DIALOGUE[] =
{
    { SPK_KARL, "So this is what lies beyond the gate..." },
    { SPK_KARL, "The air is different." },
    { SPK_KARL, "Everything here feels... alive." },
    { SPK_KARL, "I can't see what's waiting in the darkness." },
    { SPK_KARL, "But turning back isn't an option." },
    { SPK_KARL, "Whatever is down here..." },
    { SPK_KARL, "I'll overcome it." },
};
const int LEVEL3_DIALOGUE_COUNT =
    (int)(sizeof(LEVEL3_DIALOGUE) / sizeof(LEVEL3_DIALOGUE[0]));


enum Level3Phase
{
    L3_PHASE_GRUNTS,      // the ordinary enemies from LEVEL3_WAVES
    L3_PHASE_SWAMP,       // fighting the swamp thing
    L3_PHASE_HEAL,        // it is down, health restored, short beat
    L3_PHASE_BOSS_INTRO,  // the hound is announced
    L3_PHASE_BOSS,        // fighting the gate hound
    L3_PHASE_DONE         // he is down, running the victory beat
};

Level3Phase level3Phase = L3_PHASE_GRUNTS;
bool        level3Opened = false;   // has the opening conversation finished?
int         level3Timer = 0;
int         level3Tick  = 0;    // free running, drives the banner pulse
int         level3Wave    = 0;  // which row of LEVEL3_WAVES we are on
int         level3WaveGap = 0;  // countdown between waves

unsigned int bgImageLv3 = 0;

// This screen is no longer the end of the run - it leads into layer 4.
Button level3CompleteButtons[2] = {
    { (SCREEN_WIDTH - 250) / 2.0, 158, 250, 54, "CONTINUE", false },
    { (SCREEN_WIDTH - 250) / 2.0,  92, 250, 54, "MAIN MENU", false }
};

void loadLevel3Images()
{
    // TO CHANGE THE LEVEL 3 BACKGROUND: put your PNG in the Images folder
    // under any of these names. If none exist the layer 2 backdrop is reused.
    bgImageLv3 = iLoadImageIfExists("Images/level3BG.png");
    if (bgImageLv3 == 0) bgImageLv3 = iLoadImageIfExists("Images/level3bg.png");
    if (bgImageLv3 == 0) bgImageLv3 = iLoadImageIfExists("Images/bglevel3.png");
    if (bgImageLv3 == 0) bgImageLv3 = iLoadImageIfExists("Images/gameplay3.png");
}


// ---------------------------------------------------------------------------
//  ENTRY POINT
// ---------------------------------------------------------------------------
void startLevel3()
{
    currentLevel = 3;

    resetPlayer();          // full health on entering layer 3
    enemies.clear();
    endDialogue();          // drop anything left over from the layer before

    level3Phase   = L3_PHASE_GRUNTS;
    level3Timer   = 0;
    level3Tick    = 0;
    level3Wave    = 0;
    level3WaveGap = 0;
    hitPauseTimer = 0;

    // Karl speaks first and the arena stays empty while he does. The first
    // wave of warspan is spawned by updateLevel3 as soon as he has finished.
    // (Change the waves themselves in LEVEL3_WAVES, LevelSettings.h.)
    level3Opened = false;
    clearWaveQueue();
    startDialogue(LEVEL3_DIALOGUE, LEVEL3_DIALOGUE_COUNT);

    gameState = STATE_LEVEL3;
}


// ---------------------------------------------------------------------------
//  UPDATE
// ---------------------------------------------------------------------------
void updateLevel3()
{
    level3Tick++;

    // a conversation freezes the layer: nothing moves, nothing can hit you
    if (dialogueActive()) { updateDialogue(); return; }

    if (!level3Opened)
    {
        level3Opened = true;
        queueWaveFrom(LEVEL3_WAVES, LEVEL3_WAVE_COUNT, 0);
        return;
    }

    updatePlayerInput();
    updateEnemies();

    if (gameState != STATE_LEVEL3)
        return;                       // the player died mid-frame

    switch (level3Phase)
    {
    case L3_PHASE_GRUNTS:
        if (level3WaveGap > 0)
        {
            // short breather, then the next row of LEVEL3_WAVES walks in
            level3WaveGap--;
            if (level3WaveGap == 0)
            {
                level3Wave++;
                queueWaveFrom(LEVEL3_WAVES, LEVEL3_WAVE_COUNT, level3Wave);
            }
        }
        else
        {
            // let the next warspan in when there is room for it
            updateWaveFeed();

            if (waveFinished())
            {
                if (level3Wave + 1 < LEVEL3_WAVE_COUNT)
                {
                    level3WaveGap = WAVE_CLEAR_DELAY_TICKS;
                }
                else
                {
                    // the normal enemies are down - the main enemy arrives
                    enemies.clear();
                    clearWaveQueue();
                    spawnSwamp();
                    level3Phase = L3_PHASE_SWAMP;
                }
            }
        }
        break;

    case L3_PHASE_SWAMP:
        if (!anyAliveOfType(ENEMY_SWAMP))
        {
            // beating the swamp thing puts the hero back to full health
            player.health = PLAYER_MAX_HEALTH;
            level3Phase   = L3_PHASE_HEAL;
            level3Timer   = L3_HEAL_BANNER_TICKS;
        }
        break;

    case L3_PHASE_HEAL:
        level3Timer--;
        if (level3Timer <= 0)
        {
            level3Phase = L3_PHASE_BOSS_INTRO;
            level3Timer = L3_BOSS_INTRO_TICKS;
        }
        break;

    case L3_PHASE_BOSS_INTRO:
        level3Timer--;
        if (level3Timer <= 0)
        {
            enemies.clear();          // clear the remains before the duel
            spawnHound();             // the level 3 boss, 260 HP
            level3Phase = L3_PHASE_BOSS;
        }
        break;

    case L3_PHASE_BOSS:
        if (!anyAliveOfType(ENEMY_HOUND))
        {
            level3Phase = L3_PHASE_DONE;
            level3Timer = L3_BOSS_DEATH_TICKS;
        }
        break;

    case L3_PHASE_DONE:
        level3Timer--;
        if (level3Timer <= 0)
            gameState = STATE_LEVEL3_COMPLETE;
        break;
    }
}


// ---------------------------------------------------------------------------
//  DRAW
// ---------------------------------------------------------------------------
void drawLevel3Backdrop()
{
    if (bgImageLv3 != 0)
    {
        iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImageLv3);
        return;
    }

    // no layer 3 art found: fall back to whatever we do have, darkened
    if (bgImageLv2 != 0) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImageLv2);
    else                 iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                                     (unsigned int)bgImageLv1);
    dimScreen(0.35);
}

void drawLevel3BossBar()
{
    const Enemy* boss = findBoss();
    if (boss == 0) return;

    // sizes come from ENEMY_BAR_SCALE in GameUtility.h
    double w = BOSS_BAR_W;
    double h = BOSS_BAR_H;
    double x = BOSS_BAR_X;
    double y = BOSS_BAR_Y;              // clear of the hero's bar above it
    double pct = clampd(boss->health / boss->maxHealth, 0.0, 1.0);

    // Each layer 3 enemy has its own bar: the swamp thing gets the green one
    // from level33BossHealth.png, the hound the red one from level3BossHealth.png.
    const BarArt& bar = (boss->type == ENEMY_SWAMP) ? swampBarArt : level3BarArt;
    if (!drawImageBar(bar, x, y, w, h, pct))
    {
        iSetColor(25, 25, 25);
        iFilledRectangle(x, y, w, 16);
        if (boss->enraged) iSetColor(240, 90, 40);
        else               iSetColor(170, 25, 25);
        iFilledRectangle(x, y, w * pct, 16);
        iSetColor(210, 210, 210);
        iRectangle(x, y, w, 16);
    }

    char label[72];
    sprintf(label, "%s     %d / %d",
            (boss->type == ENEMY_HOUND) ? "THE GATE HOUND" : "THE SWAMP THING",
            (int)(boss->health + 0.5), (int)boss->maxHealth);
    drawStatusText(SCREEN_WIDTH / 2.0, y - 26, label, HUD_BOSSLBL_SIZE, 225, 215, 200);
}

void drawLevel3HUD()
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

    drawStatusText(SCREEN_WIDTH / 2.0, HUD_TITLE_Y,
                    "LAYER 3 -- THE ROT AND THE HOUND", HUD_TITLE_SIZE, 226, 210, 184);

    char remainLabel[72];
    if (level3Phase == L3_PHASE_GRUNTS)
        sprintf(remainLabel, "warspan wave %d / %d   --   hostiles remaining: %d",
                level3Wave + 1, LEVEL3_WAVE_COUNT,
                aliveEnemyCount() + waveStillToCome());
    else if (level3Phase == L3_PHASE_SWAMP)
        sprintf(remainLabel, "the rot is moving -- cut it down");
    else if (level3Phase == L3_PHASE_HEAL)
        sprintf(remainLabel, "the rot is dead -- your strength returns");
    else if (level3Phase == L3_PHASE_BOSS_INTRO)
        sprintf(remainLabel, "something is scratching at the door...");
    else if (level3Phase == L3_PHASE_BOSS)
        sprintf(remainLabel, "he breathes fire -- jump the burst");
    else
        sprintf(remainLabel, "the door is unguarded");

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

    if (level3Phase == L3_PHASE_SWAMP || level3Phase == L3_PHASE_BOSS ||
        level3Phase == L3_PHASE_DONE)
        drawLevel3BossBar();   // not during L3_PHASE_GRUNTS - no boss yet
}

void drawLevel3Banners()
{
    // "STRENGTH RESTORED" right after the swamp thing goes down
    if (level3Phase == L3_PHASE_HEAL)
    {
        double bR, bG, bB;
        if ((level3Tick / 8) % 2 == 0) { bR = 130; bG = 225; bB = 120; }
        else                           { bR = 80;  bG = 160; bB = 75;  }

        drawStatusText(SCREEN_WIDTH / 2.0, 380, "STRENGTH RESTORED", HUD_BANNER_SIZE, bR, bG, bB);
        drawStatusText(SCREEN_WIDTH / 2.0, 344,
                        "the rot is dead -- you are back to full health",
                        32, 178, 190, 172);
        return;
    }

    if (level3Phase != L3_PHASE_BOSS_INTRO) return;

    double bR, bG, bB;
    if ((level3Tick / 8) % 2 == 0) { bR = 238; bG = 80;  bB = 55; }
    else                           { bR = 152; bG = 38;  bB = 28; }

    drawStatusText(SCREEN_WIDTH / 2.0, 380, "THE GATE HOUND IS LOOSE", HUD_BANNER_SIZE, bR, bG, bB);
    drawStatusText(SCREEN_WIDTH / 2.0, 344,
                    "three heads, one breath of fire -- jump it", HUD_BANNER_SUB, 186, 172, 168);
}

void drawLevel3()
{
    iClear();
    drawLevel3Backdrop();

    drawAllEnemies();
    drawPlayer();
    drawHitBurst();

    drawLevel3Banners();
    drawLevel3HUD();
    drawDamageVignette();
    drawDialogue();
}


// ---------------------------------------------------------------------------
//  CLOSING SCREEN  --  "THE BOSS IS BEHIND THE DOOR"
// ---------------------------------------------------------------------------
int level3CompleteHover = -1;

void drawLevel3CompleteScreen()
{
    iClear();

    // level4Unlocked.png: a finished screen with its own buttons
    if (artLayer4Unlocked.art.tex != 0)
    {
        if (bgImageLv3 != 0) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImageLv3);
        else                 iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (unsigned int)bgImageLv1);
        dimScreen(0.80);
        drawScreenArt(artLayer4Unlocked);
        drawScreenArtHover(artLayer4Unlocked, level3CompleteHover);
        drawLayer4UnlockedStats();
        return;
    }

    // ---- fallback: the old painted panel --------------------------------
    if (bgImageLv3 != 0) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImageLv3);
    else                 iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (unsigned int)bgImageLv1);
    dimScreen(0.70);

    double pw = 700, ph = 280;
    double px = (SCREEN_WIDTH - pw) / 2.0, py = 250;
    drawPanel(px, py, pw, ph, 235, 175, 60);
    double cx = SCREEN_WIDTH / 2.0;

    drawStatusText(cx, py + ph - 64, "THE BOSS IS BEHIND THE DOOR", 34, 245, 200, 80);
    drawStatusText(cx, py + ph - 116, "LAYER 4 UNLOCKED  --  MEAFESTO",
                    20, 225, 215, 200);

    for (int i = 0; i < 2; i++)
        drawButton(level3CompleteButtons[i]);
}

void updateLevel3CompleteHover(int mx, int my)
{
    if (artLayer4Unlocked.art.tex != 0)
    {
        level3CompleteHover = -1;
        for (int i = 0; i < artLayer4Unlocked.buttonCount; i++)
            if (screenArtHit(artLayer4Unlocked, i, mx, my)) level3CompleteHover = i;
        return;
    }
    for (int i = 0; i < 2; i++)
        level3CompleteButtons[i].hover = pointInButton(level3CompleteButtons[i], mx, my);
}

void handleLevel3CompleteClick(int mx, int my)
{
    // CONTINUE opens the door into layer 4
    if (artLayer4Unlocked.art.tex != 0)
    {
        if (screenArtHit(artLayer4Unlocked, 0, mx, my)) startLevel4();
        else if (screenArtHit(artLayer4Unlocked, 1, mx, my))
        { currentLevel = 1; gameState = STATE_MENU; }
        return;
    }
    if (pointInButton(level3CompleteButtons[0], mx, my))
    {
        startLevel4();
    }
    else if (pointInButton(level3CompleteButtons[1], mx, my))
    {
        currentLevel = 1;
        gameState = STATE_MENU;
    }
}

#endif
