
#pragma once
#ifndef LEVEL4_H
#define LEVEL4_H

#include "GameUtility.h"
#include "Player.h"
#include "Enemy.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"

// ---------------------------------------------------------------------------
//  LAYER 4 -- MEAFESTO
//
//  There are NO ordinary enemies in this layer. It is the boss, start to
//  finish, and he has two lives:
//
//    1. he walks in and you fight his first form   (level4Boss1stLife.png)
//    2. at HALF health he summons SELLOIDS         (selloid.png)
//    3. at ZERO health he does not die - he plays the transformation
//       (level4EnemyTransformation.png) and stands back up on a FULL bar
//    4. his second form is faster and hits harder  (level4Boss2ndLife.png)
//    5. empty that bar too and the run is over
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  THE TWO CONVERSATIONS IN LAYER 4
//
//  The first runs once, after MEAFESTO walks in and before a blow is struck.
//  The second runs after he is finally down, before the closing screen.
//  Plain keyboard characters only - write " ' and ... , never the curly kind.
// ---------------------------------------------------------------------------
const DialogueLine LEVEL4_DIALOGUE[] =
{
    { SPK_MEAFESTO, "So... you finally made it." },
    { SPK_KARL,     "You're Meafesto." },
    { SPK_MEAFESTO, "I see you've survived the Abyss." },
    { SPK_KARL,     "You killed my father." },
    { SPK_MEAFESTO, "Yes." },
    { SPK_KARL,     "Why?" },
    { SPK_MEAFESTO, "Your father destroyed everything I had." },
    { SPK_MEAFESTO, "I waited years for revenge." },
    { SPK_KARL,     "So you murdered him... and framed me." },
    { SPK_MEAFESTO, "I needed someone to take the blame." },
    { SPK_KARL,     "You destroyed my life." },
    { SPK_MEAFESTO, "Your father destroyed mine first." },
    { SPK_KARL,     "Then this ends here." },
    { SPK_MEAFESTO, "Come, Karl." },
    { SPK_MEAFESTO, "Show me what the Abyss has made you." },
    { SPK_KARL,     "Gladly." },
};
const int LEVEL4_DIALOGUE_COUNT =
    (int)(sizeof(LEVEL4_DIALOGUE) / sizeof(LEVEL4_DIALOGUE[0]));

const DialogueLine LEVEL4_ENDING_DIALOGUE[] =
{
    { SPK_KARL, "It's over..." },
    { SPK_KARL, "The truth is finally out." },
    { SPK_KARL, "I was never the murderer." },
    { SPK_KARL, "I survived the Abyss." },
    { SPK_KARL, "And I avenged my father." },
    { SPK_KARL, "Now... I'm going home." },
};
const int LEVEL4_ENDING_DIALOGUE_COUNT =
    (int)(sizeof(LEVEL4_ENDING_DIALOGUE) / sizeof(LEVEL4_ENDING_DIALOGUE[0]));


enum Level4Phase
{
    L4_PHASE_INTRO,        // "MEAFESTO" beat before he walks in
    L4_PHASE_TALK,         // he is on screen, the two of them speak
    L4_PHASE_LIFE1,        // fighting his first form
    L4_PHASE_TRANSFORM,    // the change, played as a centred cut-in
    L4_PHASE_LIFE2,        // fighting his second form
    L4_PHASE_DONE,         // he is down, running the victory beat
    L4_PHASE_LAST_WORDS    // Karl's closing lines, before the final screen
};

Level4Phase level4Phase   = L4_PHASE_INTRO;
// Where his first life ran out. The change happens on that spot and his second
// form stands back up on it, instead of the fight teleporting to the middle of
// the screen and back.
double      level4ChangeX = SCREEN_WIDTH / 2.0;
int         level4Timer   = 0;
int         level4Tick    = 0;   // free running, drives the banner pulse
int         level4Summons = 0;   // how many times he has called the wraiths in
int         level4SummonBanner = 0;

unsigned int bgImageLv4 = 0;

Button level4CompleteButtons[1] = {
    { (SCREEN_WIDTH - 250) / 2.0, 150, 250, 54, "MAIN MENU", false }
};

void loadLevel4Images()
{
    // TO CHANGE THE LAYER 4 BACKGROUND: replace level4BG.png, or add your own
    // file name to this list. If none exist the layer 3 backdrop is reused.
    bgImageLv4 = iLoadImageIfExists("Images/Enime/Level 4/level4BG.png");
    if (bgImageLv4 == 0) bgImageLv4 = iLoadImageIfExists("Images/level4BG.png");
    if (bgImageLv4 == 0) bgImageLv4 = iLoadImageIfExists("Images/level4bg.png");
    if (bgImageLv4 == 0) bgImageLv4 = iLoadImageIfExists("Images/bglevel4.png");
}


// ---------------------------------------------------------------------------
//  ENTRY POINT
// ---------------------------------------------------------------------------
void startLevel4()
{
    currentLevel = 4;

    resetPlayer();          // full health on entering layer 4
    enemies.clear();
    endDialogue();          // drop anything left over from the layer before

    level4Phase   = L4_PHASE_INTRO;
    level4ChangeX = SCREEN_WIDTH / 2.0;
    level4Timer   = L4_INTRO_TICKS;
    level4Tick    = 0;
    level4Summons = 0;
    level4SummonBanner = 0;
    hitPauseTimer = 0;

    gameState = STATE_LEVEL4;
}


// ---------------------------------------------------------------------------
//  UPDATE
// ---------------------------------------------------------------------------
void updateLevel4()
{
    level4Tick++;

    // a conversation freezes the layer: nothing moves, nothing can hit you
    if (dialogueActive()) { updateDialogue(); return; }

    // whichever conversation just closed, this is what happens next
    if (level4Phase == L4_PHASE_TALK)       level4Phase = L4_PHASE_LIFE1;
    else if (level4Phase == L4_PHASE_LAST_WORDS)
    {
        // The run is over and won. This is the only place a score is written,
        // so dying never puts a name on the board.
        int seconds = (int)(runTicks / STEPS_PER_REAL_SECOND);
        int score   = computeScore(killCount, 4, seconds);
        saveScore(playerName[0] ? playerName : "PLAYER",
                   score, killCount, 4, seconds);

        gameState = STATE_LEVEL4_COMPLETE;
        return;
    }

    if (level4SummonBanner > 0) level4SummonBanner--;

    // While he is changing, everything stands still. The hero stays exactly
    // where he was when the first bar emptied - he cannot walk away from it,
    // and nothing can hit him either.
    if (level4Phase == L4_PHASE_TRANSFORM)
    {
        level4Timer--;
        if (level4Timer <= 0)
        {
            enemies.clear();
            spawnLevel4Boss();
            Enemy* again = findLevel4Boss();
            if (again)
            {
                again->x = level4ChangeX;      // back up on the very same spot
                again->facingRight = (player.x > level4ChangeX);
                again->entered = true;
                beginSecondLife(*again);       // full bar, new form
            }
            level4Phase = L4_PHASE_LIFE2;
        }
        return;
    }

    updatePlayerInput();
    updateEnemies();

    if (gameState != STATE_LEVEL4)
        return;                       // the player died mid-frame

    Enemy* boss = findLevel4Boss();

    switch (level4Phase)
    {
    case L4_PHASE_INTRO:
        level4Timer--;
        if (level4Timer <= 0)
        {
            spawnLevel4Boss();

            // He normally walks in from off screen, which would put his half
            // of the conversation off the edge of the picture. For the talk he
            // is placed where he can be seen; he moves normally again the
            // moment the fight starts.
            Enemy* arriving = findLevel4Boss();
            if (arriving != 0)
            {
                arriving->x = ARENA_RIGHT - 190.0;
                arriving->facingRight = false;
            }

            level4Phase = L4_PHASE_TALK;
            startDialogue(LEVEL4_DIALOGUE, LEVEL4_DIALOGUE_COUNT);
        }
        break;

    case L4_PHASE_TALK:
        break;                     // handled above, before the layer updates

    case L4_PHASE_LIFE1:
        if (boss == 0) break;

        // ---- half health: he calls the wraiths in, once ------------------
        if (!boss->hasSummoned &&
            boss->health <= boss->maxHealth * L4_SUMMON_AT_FRACTION)
        {
            boss->hasSummoned = true;
            spawnSelloids(L4_SUMMON_COUNT);
            level4Summons++;
            level4SummonBanner = L4_SUMMON_BANNER_TICKS;
            boss = findLevel4Boss();   // the vector may have moved on push_back
        }

        // ---- empty bar: he does not die, he changes ---------------------
        if (boss != 0 && !boss->alive)
        {
            // Breaking his first life puts the hero back to FULL health, so
            // the second form is a fresh fight and not a formality.
            player.health = PLAYER_MAX_HEALTH;

            level4ChangeX = boss->x;   // the change happens right here
            level4Phase = L4_PHASE_TRANSFORM;
            level4Timer = L4_TRANSFORM_TICKS;
            enemies.clear();           // the wraiths burn up with him
        }
        break;

    case L4_PHASE_TRANSFORM:
        break;                     // handled above, before the layer updates

    case L4_PHASE_LIFE2:
        if (!anyAliveOfType(ENEMY_L4BOSS))
        {
            level4Phase = L4_PHASE_DONE;
            level4Timer = L4_BOSS_DEATH_TICKS;
        }
        break;

    case L4_PHASE_DONE:
        // the one second beat after the last blow, then Karl's closing lines
        level4Timer--;
        if (level4Timer <= 0)
        {
            level4Phase = L4_PHASE_LAST_WORDS;
            startDialogue(LEVEL4_ENDING_DIALOGUE, LEVEL4_ENDING_DIALOGUE_COUNT);
        }
        break;

    case L4_PHASE_LAST_WORDS:
        break;                     // handled above, before the layer updates
    }
}


// ---------------------------------------------------------------------------
//  DRAW
// ---------------------------------------------------------------------------
void drawLevel4Backdrop()
{
    if (bgImageLv4 != 0)
    {
        iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImageLv4);
        return;
    }
    if (bgImageLv3 != 0) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImageLv3);
    else                 iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                                     (unsigned int)bgImageLv1);
    dimScreen(0.35);
}

void drawLevel4BossBar()
{
    const Enemy* boss = findBoss();
    if (boss == 0) return;

    // sizes come from ENEMY_BAR_SCALE in GameUtility.h
    double w = BOSS_BAR_W;
    double h = BOSS_BAR_H_L4;
    double x = BOSS_BAR_X;
    double y = BOSS_BAR_Y;              // clear of the hero's bar above it
    double pct = clampd(boss->health / boss->maxHealth, 0.0, 1.0);

    // level4BossHealth.png. Only layer 4 uses this bar.
    if (!drawImageBar(l4BossBarArt, x, y, w, h, pct))
    {
        iSetColor(25, 25, 25);
        iFilledRectangle(x, y, w, 16);
        iSetColor(190, 30, 30);
        iFilledRectangle(x, y, w * pct, 16);
        iSetColor(210, 210, 210);
        iRectangle(x, y, w, 16);
    }

    // the picture already carries his name, so this is just the state
    char label[80];
    sprintf(label, "%s     %d / %d",
            boss->secondLife ? "SECOND LIFE" : "FIRST LIFE",
            (int)(boss->health + 0.5), (int)boss->maxHealth);
    drawStatusText(SCREEN_WIDTH / 2.0, y - 26, label, HUD_BOSSLBL_SIZE, 230, 205, 195);
}

void drawLevel4HUD()
{
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
                    "LAYER 4 -- MEAFESTO", HUD_TITLE_SIZE, 226, 210, 184);

    char note[72];
    if (level4Phase == L4_PHASE_INTRO)
        sprintf(note, "something very old is waking up...");
    else if (level4Phase == L4_PHASE_TALK)
        sprintf(note, "he has been waiting for you");
    else if (level4Phase == L4_PHASE_LIFE1)
        sprintf(note, "first life  --  he summons at half");
    else if (level4Phase == L4_PHASE_TRANSFORM)
        sprintf(note, "he is not finished  --  your strength returns");
    else if (level4Phase == L4_PHASE_LIFE2)
        sprintf(note, "second life  --  faster, and he hits harder");
    else
        sprintf(note, "it is over");

    drawStatusText(SCREEN_WIDTH / 2.0, HUD_SUB_Y, note, HUD_SUB_SIZE, 176, 164, 150);

    char killLabel[32];
    sprintf(killLabel, "KILLS  %d", killCount);
    if (gameFontTex != 0)
        drawGameTextRight(HUD_KILLS_RIGHT, HUD_KILLS_Y, killLabel,
                           HUD_KILLS_SIZE, 214, 198, 172);
    else
    { iSetColor(200, 200, 200);
      iText(SCREEN_WIDTH - 130, SCREEN_HEIGHT - 30, killLabel, GLUT_BITMAP_8_BY_13); }

    drawSpecialCue();   // "H  SPECIAL" / the cooldown, on its own line below

    if (level4Phase == L4_PHASE_TALK  || level4Phase == L4_PHASE_LIFE1 ||
        level4Phase == L4_PHASE_LIFE2 || level4Phase == L4_PHASE_DONE)
        drawLevel4BossBar();
}

// The change between his two lives, played big and centred over a dimmed
// arena so it reads as a cut-in rather than a picture pasted on the level.
void drawLevel4Transformation()
{
    if (level4Phase != L4_PHASE_TRANSFORM) return;

    // A light darkening, not the old full screen dim. The change now happens
    // in the arena on the spot where he fell rather than as a picture panel
    // pasted over the middle of the screen, so the scene has to stay visible.
    dimScreen(0.34);

    double done = 1.0 - (double)level4Timer / (double)L4_TRANSFORM_TICKS;
    if (done < 0.0) done = 0.0;
    if (done > 0.999) done = 0.999;

    if (l4TransformAnim.tex != 0 && l4TransformAnim.count > 0)
    {
        int frame = (int)(done * l4TransformAnim.count);
        if (frame >= l4TransformAnim.count) frame = l4TransformAnim.count - 1;

        // exactly as tall as the form he is about to stand up in, standing on
        // the floor at the x where his first life ran out
        double h = L4BOSS_DRAW_H_LIFE2 * L4_TRANSFORM_SCALE;
        double w = h * l4TransformAnim.aspect;
        drawSpriteUV(l4TransformAnim.tex,
                      level4ChangeX - w / 2.0, GROUND_Y, w, h, false,
                      (double)frame / l4TransformAnim.count, 0.0,
                      (double)(frame + 1) / l4TransformAnim.count, 1.0);
    }

    double bR, bG, bB;
    if ((level4Tick / 6) % 2 == 0) { bR = 255; bG = 110; bB = 55; }
    else                           { bR = 172; bG = 55;  bB = 28; }

    // L4_CHANGE_BANNER_Y keeps this clear of the layer title and its line of
    // small writing, which sit at the very top of the screen. At 520 it was
    // landing on top of them.
    drawStatusText(SCREEN_WIDTH / 2.0, L4_CHANGE_BANNER_Y,
                    "MEAFESTO IS CHANGING", HUD_BANNER_SIZE, bR, bG, bB);
    drawStatusText(SCREEN_WIDTH / 2.0, L4_CHANGE_BANNER_Y - 38.0,
                    "killing him once was not enough", HUD_BANNER_SUB, 196, 176, 166);
}

void drawLevel4Banners()
{
    if (level4Phase == L4_PHASE_INTRO)
    {
        double bR, bG, bB;
        if ((level4Tick / 8) % 2 == 0) { bR = 245; bG = 90;  bB = 55; }
        else                           { bR = 155; bG = 42;  bB = 28; }
        drawStatusText(SCREEN_WIDTH / 2.0, 380, "MEAFESTO", HUD_BANNER_SIZE * 1.15, bR, bG, bB);
        drawStatusText(SCREEN_WIDTH / 2.0, 344,
                        "no minions, no waves -- just him", HUD_BANNER_SUB, 186, 172, 168);
        return;
    }

    if (level4SummonBanner > 0)
    {
        double bR, bG, bB;
        if ((level4Tick / 6) % 2 == 0) { bR = 226; bG = 216; bB = 246; }
        else                           { bR = 150; bG = 140; bB = 180; }
        // the boss bar lives at 410..496, so this sits clear below it
        drawStatusText(SCREEN_WIDTH / 2.0, 330, "SELLOIDS SUMMONED", HUD_BANNER_SIZE, bR, bG, bB);
    }
}

void drawLevel4()
{
    iClear();
    drawLevel4Backdrop();

    drawAllEnemies();
    drawPlayer();
    drawHitBurst();

    drawLevel4Transformation();
    drawLevel4Banners();
    drawLevel4HUD();
    drawDamageVignette();
    drawDialogue();
}


// ---------------------------------------------------------------------------
//  RUN COMPLETE SCREEN
// ---------------------------------------------------------------------------
int level4CompleteHover = -1;

void drawLevel4CompleteScreen()
{
    iClear();

    // level4GameFinished.png: the closing screen, with its own MAIN MENU
    if (artGameFinished.art.tex != 0)
    {
        if (bgImageLv4 != 0) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImageLv4);
        else                 iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (unsigned int)bgImageLv1);
        dimScreen(0.80);
        drawScreenArt(artGameFinished);
        drawScreenArtHover(artGameFinished, level4CompleteHover);
        drawGameFinishedStats();
        return;
    }

    // ---- fallback: the old painted panel --------------------------------
    if (bgImageLv4 != 0) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImageLv4);
    else                 iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (unsigned int)bgImageLv1);
    dimScreen(0.70);

    double pw = 700, ph = 290;
    double px = (SCREEN_WIDTH - pw) / 2.0, py = 245;
    drawPanel(px, py, pw, ph, 235, 175, 60);
    double cx = SCREEN_WIDTH / 2.0;

    drawStatusText(cx, py + ph - 60, "MEAFESTO HAS FALLEN", 36, 245, 200, 80);
    drawStatusText(cx, py + ph - 112, "RUN COMPLETE  --  ALL FOUR LAYERS CLEARED",
                    20, 225, 215, 200);

    drawButton(level4CompleteButtons[0]);
}

void updateLevel4CompleteHover(int mx, int my)
{
    if (artGameFinished.art.tex != 0)
    {
        level4CompleteHover = screenArtHit(artGameFinished, 0, mx, my) ? 0 : -1;
        return;
    }
    level4CompleteButtons[0].hover = pointInButton(level4CompleteButtons[0], mx, my);
}

// MAIN MENU on the closing screen goes to the board first - that is where the
// name and the score from this run are shown.
void handleLevel4CompleteClick(int mx, int my)
{
    bool hit = (artGameFinished.art.tex != 0)
                ? screenArtHit(artGameFinished, 0, mx, my)
                : pointInButton(level4CompleteButtons[0], mx, my);
    if (hit) showScoreboard();
}

#endif
