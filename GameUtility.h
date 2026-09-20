
#pragma once
#ifndef GAME_UTILITY_H
#define GAME_UTILITY_H

// iGraphics.h is guarded, so including it here is free at build time. It means
// every game header now parses standalone, which is what stops Visual Studio's
// IntelliSense from red-underlining iSetColor / iFilledRectangle / GLUT_* in
// files it is looking at outside of iMain.cpp.
#include "iGraphics.h"
#include "LevelSettings.h"   // <-- the waves live here. Edit that file.

#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <vector>


const int   SCREEN_WIDTH   = 1000;
const int   SCREEN_HEIGHT  = 600;

const double ARENA_LEFT    = 60.0;
const double ARENA_RIGHT   = SCREEN_WIDTH - 60.0;
const double ARENA_BOTTOM  = 90.0;
const double ARENA_TOP     = 300.0;

// ---------------------------------------------------------------------------
//  WORLD / GROUND
// ---------------------------------------------------------------------------
const double GROUND_Y          = ARENA_BOTTOM;          // the floor line (feet rest here)
const double PLAYER_CEILING_Y  = SCREEN_HEIGHT - 190.0; // highest the feet may reach

// ---------------------------------------------------------------------------
//  ON-SCREEN CHARACTER SIZES
//
//  Only the HEIGHT is set here. The width is worked out automatically from
//  each image's own shape when it loads (see loadSprite below), so swapping in
//  a new PNG never needs a number changed here.
// ---------------------------------------------------------------------------
const double PLAYER_DRAW_H     = 130.0;
const double PLAYER_HALF_W     = 24.0;   // collision half-width, not the art width

const double SHADOW_DRAW_H     = 125.0;  // level 1 small enemy  (normalEnemeylevel1.png)
const double PRISONER_DRAW_H   = 168.0;  // level 1 big enemy    (enemyBoss.png)

// ---------------------------------------------------------------------------
//  PLAYER MOVEMENT (tuned for the ~62 Hz fixedUpdate tick)
// ---------------------------------------------------------------------------
const double PLAYER_MAX_SPEED       = 5.4;   // top horizontal speed
const double PLAYER_RUN_ACCEL       = 0.95;  // ground acceleration per tick
const double PLAYER_AIR_ACCEL       = 0.55;  // weaker steering while airborne
const double PLAYER_TURN_BOOST      = 1.9;   // extra accel when reversing direction
const double PLAYER_GROUND_FRICTION = 0.78;  // velocity kept per tick with no input
const double PLAYER_AIR_FRICTION    = 0.955;
const double PLAYER_VEL_EPSILON     = 0.06;  // below this we snap to a full stop

// ---------------------------------------------------------------------------
//  GRAVITY / JUMP
// ---------------------------------------------------------------------------
const double GRAVITY            = 0.90;  // base downward pull per tick
const double GRAVITY_FALL_MULT  = 1.45;  // heavier while falling -> snappy arc
const double FAST_FALL_MULT     = 2.10;  // holding S / Down slams you back down
const double MAX_FALL_SPEED     = 22.0;  // terminal velocity

const double JUMP_VELOCITY      = 14.2;  // ~112 px of hang for the first jump
const double AIR_JUMP_VELOCITY  = 12.2;  // the double jump is slightly weaker
const double JUMP_CUT_MULT      = 0.42;  // release the key early -> short hop
const int    MAX_AIR_JUMPS      = 1;     // 1 = double jump enabled
const int    COYOTE_TICKS       = 7;     // jump grace after walking off an edge
const int    JUMP_BUFFER_TICKS  = 8;     // jump pressed just before landing still fires
const int    LAND_SQUASH_TICKS  = 8;     // landing squash-and-stretch length
const double WALK_CYCLE_PX      = 17.0;  // pixels travelled per walk frame
const double WALK_BOB_PX        = 1.8;   // body rise at mid-stride
const double WALK_SQUASH        = 0.012; // weight shift as the foot lands
const int    GRUNT_ATTACK_ANIM  = 18;    // how long a grunt's swing is drawn

const double PLAYER_KNOCKBACK_X = 6.0;   // pushed back when an enemy connects
const double PLAYER_KNOCKBACK_Y = 5.0;
const int    PLAYER_HITSTUN     = 10;    // ticks of reduced control after a hit

const double PLAYER_MAX_HEALTH      = 230.0;
const double PLAYER_ATTACK_RANGE    = 80.0;  // the kick out-reaches the enemies
const double PLAYER_ATTACK_VSPAN    = 72.0;  // you can still connect from a low jump
const double PLAYER_ATTACK_DAMAGE   = 20.0;
const int    PLAYER_ATTACK_COOLDOWN = 26;
const int    PLAYER_ATTACK_ANIM_LEN = 20;  // long enough to read 4 frames
const int    HIT_PAUSE_TICKS        = 6;
const int    HIT_FLASH_TICKS        = 10;

// ---------------------------------------------------------------------------
//  THE SPECIAL MOVE  --  the H key
//
//  Unlike the kick, this one hits BOTH sides at once, reaches further, hits
//  much harder and costs a long cooldown. Which layers it is available in is
//  set by SPECIAL_MOVE_FROM_LEVEL in LevelSettings.h.
// ---------------------------------------------------------------------------
const double SPECIAL_RANGE          = 170.0; // more than twice the kick's reach
const double SPECIAL_VSPAN          = 130.0; // it catches airborne enemies too
const double SPECIAL_DAMAGE         = 45.0;  // nearly three kicks in one blow
const double SPECIAL_KNOCKBACK      = 22.0;  // and it throws them
const int    SPECIAL_COOLDOWN       = 200;   // about 3 seconds before the next one
const int    SPECIAL_ANIM_LEN       = 34;    // how long the move is drawn
const int    SPECIAL_HIT_AT         = 14;    // ticks in, the blow actually lands
const double SPECIAL_DRAW_SCALE     = 1.25;  // the hero swells while it fires

// kept for backwards compatibility with older code that referenced it
const double PLAYER_SPEED           = PLAYER_MAX_SPEED;

// ---------------------------------------------------------------------------
//  ENEMIES
// ---------------------------------------------------------------------------
const double SHADOW_SPEED           = 2.3;
const double SHADOW_MAX_HEALTH      = 40.0;  // small enemy - dies quicker
const double PRISONER_SPEED         = 1.5;
const double PRISONER_MAX_HEALTH    = 80.0;  // big enemy - soaks more
const double ENEMY_TOUCH_RANGE      = 62.0;   // horizontal reach (sprites are wide)
const double ENEMY_REACH_HEIGHT     = 62.0;   // jump higher than this and you are safe
const double ENEMY_TOUCH_DAMAGE     = 6.0;
const int    ENEMY_ATTACK_COOLDOWN  = 50;
const double ENEMY_SEPARATION       = 62.0;   // stops the wide sprites overlapping
const double ENEMY_KNOCKBACK        = 9.0;

// ---------------------------------------------------------------------------
//  LEVEL 2 BOSS  --  "THE WARDEN"   (the new enemy character, 200 HP)
// ---------------------------------------------------------------------------
const double BRUTE_MAX_HEALTH        = 250.0;   // <-- his life
const double BRUTE_SPEED             = 1.15;
const double BRUTE_RAGE_SPEED        = 2.05;    // he speeds up when badly hurt
const double BRUTE_RAGE_THRESHOLD    = 0.40;    // ...below 40% health
const double BRUTE_DRAW_H            = PLAYER_DRAW_H * 1.5;  // exactly 1.5x the hero
const double BRUTE_TOUCH_RANGE       = 74.0;  // must stay under PLAYER_ATTACK_RANGE
const double BRUTE_REACH_HEIGHT      = 88.0;    // you can still jump over him
const double BRUTE_DAMAGE            = 15.0;
const int    BRUTE_ATTACK_COOLDOWN   = 62;
const int    BRUTE_WINDUP_TICKS      = 26;      // telegraph before the swing
const int    BRUTE_SWING_TICKS       = 14;      // follow-through after the hit lands
const double BRUTE_KNOCKBACK_RESIST  = 0.30;    // barely flinches

// ---------------------------------------------------------------------------
//  LEVEL 3  --  "THE ROT AND THE HOUND"
//
//  Two fights back to back:
//    1. the SWAMP THING   (level3enemy.png)
//    2. the GATE HOUND    (level3enemyBoss.png)  <- the level 3 boss
//  Beating the swamp thing restores the hero to full health before the boss.
// ---------------------------------------------------------------------------
const double SWAMP_MAX_HEALTH        = 200.0;
const double SWAMP_SPEED             = 1.35;
const double SWAMP_RAGE_SPEED        = 2.10;
const double SWAMP_DRAW_H            = 165.0;
const double SWAMP_TOUCH_RANGE       = 78.0;   // must stay under PLAYER_ATTACK_RANGE
const double SWAMP_REACH_HEIGHT      = 86.0;   // vines reach high - jump early
const double SWAMP_DAMAGE            = 12.0;
const int    SWAMP_ATTACK_COOLDOWN   = 58;
const int    SWAMP_WINDUP_TICKS      = 24;     // the vines gather before they lash
const double SWAMP_KNOCKBACK_RESIST  = 0.55;

const double HOUND_MAX_HEALTH        = 290.0;  // the biggest health pool in the game
const double HOUND_SPEED             = 1.55;   // faster than the Warden, he is a beast
const double HOUND_RAGE_SPEED        = 2.60;
const double HOUND_DRAW_H            = 240.0;  // 1.5x - short but very wide
const double HOUND_TOUCH_RANGE       = 86.0;   // still under PLAYER_ATTACK_RANGE
const double HOUND_REACH_HEIGHT      = 70.0;   // he is low - easy to jump over
const double HOUND_DAMAGE            = 18.0;
const int    HOUND_ATTACK_COOLDOWN   = 66;
const int    HOUND_WINDUP_TICKS      = 28;     // the fire builds before it hits
const double HOUND_KNOCKBACK_RESIST  = 0.22;

// WARSPAN, the layer 3 crawler that comes in waves before the swamp thing.
// He is quicker and tougher than the layer 1 grunt but is not a boss, so he
// gets the small bar over his head, not the wide one at the top.
const double WARSPAN_MAX_HEALTH         = 65.0;
const double WARSPAN_SPEED              = 2.05;
const double WARSPAN_DRAW_H             = 122.0;  // wide and low - aspect is ~1.17
const double WARSPAN_TOUCH_RANGE        = 72.0;   // must stay under PLAYER_ATTACK_RANGE
const double WARSPAN_REACH_HEIGHT       = 56.0;   // low reach - easy to jump over
const double WARSPAN_DAMAGE             = 9.0;
const int    WARSPAN_ATTACK_COOLDOWN    = 46;

const double BOSS_RAGE_THRESHOLD     = 0.40;   // any boss speeds up below 40% health

// ---------------------------------------------------------------------------
//  LEVEL 4  --  "MEAFESTO"   (a boss-only layer, and he has TWO lives)
//
//  Life 1: level4Boss1stLife.png. At half health he summons SELLOIDS.
//  Life 0: he plays the transformation and comes back on a FULL bar as
//          level4Boss2ndLife.png, faster and harder.
// ---------------------------------------------------------------------------
const double L4BOSS_LIFE1_HEALTH     = 350.0;  // first life
const double L4BOSS_LIFE2_HEALTH     = 410.0;  // second life, after the change
const double L4BOSS_SPEED            = 1.30;
const double L4BOSS_SPEED_LIFE2      = 1.95;   // the new form is quicker
const double L4BOSS_RAGE_SPEED       = 2.40;
const double L4BOSS_DRAW_H           = 210.0;  // life 1 is tall and thin
// He has been through three sizes: 185, then 1.8x that at 333, and now 0.6x
// of the 333 he was. At 200 px he is a head taller than his first form and
// still the biggest thing in the game, but he no longer crowds the arena.
const double L4BOSS_DRAW_H_LIFE2     = 185.0 * 1.8 * 0.6;   // = 199.8
const double L4BOSS_TOUCH_RANGE      = 84.0;   // must stay under PLAYER_ATTACK_RANGE
const double L4BOSS_REACH_HEIGHT     = 96.0;
const double L4BOSS_DAMAGE           = 16.0;
const double L4BOSS_DAMAGE_LIFE2     = 22.0;
const int    L4BOSS_ATTACK_COOLDOWN  = 60;
const int    L4BOSS_WINDUP_TICKS     = 26;
const double L4BOSS_KNOCKBACK_RESIST = 0.18;

// the minions he calls in at half health
const double SELLOID_MAX_HEALTH      = 55.0;
const double SELLOID_SPEED           = 2.45;   // fast but flimsy
const double SELLOID_DRAW_H          = 96.0 * 2.0;    // = 192, twice what they were
const double SELLOID_TOUCH_RANGE      = 70.0;
const double SELLOID_REACH_HEIGHT     = 58.0;  // low - easy to jump
const double SELLOID_DAMAGE           = 8.0;
const int    SELLOID_ATTACK_COOLDOWN  = 48;

// ---------------------------------------------------------------------------
//  HOW LONG AFTER THE LAST ENEMY DIES THE RESULT SCREEN APPEARS
//
//  One second, in every layer. Before this, layer 1 cut to its result screen
//  with no pause at all while layers 2, 3 and 4 held for one and a half to
//  nearly two seconds. All four now use the same beat, so the kill lands and
//  then the screen comes up.
// ---------------------------------------------------------------------------
const int    RESULT_SCREEN_DELAY_TICKS = 62;   // 62 ticks = 1 second

// ---- LEVEL 4 PACING ----
const double L4_SUMMON_AT_FRACTION   = 0.50;  // he summons at half his first life
const int    L4_SUMMON_COUNT         = 2;     // selloids per summon
const int    L4_SUMMON_BANNER_TICKS  = 100;
const int    L4_INTRO_TICKS          = 120;   // "MEAFESTO" beat before the fight
const int    L4_TRANSFORM_TICKS      = 220;   // the whole change, ~3.5 seconds
// The change is drawn at the spot where his first life ended, at the height of
// the form he stands up in. 1.0 means exactly that height; raise it if the
// flames should reach further over his head.
const double L4_TRANSFORM_SCALE      = 1.00;
// where "MEAFESTO IS CHANGING" sits. The layer title is at 564 and its line of
// small writing at 540, so this has to stay well clear of both.
const double L4_CHANGE_BANNER_Y      = 430.0;
const int    L4_BOSS_DEATH_TICKS     = RESULT_SCREEN_DELAY_TICKS;

// ---- LEVEL 3 PACING ----
const int    L3_BOSS_INTRO_TICKS     = 120;  // "THE HOUND IS LOOSE" beat
const int    L3_BOSS_DEATH_TICKS     = RESULT_SCREEN_DELAY_TICKS;
const int    L3_HEAL_BANNER_TICKS    = 120;  // "STRENGTH RESTORED" after the swamp thing

// The number of waves, and what is in each one, now come from the tables in
// LevelSettings.h  (LEVEL1_WAVES / LEVEL2_WAVES / LEVEL3_WAVES).
// WAVE_CLEAR_DELAY_TICKS lives there too.

// ---------------------------------------------------------------------------
//  LEVEL 2 PACING
// ---------------------------------------------------------------------------
const int    L2_BOSS_INTRO_TICKS    = 110;  // "THE WARDEN APPROACHES" beat
const int    L2_BOSS_DEATH_TICKS    = RESULT_SCREEN_DELAY_TICKS;

enum GameState
{
    STATE_MENU,
    STATE_INSTRUCTIONS,
    STATE_LEVEL1,
    STATE_LEVEL2,
    STATE_PAUSED,
    STATE_GAMEOVER,
    STATE_LEVEL_COMPLETE,   // layer 1 cleared -> CONTINUE leads into level 2
    STATE_LEVEL3,           // layer 3, the swamp thing then the hound
    STATE_GAME_COMPLETE,    // layer 2 cleared -> CONTINUE leads into level 3
    STATE_LEVEL3_COMPLETE,  // "THE BOSS IS BEHIND THE DOOR" -> leads into level 4
    STATE_LEVEL4,           // layer 4, MEAFESTO and his two lives
    STATE_LEVEL4_COMPLETE,  // the run is finished
    STATE_STORY,            // the comic pages shown when PLAY is pressed
    STATE_NAME_ENTRY,       // typing your name, before the story
    STATE_SCOREBOARD        // the saved scores, after the run is finished
};


GameState gameState        = STATE_MENU;
GameState stateBeforePause = STATE_LEVEL1;


void resetGame();
void startNewGameFromMenu();
void performPlayerAttack();
void performPlayerSpecial();

// defined in Menu.h - the story pages shown before layer 1
void startStory();
void drawStory();
void updateStory();
void storyAdvance();
void storySkip();

// defined in Menu.h - typing your name, and the board of saved scores
void startNameEntry();
void drawNameEntry();
void updateNameEntry();
void drawScoreboard();
void updateScoreboardHover(int mx, int my);
void handleScoreboardClick(int mx, int my);
void showScoreboard();

// defined in Level2.h
void startLevel2();
void retryCurrentLevel();
void updateLevel2();
void drawLevel2();

// defined in Level3.h
void startLevel3();
void updateLevel3();
void drawLevel3();

// defined in Level4.h
void startLevel4();
void updateLevel4();
void drawLevel4();


int    hitPauseTimer = 0;
double lastHitX = 0, lastHitY = 0;
int    killCount = 0;

// 1 to 4 - which layer of the Abyss we are in (used by pause / retry)
int    currentLevel = 1;

// free running, used by the menu and the end screens for their glow / pulse
int    uiTick = 0;

// ---------------------------------------------------------------------------
//  HOW LONG THIS RUN HAS TAKEN
//
//  fixedUpdate is driven by a 16 millisecond timer, so it ticks about 62.5
//  times a second. Counting ticks while a layer is being played gives the
//  time spent actually fighting, which is what the result screens show.
// ---------------------------------------------------------------------------
const double TICKS_PER_SECOND = 62.5;

// How many of those steps actually happen in one REAL second, once GAME_SPEED
// (LevelSettings.h) has been applied. At full speed the two are the same
// number. Anything turning a count of steps into a count of seconds - the run
// clock, the score - has to use this one, or the timer on screen ticks slower
// than a real second does.
const double STEPS_PER_REAL_SECOND = TICKS_PER_SECOND * GAME_SPEED;

// ENEMY_FEED_SECONDS (LevelSettings.h) in game steps. Worked out from the real
// rate, so the wait between one enemy dying and the next walking in stays the
// same number of seconds on a clock however GAME_SPEED is set.
const int ENEMY_FEED_TICKS =
    (int)(ENEMY_FEED_SECONDS * STEPS_PER_REAL_SECOND + 0.5);
int    runTicks = 0;

// ===========================================================================
//
//            * * *  T H E   N A M E   A N D   T H E   S C O R E S  * * *
//
//  The name typed at the start of a run, and the board of finished runs kept
//  in scores.txt next to the game. One line per run:
//
//      name|score|kills|layer|seconds
//
//  Pipes separate the fields, so a pipe typed into a name is thrown away. The
//  file is plain text: open it in Notepad and you can read it, edit it or
//  delete it, and the game will cope with whatever is left.
//
//  It is read once when the game starts and appended to when a run is
//  finished, so nothing is lost if the game is closed mid-fight.
// ===========================================================================
const int  PLAYER_NAME_MAX = 14;
const char SCORE_FILE[]    = "scores.txt";

char playerName[PLAYER_NAME_MAX + 1] = "";
int  playerNameLen = 0;

struct ScoreEntry
{
    char name[PLAYER_NAME_MAX + 1];
    int  score;
    int  kills;
    int  layer;     // how deep the run got
    int  seconds;
};

ScoreEntry scoreTable[SCORE_MAX_ENTRIES];
int scoreCount   = 0;
int scoreJustSet = -1;   // which row this run landed on, so it can be lit up

inline int computeScore(int kills, int layersCleared, int seconds)
{
    int bonus = SCORE_TIME_BONUS - seconds * SCORE_TIME_COST;
    if (bonus < 0) bonus = 0;

    int s = kills * SCORE_PER_KILL + layersCleared * SCORE_PER_LAYER + bonus;
    if (s < 0) s = 0;
    return s;
}

inline void formatClock(int seconds, char* out)
{
    if (seconds < 0) seconds = 0;
    sprintf(out, "%02d:%02d", seconds / 60, seconds % 60);
}

// highest score first
inline void sortScores()
{
    for (int i = 1; i < scoreCount; i++)
    {
        ScoreEntry key = scoreTable[i];
        int j = i - 1;
        while (j >= 0 && scoreTable[j].score < key.score)
        {
            scoreTable[j + 1] = scoreTable[j];
            j--;
        }
        scoreTable[j + 1] = key;
    }
}

inline void loadScores()
{
    scoreCount = 0;

    FILE* f = fopen(SCORE_FILE, "r");
    if (f == 0) return;            // no file yet: an empty board, not an error

    char line[256];
    while (scoreCount < SCORE_MAX_ENTRIES && fgets(line, sizeof(line), f) != 0)
    {
        // chop the newline
        int n = 0;
        while (line[n] && line[n] != '\n' && line[n] != '\r') n++;
        line[n] = 0;
        if (n == 0) continue;

        // split on the pipes, in place
        char* field[5];
        int   fields = 0;
        field[fields++] = line;
        for (int i = 0; line[i] && fields < 5; i++)
            if (line[i] == '|') { line[i] = 0; field[fields++] = line + i + 1; }
        if (fields < 5) continue;   // a half written or hand mangled line

        ScoreEntry e;
        int k = 0;
        while (field[0][k] && k < PLAYER_NAME_MAX) { e.name[k] = field[0][k]; k++; }
        e.name[k]  = 0;
        e.score    = atoi(field[1]);
        e.kills    = atoi(field[2]);
        e.layer    = atoi(field[3]);
        e.seconds  = atoi(field[4]);
        // a finished run always scores at least the four layers, so anything
        // at or below zero is a hand mangled line rather than a real result
        if (e.name[0] == 0 || e.score <= 0) continue;

        scoreTable[scoreCount++] = e;
    }
    fclose(f);
    sortScores();
}

// Appends this run to the file and works out where it lands on the board.
inline void saveScore(const char* name, int score, int kills, int layer, int seconds)
{
    FILE* f = fopen(SCORE_FILE, "a");
    if (f != 0)
    {
        fprintf(f, "%s|%d|%d|%d|%d\n", name, score, kills, layer, seconds);
        fclose(f);
    }

    // keep the board in memory in step with the file
    if (scoreCount < SCORE_MAX_ENTRIES)
    {
        ScoreEntry e;
        int k = 0;
        while (name[k] && k < PLAYER_NAME_MAX) { e.name[k] = name[k]; k++; }
        e.name[k] = 0;
        e.score = score; e.kills = kills; e.layer = layer; e.seconds = seconds;
        scoreTable[scoreCount++] = e;
    }
    sortScores();

    scoreJustSet = -1;
    for (int i = 0; i < scoreCount; i++)
        if (scoreTable[i].score == score && scoreTable[i].seconds == seconds &&
            scoreTable[i].kills == kills)
        { scoreJustSet = i; break; }
}



inline void resetRunTimer() { runTicks = 0; }

// Fills out with MM:SS. Needs room for at least 8 characters.
inline void formatRunTime(char* out)
{
    int total = (int)(runTicks / STEPS_PER_REAL_SECOND);
    int mins  = total / 60;
    int secs  = total % 60;
    if (mins > 99) mins = 99;
    sprintf(out, "%02d:%02d", mins, secs);
}


inline double clampd(double v, double lo, double hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

inline double signd(double v)
{
    if (v > 0) return 1.0;
    if (v < 0) return -1.0;
    return 0.0;
}


inline int textWidthApprox(const char* s, int perCharPx)
{
    int len = 0;
    while (s[len] != '\0') len++;
    return len * perCharPx;
}

inline void drawCenteredText(double cx, double y, char* str, void* font, int perCharPx)
{
    double w = textWidthApprox(str, perCharPx);
    iText(cx - w / 2.0, y, str, font);
}


// ---------------------------------------------------------------------------
//  SAFE IMAGE LOADING
//  iLoadImage() still returns a texture id when the file is missing, which
//  then draws as a solid white box. Check the file exists first.
// ---------------------------------------------------------------------------
inline unsigned int iLoadImageIfExists(const char* path)
{
    // Decode first, THEN make the texture.
    //
    // iLoadImage() hands the raw pointer straight to glTexImage2D without
    // checking it, so if the file is missing OR is not a format stb_image can
    // read, you still get back a valid-looking texture id full of garbage -
    // which renders as a blank white/grey rectangle. Checking here means a bad
    // image returns 0 and the caller's fallback runs instead.
    //
    // stb_image reads PNG, JPG, BMP, TGA, GIF and PSD. It does NOT read WebP.
    // Files downloaded from the web are often WebP with a .png name on them -
    // that is exactly the case this guard catches.
    int w = 0, h = 0, bpp = 0;
    unsigned char* data = stbi_load(path, &w, &h, &bpp, 4);
    if (data == 0 || w <= 0 || h <= 0)
    {
        if (data) stbi_image_free(data);
        return 0;
    }

    unsigned int texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
                  GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    return texture;
}


// ---------------------------------------------------------------------------
//  SPRITE
//
//  Downloaded character art almost always has empty space around the figure,
//  and every image has a different amount of it. Rather than measuring each
//  file by hand, loadSprite() scans the alpha channel when the image loads and
//  records the tight box around the visible pixels, plus that box's width to
//  height ratio. drawSprite() then draws only that box at the right shape.
//
//  Practical effect: drop in any PNG and the character stands on the floor at
//  the right proportions with nothing to adjust.
// ---------------------------------------------------------------------------
struct Sprite
{
    unsigned int tex;
    int    w, h;              // full image size in pixels
    double u0, v0, u1, v1;    // tight box around the visible pixels
    double aspect;            // that box's width / height
    bool   facesLeft;         // true if the artwork is drawn facing LEFT
    double clearFrac;         // how much of the image is see-through, 0..1
};

inline Sprite emptySprite()
{
    Sprite s;
    s.tex = 0; s.w = 0; s.h = 0;
    s.u0 = 0.0; s.v0 = 0.0; s.u1 = 1.0; s.v1 = 1.0;
    s.aspect = 1.0;
    s.facesLeft = false;
    s.clearFrac = 0.0;
    return s;
}

inline Sprite loadSprite(const char* path)
{
    Sprite s = emptySprite();

    int w = 0, h = 0, bpp = 0;
    unsigned char* data = stbi_load(path, &w, &h, &bpp, 4);
    if (data == 0 || w <= 0 || h <= 0)
    {
        if (data) stbi_image_free(data);
        return s;                       // missing file, or a format stb cannot read
    }

    // tight box around everything that is not fully transparent, and at the
    // same time a count of how many pixels are see-through. That count is what
    // tells a properly cut-out sheet from one that still has its background
    // baked in - see loadAnimCutout below.
    int minX = w, minY = h, maxX = -1, maxY = -1;
    long clearPixels = 0;
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            if (data[(y * w + x) * 4 + 3] > 8)
            {
                if (x < minX) minX = x;
                if (x > maxX) maxX = x;
                if (y < minY) minY = y;
                if (y > maxY) maxY = y;
            }
            else clearPixels++;
        }
    }
    s.clearFrac = (double)clearPixels / (double)((long)w * (long)h);
    if (maxX < minX || maxY < minY)     // no alpha at all (a JPG, say): use it whole
    {
        minX = 0; minY = 0; maxX = w - 1; maxY = h - 1;
    }

    glGenTextures(1, &s.tex);
    glBindTexture(GL_TEXTURE_2D, s.tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    s.w = w; s.h = h;
    s.u0 = (double)minX / (double)w;
    s.u1 = (double)(maxX + 1) / (double)w;
    s.v0 = (double)minY / (double)h;
    s.v1 = (double)(maxY + 1) / (double)h;
    s.aspect = (double)(maxX - minX + 1) / (double)(maxY - minY + 1);
    return s;
}

// first of the given paths that loads; lets a file be named several ways
inline Sprite loadSpriteAny(const char* a, const char* b = 0,
                            const char* c = 0, const char* d = 0)
{
    Sprite s;
    if (a) { s = loadSprite(a); if (s.tex) return s; }
    if (b) { s = loadSprite(b); if (s.tex) return s; }
    if (c) { s = loadSprite(c); if (s.tex) return s; }
    if (d) { s = loadSprite(d); if (s.tex) return s; }
    return emptySprite();
}


// ---------------------------------------------------------------------------
//  SPRITE DRAWING
//  iShowImage() ignores the alpha channel and cannot mirror an image, so
//  characters need their own draw call: alpha blended, nearest-neighbour
//  filtered (crisp pixel art) and horizontally flippable for facing.
// ---------------------------------------------------------------------------
inline void drawSpriteUV(unsigned int texture,
                         double x, double y, double w, double h,
                         bool flipX,
                         double u0, double v0, double u1, double v1,
                         double tintR = 255, double tintG = 255, double tintB = 255,
                         double alpha = 255)
{
    if (texture == 0) return;

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glColor4d(tintR / 255.0, tintG / 255.0, tintB / 255.0, alpha / 255.0);

    double a = flipX ? u1 : u0;
    double b = flipX ? u0 : u1;

    glBegin(GL_QUADS);
        glTexCoord2d(a, v1); glVertex2d(x,     y);
        glTexCoord2d(b, v1); glVertex2d(x + w, y);
        glTexCoord2d(b, v0); glVertex2d(x + w, y + h);
        glTexCoord2d(a, v0); glVertex2d(x,     y + h);
    glEnd();

    glColor4d(1.0, 1.0, 1.0, 1.0);
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

// whole-image version
inline void drawSprite(unsigned int texture,
                       double x, double y, double w, double h,
                       bool flipX,
                       double tintR = 255, double tintG = 255, double tintB = 255,
                       double alpha = 255)
{
    drawSpriteUV(texture, x, y, w, h, flipX, 0.0, 0.0, 1.0, 1.0,
                 tintR, tintG, tintB, alpha);
}


// ---------------------------------------------------------------------------
//  UI PAINTING
//  iGraphics only has flat fills, so the panels and buttons below drop to raw
//  OpenGL for gradients and translucency.
// ---------------------------------------------------------------------------
inline void fillRectAlpha(double x, double y, double w, double h,
                          double r, double g, double b, double a)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4d(r/255.0, g/255.0, b/255.0, a);
    glBegin(GL_QUADS);
        glVertex2d(x, y); glVertex2d(x+w, y);
        glVertex2d(x+w, y+h); glVertex2d(x, y+h);
    glEnd();
    glColor4d(1,1,1,1);
    glDisable(GL_BLEND);
}

// vertical gradient: colour 1 at the bottom, colour 2 at the top
inline void fillRectGradient(double x, double y, double w, double h,
                             double r1, double g1, double b1, double a1,
                             double r2, double g2, double b2, double a2)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
        glColor4d(r1/255.0, g1/255.0, b1/255.0, a1); glVertex2d(x, y);
        glColor4d(r1/255.0, g1/255.0, b1/255.0, a1); glVertex2d(x+w, y);
        glColor4d(r2/255.0, g2/255.0, b2/255.0, a2); glVertex2d(x+w, y+h);
        glColor4d(r2/255.0, g2/255.0, b2/255.0, a2); glVertex2d(x, y+h);
    glEnd();
    glColor4d(1,1,1,1);
    glDisable(GL_BLEND);
}

// darkens whatever has already been drawn - used over the end-screen backdrops
inline void dimScreen(double amount)
{
    fillRectAlpha(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 4, 3, 8, amount);
}

// a soft rectangular halo, drawn as a few fading frames around the rect
inline void glowRect(double x, double y, double w, double h,
                     double r, double g, double b, double strength, int rings = 10)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int i = rings; i >= 1; i--)
    {
        double t = (double)i / (double)rings;
        double o = i * 1.6;
        glColor4d(r/255.0, g/255.0, b/255.0, strength * (1.0 - t) * 0.20);
        glBegin(GL_LINE_LOOP);
            glVertex2d(x-o, y-o); glVertex2d(x+w+o, y-o);
            glVertex2d(x+w+o, y+h+o); glVertex2d(x-o, y+h+o);
        glEnd();
    }
    glColor4d(1,1,1,1);
    glDisable(GL_BLEND);
}

// the dark slab the end screens put their text on
inline void drawPanel(double x, double y, double w, double h,
                      double edgeR, double edgeG, double edgeB)
{
    glowRect(x, y, w, h, edgeR, edgeG, edgeB, 1.0, 12);
    fillRectGradient(x, y, w, h, 10, 8, 14, 0.88, 26, 20, 30, 0.88);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4d(edgeR/255.0, edgeG/255.0, edgeB/255.0, 0.85);
    glBegin(GL_LINE_LOOP);
        glVertex2d(x, y); glVertex2d(x+w, y); glVertex2d(x+w, y+h); glVertex2d(x, y+h);
    glEnd();
    // a brighter hairline just inside the top and bottom edges
    glColor4d(edgeR/255.0, edgeG/255.0, edgeB/255.0, 0.35);
    glBegin(GL_LINES);
        glVertex2d(x+10, y+h-6); glVertex2d(x+w-10, y+h-6);
        glVertex2d(x+10, y+6);   glVertex2d(x+w-10, y+6);
    glEnd();
    glColor4d(1,1,1,1);
    glDisable(GL_BLEND);
}


// ---------------------------------------------------------------------------
//  SPRITE ANIMATION
//
//  An animation is one PNG holding its frames side by side in a single row,
//  every frame the same size. Because a swinging figure is not centred in its
//  own frame, each sheet also records where the character's body sits inside a
//  cell (anchorX across, anchorY up from the bottom) so the character stays put
//  while the sword and the slash swing around it.
//
//  The four numbers per sheet come from how the sheet was cut and are listed
//  next to each loadAnim() call.
// ---------------------------------------------------------------------------
struct SpriteAnim
{
    unsigned int tex;
    int    count;          // frames, left to right
    double aspect;         // one cell's width / height
    double heightScale;    // cell height as a multiple of the character height
    double anchorX;        // 0..1 across the cell: where the body centre sits
    double anchorY;        // 0..1 up from the cell bottom: where the feet sit
    bool   facesLeft;      // true if the artwork is drawn facing LEFT
};

inline SpriteAnim emptyAnim()
{
    SpriteAnim a;
    a.tex = 0; a.count = 0;
    a.aspect = 1.0; a.heightScale = 1.0; a.anchorX = 0.5; a.anchorY = 0.0;
    a.facesLeft = false;
    return a;
}

inline SpriteAnim loadAnim(const char* path, int frames, double aspect,
                           double heightScale, double anchorX, double anchorY,
                           bool facesLeft = false)
{
    SpriteAnim a = emptyAnim();
    Sprite s = loadSprite(path);            // reuses the safe decode-then-upload
    if (s.tex == 0 || frames <= 0) return a;

    a.tex = s.tex;
    a.count = frames;
    a.aspect = aspect;
    a.heightScale = heightScale;
    a.anchorX = anchorX;
    a.anchorY = anchorY;
    a.facesLeft = facesLeft;
    return a;
}

// ---------------------------------------------------------------------------
//  loadAnimCutout - a loader that refuses a sheet that was never cut out
//
//  Every animation sheet in this game has to be a PNG with a see-through
//  background. A sheet that still has its black studio background baked in
//  loads perfectly well and then draws as a solid black rectangle sitting on
//  top of the character, which looks far worse than no animation at all.
//
//  So: if a sheet has almost no transparent pixels, this throws it away and
//  hands back an empty animation. The character then falls back to its
//  standing picture, which is the right thing to show until the sheet has had
//  its background removed. Cut the background out and it starts working with
//  no code change.
// ---------------------------------------------------------------------------
inline SpriteAnim loadAnimCutout(const char* path, int frames, double aspect,
                                 double heightScale, double anchorX, double anchorY,
                                 bool facesLeft = false, double minClear = 0.08)
{
    SpriteAnim a = emptyAnim();
    if (path == 0 || frames <= 0) return a;

    Sprite s = loadSprite(path);
    if (s.tex == 0) return a;                 // missing, or a format stb cannot read

    if (s.clearFrac < minClear)
    {
        // it loaded, but it is a solid picture - drop it and the texture with it
        glDeleteTextures(1, &s.tex);
        return a;
    }

    a.tex = s.tex;
    a.count = frames;
    a.aspect = aspect;
    a.heightScale = heightScale;
    a.anchorX = anchorX;
    a.anchorY = anchorY;
    a.facesLeft = facesLeft;
    return a;
}

// Some sheets are drawn facing right, some facing left. This turns "which way
// is the character facing" into the flip flag the draw call wants, so a sheet
// can be swapped for one drawn the other way round by changing one flag.
inline bool animFlipFor(const SpriteAnim& a, bool facingRight)
{
    return a.facesLeft ? facingRight : !facingRight;
}

// Same idea for a still image. Without this every idle pose was drawn with a
// hard-coded !facingRight, which silently mirrors any art that faces left.
inline bool spriteFlipFor(const Sprite& s, bool facingRight)
{
    return s.facesLeft ? facingRight : !facingRight;
}

// frame: 0 .. count-1.  cx / feetY are where the CHARACTER should stand.
inline void drawAnimFrame(const SpriteAnim& a, int frame,
                          double cx, double feetY, double charDrawH, bool flipX,
                          double tintR = 255, double tintG = 255, double tintB = 255,
                          double alpha = 255)
{
    if (a.tex == 0 || a.count <= 0) return;
    if (frame < 0) frame = 0;
    if (frame >= a.count) frame = a.count - 1;

    double h = charDrawH * a.heightScale;
    double w = h * a.aspect;

    double u0 = (double)frame / (double)a.count;
    double u1 = (double)(frame + 1) / (double)a.count;

    double ax = flipX ? (1.0 - a.anchorX) : a.anchorX;
    double x0 = cx - ax * w;
    double y0 = feetY - a.anchorY * h;

    drawSpriteUV(a.tex, x0, y0, w, h, flipX, u0, 0.0, u1, 1.0,
                 tintR, tintG, tintB, alpha);
}


// A character that has its own artwork for each direction. Fill in whichever
// sheets exist: if only one is present it is used for both and mirrored.
struct DirAnim
{
    SpriteAnim right;
    SpriteAnim left;
};

inline DirAnim emptyDirAnim()
{
    DirAnim d;
    d.right = emptyAnim();
    d.left  = emptyAnim();
    return d;
}

inline const SpriteAnim& dirAnimFor(const DirAnim& d, bool facingRight)
{
    if (!facingRight && d.left.tex  != 0) return d.left;
    if ( facingRight && d.right.tex != 0) return d.right;
    return (d.right.tex != 0) ? d.right : d.left;   // only one sheet: mirror it
}


// Draws a character standing on feetY, centred on cx, drawH pixels tall.
// widthScale/heightScale are for squash-and-stretch (1.0 = normal).
inline void drawSpriteArt(const Sprite& s, double cx, double feetY, double drawH,
                          bool flipX,
                          double tintR = 255, double tintG = 255, double tintB = 255,
                          double alpha = 255)
{
    if (s.tex == 0) return;
    double w = drawH * s.aspect;
    drawSpriteUV(s.tex, cx - w / 2.0, feetY, w, drawH, flipX,
                 s.u0, s.v0, s.u1, s.v1, tintR, tintG, tintB, alpha);
}


// ---------------------------------------------------------------------------
//  ART-BASED HEALTH BARS
//
//  Each bar is TWO pictures of the same ornate frame: one with the track empty
//  and one with it full. The empty one is drawn whole, then the full one is
//  drawn on top clipped to the health fraction, so the red you see is the real
//  artwork being revealed rather than a flat rectangle painted over it.
//
//  trackU0 / trackU1 say where the fill track starts and ends across the
//  picture, measured 0..1 from its left edge. Everything left of trackU0 (the
//  portrait and the name) is identical in both pictures, so it never changes.
// ---------------------------------------------------------------------------
struct BarArt
{
    Sprite empty;
    Sprite full;
    double trackU0;
    double trackU1;
};

inline BarArt emptyBarArt()
{
    BarArt b;
    b.empty = emptySprite();
    b.full  = emptySprite();
    b.trackU0 = 0.0;
    b.trackU1 = 1.0;
    return b;
}

// x, y is the bottom-left corner. Returns false if the art is missing, so the
// caller can fall back to the old plain rectangles.
inline bool drawImageBar(const BarArt& bar, double x, double y,
                         double w, double h, double pct)
{
    if (bar.empty.tex == 0) return false;
    if (pct < 0.0) pct = 0.0;
    if (pct > 1.0) pct = 1.0;

    const Sprite& e = bar.empty;
    drawSpriteUV(e.tex, x, y, w, h, false, e.u0, e.v0, e.u1, e.v1);

    const Sprite& f = bar.full;
    if (f.tex == 0 || pct <= 0.0) return true;

    // how far across the whole picture the fill reaches
    double uEnd = bar.trackU0 + pct * (bar.trackU1 - bar.trackU0);
    double uu   = f.u0 + (f.u1 - f.u0) * uEnd;
    drawSpriteUV(f.tex, x, y, w * uEnd, h, false, f.u0, f.v0, uu, f.v1);
    return true;
}

// The hero's bar, used in EVERY layer. Each boss has his own.
BarArt heroBarArt   = emptyBarArt();
BarArt wardenBarArt = emptyBarArt();   // layer 2, leve2BossHealth.png
BarArt level3BarArt = emptyBarArt();   // layer 3 hound, level3BossHealth.png
BarArt swampBarArt  = emptyBarArt();   // layer 3 swamp thing, level33BossHealth.png
BarArt l4BossBarArt = emptyBarArt();   // layer 4, level4BossHealth.png

// The hero's SPECIAL MOVE charge, top right. Same two-picture trick as the
// health bars: hSpecialBarEmpty.png is the frame with the track dark and
// hSpecialBarFull.png is the same frame with it lit, so what fills up is the
// real artwork. Both were cut out of hSpecialMoveHealthBar.png.
BarArt specialBarArt = emptyBarArt();

inline void loadHudImages()
{
    heroBarArt.empty = loadSpriteAny("Images/Enime/Level 4/hpBarEmpty.png",
                                      "Images/Enime/hpBarEmpty.png", 0);
    heroBarArt.full  = loadSpriteAny("Images/Enime/Level 4/hpBarFull.png",
                                      "Images/Enime/hpBarFull.png", 0);
    heroBarArt.trackU0 = 0.2431;   // measured from level4HPBar.png
    heroBarArt.trackU1 = 0.8968;

    l4BossBarArt.empty = loadSpriteAny("Images/Enime/Level 4/bossBarEmpty.png",
                                        "Images/Enime/bossBarEmpty.png", 0);
    l4BossBarArt.full  = loadSpriteAny("Images/Enime/Level 4/bossBarFull.png",
                                        "Images/Enime/bossBarFull.png", 0);
    l4BossBarArt.trackU0 = 0.2467;  // measured from level4BossHealth.png
    l4BossBarArt.trackU1 = 0.8776;

    // THE WARDEN, layer 2
    wardenBarArt.empty = loadSpriteAny("Images/Enime/wardenBarEmpty.png", 0, 0);
    wardenBarArt.full  = loadSpriteAny("Images/Enime/wardenBarFull.png", 0, 0);
    wardenBarArt.trackU0 = 0.2231;  // measured from leve2BossHealth.png
    wardenBarArt.trackU1 = 0.8701;

    // the layer 3 pair - the swamp thing and the gate hound share this bar
    level3BarArt.empty = loadSpriteAny("Images/Enime/Level 3/level3BarEmpty.png",
                                        "Images/Enime/level3BarEmpty.png", 0);
    level3BarArt.full  = loadSpriteAny("Images/Enime/Level 3/level3BarFull.png",
                                        "Images/Enime/level3BarFull.png", 0);
    level3BarArt.trackU0 = 0.2117;  // measured from level3BossHealth.png
    level3BarArt.trackU1 = 0.8704;

    // the hero's special move charge
    specialBarArt.empty = loadSpriteAny("Images/CharacterImages/hSpecialBarEmpty.png",
                                         "Images/hSpecialBarEmpty.png", 0);
    specialBarArt.full  = loadSpriteAny("Images/CharacterImages/hSpecialBarFull.png",
                                         "Images/hSpecialBarFull.png", 0);
    specialBarArt.trackU0 = 0.1965;  // measured from hSpecialMoveHealthBar.png
    specialBarArt.trackU1 = 0.8943;

    // the swamp thing gets his own green bar
    swampBarArt.empty = loadSpriteAny("Images/Enime/Level 3/swampBarEmpty.png",
                                       "Images/Enime/swampBarEmpty.png", 0);
    swampBarArt.full  = loadSpriteAny("Images/Enime/Level 3/swampBarFull.png",
                                       "Images/Enime/swampBarFull.png", 0);
    swampBarArt.trackU0 = 0.2829;   // measured from level33BossHealth.png
    swampBarArt.trackU1 = 0.8798;
}

// ---------------------------------------------------------------------------
//  THE GAME FONT
//
//  GLUT only offers a few fixed bitmap fonts. They cannot be styled, and
//  worse, they are drawn at a fixed PIXEL size, so in fullscreen everything
//  else grows and the writing stays tiny.
//
//  So the game carries its own alphabet instead: Images/gameFont.png holds
//  every printable character in a 16 x 6 grid of square cells, and each letter
//  is drawn as a small textured quad. That means the writing scales with the
//  rest of the picture and can be tinted any colour.
//
//  The picture stores each letter white with a soft BLACK halo around it, so
//  tinting gives coloured text with a dark edge that stays readable over a
//  bright background.
//
//  TO CHANGE THE FONT: rebuild Images/gameFont.png with the same grid and
//  replace the width table below.
// ---------------------------------------------------------------------------
const int GF_COLS  = 16;
const int GF_ROWS  = 6;
const int GF_FIRST = 32;    // space
const int GF_LAST  = 126;   // ~

unsigned int gameFontTex = 0;

// How far the pen moves after each letter, as a fraction of the size it is
// drawn at. Without this every letter would be the same width and the writing
// would look like a ransom note.
const double GF_ADVANCE[GF_LAST - GF_FIRST + 1] = {
    0.2218, 0.2196, 0.3557, 0.4644, 0.3978, 0.6669, 0.5892, 0.2161,
    0.2728, 0.2728, 0.3138, 0.3978, 0.2394, 0.3709, 0.2372, 0.2832,
    0.3978, 0.3978, 0.3978, 0.3978, 0.3978, 0.3978, 0.3978, 0.3978,
    0.3978, 0.3978, 0.2372, 0.2394, 0.3978, 0.3978, 0.3978, 0.3021,
    0.7504, 0.5712, 0.5503, 0.5881, 0.6463, 0.4894, 0.4553, 0.6152,
    0.6632, 0.3060, 0.3038, 0.6128, 0.4859, 0.7289, 0.6545, 0.6717,
    0.4865, 0.6704, 0.5599, 0.4110, 0.5575, 0.6174, 0.5903, 0.8572,
    0.6361, 0.6039, 0.5694, 0.3030, 0.2834, 0.3030, 0.4210, 0.3854,
    0.2888, 0.3865, 0.4421, 0.3763, 0.4390, 0.3854, 0.2747, 0.4594,
    0.4779, 0.2578, 0.2580, 0.4785, 0.2383, 0.6923, 0.4920, 0.4499,
    0.4390, 0.4412, 0.3179, 0.2951, 0.2828, 0.4510, 0.4262, 0.6354,
    0.4195, 0.4386, 0.3804, 0.4277, 0.2010, 0.4277, 0.4188
};

// A letter's cell is square and the capitals fill exactly half of it, so
// size = 2 x the capital height you want.
const double GF_CAP_RATIO  = 0.5000;
// Where the writing line sits inside a cell, measured up from the cell floor.
const double GF_BASELINE_UP = 0.2917;
// The widths the game actually uses. They start as the hand-measured table
// above, and are replaced by widths measured off the picture when a new
// alphabet picture is loaded.
double gfAdvance[GF_LAST - GF_FIRST + 1];
bool   gfMeasured = false;

// How much the writing tightens up after the last inked pixel of a letter.
// Measured off gameFont.png, where the hand table averages 0.0575 of a cell
// inside the letter's right edge. Raise it to spread the letters out.
const double GF_TRACK = -0.0575;

// ---------------------------------------------------------------------------
//  LOADING A NEW ALPHABET PICTURE
//
//  An alphabet picture is ONE image holding 16 columns by 6 rows of equal
//  cells: ASCII 32 (space) through 126, in order, reading left to right and
//  top to bottom. gameFont.png is 1152 x 432, so its cells are 72 x 72.
//
//  Every letter in a font is a different width, and the game has to know all
//  95 of them or the writing comes out with gaps and collisions. Rather than
//  measure a new picture by hand, this walks the cells and reads each letter's
//  width straight off the pixels. Drop a new picture in on the same grid and
//  it just works.
// ---------------------------------------------------------------------------
inline bool loadFontAtlas(const char* path)
{
    int w = 0, h = 0, bpp = 0;
    unsigned char* data = stbi_load(path, &w, &h, &bpp, 4);
    if (data == 0 || w <= 0 || h <= 0)
    {
        if (data) stbi_image_free(data);
        return false;
    }

    int cw = w / GF_COLS;
    int chh = h / GF_ROWS;
    if (cw <= 0 || chh <= 0) { stbi_image_free(data); return false; }

    for (int i = 0; i <= GF_LAST - GF_FIRST; i++)
    {
        int col = i % GF_COLS;
        int row = i / GF_COLS;

        int lastInk = -1;
        for (int y = 0; y < chh; y++)
        {
            const unsigned char* line = data + (((row * chh + y) * w) + col * cw) * 4;
            for (int x = 0; x < cw; x++)
                if (line[x * 4 + 3] > 40 && x > lastInk) lastInk = x;
        }

        if (lastInk < 0)
            gfAdvance[i] = GF_ADVANCE[i];              // a blank cell: keep the old width
        else
        {
            double adv = (double)(lastInk + 1) / (double)cw + GF_TRACK;
            if (adv < 0.08) adv = 0.08;
            if (adv > 1.00) adv = 1.00;
            gfAdvance[i] = adv;
        }
    }

    glGenTextures(1, &gameFontTex);
    glBindTexture(GL_TEXTURE_2D, gameFontTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    return true;
}

inline void loadGameFont()
{
    for (int i = 0; i <= GF_LAST - GF_FIRST; i++) gfAdvance[i] = GF_ADVANCE[i];
    gfMeasured  = false;
    gameFontTex = 0;

    // TO CHANGE THE WRITING: put your alphabet picture in Images/ as
    // gameFront.png, on the 16 x 6 grid described above. Everything the game
    // writes uses it - the layer titles, the health numbers, the kill count,
    // the buttons, the conversations and the instructions screen.
    if (loadFontAtlas("Images/gameFront.png")) { gfMeasured = true; return; }
    if (loadFontAtlas("Images/gamefront.png")) { gfMeasured = true; return; }

    gameFontTex = iLoadImageIfExists("Images/gameFont.png");
}

inline double gameTextWidth(const char* text, double size)
{
    if (text == 0) return 0.0;
    double w = 0.0;
    for (int i = 0; text[i]; i++)
    {
        int c = (unsigned char)text[i];
        if (c < GF_FIRST || c > GF_LAST) c = ' ';
        w += gfAdvance[c - GF_FIRST] * size;
    }
    return w;
}

// x, y is the bottom-left of the line of writing.
//
// Every letter is one quad off the alphabet picture, and a busy frame can be
// several hundred of them. Going through drawSpriteUV would re-set the texture
// settings, the blend mode and the colour for every single letter - the same
// six calls, hundreds of times, for a picture that never changes. So the whole
// line is drawn as one batch instead: the settings go on once, all the letters
// are sent, and the settings come off once.
inline void drawGameText(double x, double y, const char* text, double size,
                         double r = 230, double g = 214, double b = 186,
                         double a = 255)
{
    if (gameFontTex == 0 || text == 0 || text[0] == 0) return;

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, gameFontTex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glColor4d(r / 255.0, g / 255.0, b / 255.0, a / 255.0);

    glBegin(GL_QUADS);
    double pen = x;
    for (int i = 0; text[i]; i++)
    {
        int c = (unsigned char)text[i];
        if (c < GF_FIRST || c > GF_LAST) c = ' ';
        int idx = c - GF_FIRST;

        if (c != ' ')
        {
            int col = idx % GF_COLS;
            int row = idx / GF_COLS;
            double u0 = (double)col / GF_COLS;
            double u1 = (double)(col + 1) / GF_COLS;
            double v0 = (double)row / GF_ROWS;        // v grows downward
            double v1 = (double)(row + 1) / GF_ROWS;

            glTexCoord2d(u0, v1); glVertex2d(pen,        y);
            glTexCoord2d(u1, v1); glVertex2d(pen + size, y);
            glTexCoord2d(u1, v0); glVertex2d(pen + size, y + size);
            glTexCoord2d(u0, v0); glVertex2d(pen,        y + size);
        }
        pen += gfAdvance[idx] * size;
    }
    glEnd();

    glColor4d(1.0, 1.0, 1.0, 1.0);
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

// Same as drawGameText, but y is the WRITING LINE rather than the bottom of
// the letter cells, which is what you want when lining up with existing art.
inline void drawGameTextBaseline(double x, double baselineY, const char* text,
                                 double size,
                                 double r = 230, double g = 214, double b = 186,
                                 double a = 255)
{
    drawGameText(x, baselineY - GF_BASELINE_UP * size, text, size, r, g, b, a);
}

inline void drawGameTextCentered(double cx, double y, const char* text,
                                 double size,
                                 double r = 230, double g = 214, double b = 186,
                                 double a = 255)
{
    drawGameText(cx - gameTextWidth(text, size) / 2.0, y, text, size, r, g, b, a);
}

// Ends at rightX. Used for the kill count so it never drifts into the middle.
inline void drawGameTextRight(double rightX, double y, const char* text,
                              double size,
                              double r = 230, double g = 214, double b = 186,
                              double a = 255)
{
    drawGameText(rightX - gameTextWidth(text, size), y, text, size, r, g, b, a);
}

// Falls back to the old GLUT text if gameFont.png is missing, so the game
// never ends up with no writing at all.
inline void drawStatusText(double cx, double y, const char* text, double size,
                           double r = 230, double g = 214, double b = 186)
{
    if (gameFontTex != 0) { drawGameTextCentered(cx, y, text, size, r, g, b); return; }
    iSetColor((int)r, (int)g, (int)b);
    drawCenteredText(cx, y, (char*)text, GLUT_BITMAP_HELVETICA_18, 9);
}


// ---------------------------------------------------------------------------
//  FULLSCREEN, WITHOUT THE CURSOR DRIFTING OFF THE BUTTONS
//
//  The game is drawn in a fixed 1000 x 600 world no matter how big the real
//  window is. Going fullscreen just stretches that world onto the monitor,
//  keeping its shape, with black bars on whichever pair of sides is spare.
//
//  That is the easy half. The hard half is the mouse: iGraphics reports mouse
//  positions in REAL screen pixels, so at 1920x1080 a click near the middle of
//  the screen arrives as roughly (960, 540) while the button it is sitting on
//  lives at (500, 300) in our world. Left alone, every button would respond
//  somewhere other than where you can see it. mouseToWorld undoes the stretch
//  so the cursor and the buttons always agree.
// ---------------------------------------------------------------------------
int g_viewX = 0;              // where the 1000x600 picture starts, in real pixels
int g_viewY = 0;
int g_viewW = SCREEN_WIDTH;   // how big it is drawn, in real pixels
int g_viewH = SCREEN_HEIGHT;

// Call this once at the top of every frame, before anything is drawn.
inline void applyLetterbox()
{
    int winW = glutGet(GLUT_WINDOW_WIDTH);
    int winH = glutGet(GLUT_WINDOW_HEIGHT);
    if (winW <= 0 || winH <= 0) return;

    // biggest whole-shape fit: never stretch the picture out of proportion
    double scale = (double)winW / (double)SCREEN_WIDTH;
    double other = (double)winH / (double)SCREEN_HEIGHT;
    if (other < scale) scale = other;

    g_viewW = (int)(SCREEN_WIDTH  * scale + 0.5);
    g_viewH = (int)(SCREEN_HEIGHT * scale + 0.5);
    g_viewX = (winW - g_viewW) / 2;
    g_viewY = (winH - g_viewH) / 2;

    glViewport(g_viewX, g_viewY, g_viewW, g_viewH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, SCREEN_WIDTH, 0.0, SCREEN_HEIGHT, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Turns what iGraphics hands the mouse callbacks into our 1000x600 world.
// iGraphics gives us (realX, iScreenHeight - realY) where iScreenHeight is
// still the ORIGINAL 600, so the first thing to do is put realY back.
inline void mouseToWorld(int inX, int inY, int& outX, int& outY)
{
    int realX = inX;
    int realY = SCREEN_HEIGHT - inY;          // undo the flip iGraphics did

    double scale = (g_viewW > 0) ? (double)g_viewW / (double)SCREEN_WIDTH : 1.0;
    if (scale <= 0.0) scale = 1.0;

    double wx = (realX - g_viewX) / scale;
    double wyFromTop = (realY - g_viewY) / scale;

    outX = (int)(wx + 0.5);
    outY = (int)(SCREEN_HEIGHT - wyFromTop + 0.5);   // our world counts y upward
}


// ---------------------------------------------------------------------------
//  RESULT SCREENS THAT ARE ONE FINISHED PICTURE
//
//  The layer-cleared screens are single images with their buttons already
//  painted into the artwork, so there is nothing for drawButton to draw. What
//  the game needs instead is to know WHERE those painted buttons are. Each
//  rectangle below is stored as a fraction of the picture, so wherever the
//  picture lands on screen the clickable area lands on top of it.
// ---------------------------------------------------------------------------
const int SCREEN_ART_MAX_BUTTONS = 2;

struct ScreenArt
{
    Sprite art;
    int    buttonCount;
    double bx0[SCREEN_ART_MAX_BUTTONS], bx1[SCREEN_ART_MAX_BUTTONS];
    double by0[SCREEN_ART_MAX_BUTTONS], by1[SCREEN_ART_MAX_BUTTONS];  // from the TOP
};

inline ScreenArt emptyScreenArt()
{
    ScreenArt s;
    s.art = emptySprite();
    s.buttonCount = 0;
    for (int i = 0; i < SCREEN_ART_MAX_BUTTONS; i++)
    { s.bx0[i]=s.bx1[i]=s.by0[i]=s.by1[i]=0.0; }
    return s;
}

// Where the picture sits in the 1000x600 world: as big as it can be without
// changing shape, centred.
inline void screenArtRect(const ScreenArt& s, double& x, double& y,
                          double& w, double& h)
{
    double aspect = (s.art.aspect > 0.01) ? s.art.aspect : 1.0;
    w = SCREEN_WIDTH;
    h = w / aspect;
    if (h > SCREEN_HEIGHT) { h = SCREEN_HEIGHT; w = h * aspect; }
    x = (SCREEN_WIDTH  - w) / 2.0;
    y = (SCREEN_HEIGHT - h) / 2.0;
}

// One painted button, converted into world coordinates.
inline void screenArtButton(const ScreenArt& s, int i, double& bx, double& by,
                            double& bw, double& bh)
{
    double x, y, w, h;
    screenArtRect(s, x, y, w, h);
    bx = x + w * s.bx0[i];
    bw = w * (s.bx1[i] - s.bx0[i]);
    // the stored fractions run down from the top, the world counts up
    by = y + h * (1.0 - s.by1[i]);
    bh = h * (s.by1[i] - s.by0[i]);
}

inline bool screenArtHit(const ScreenArt& s, int i, int mx, int my)
{
    if (i < 0 || i >= s.buttonCount) return false;
    double bx, by, bw, bh;
    screenArtButton(s, i, bx, by, bw, bh);
    return (mx >= bx && mx <= bx + bw && my >= by && my <= by + bh);
}

// Writes onto a result picture at a spot given as a fraction of the picture,
// so the live numbers land exactly where the baked-in ones used to sit and
// keep their place however the picture is scaled.
//   u         : across the picture, 0..1
//   vBaseline : down the picture to the writing line, 0..1
//   sizeFrac  : letter size as a fraction of the picture's height
inline void drawScreenArtText(const ScreenArt& s, double u, double vBaseline,
                              const char* text, double sizeFrac, bool centered,
                              double r = 232, double g = 220, double b = 196)
{
    double x, y, w, h;
    screenArtRect(s, x, y, w, h);
    double size = h * sizeFrac;
    double tx = x + w * u;
    double ty = y + h * (1.0 - vBaseline);      // the writing line, in world y

    if (centered)
        drawGameTextBaseline(tx - gameTextWidth(text, size) / 2.0, ty, text, size, r, g, b);
    else
        drawGameTextBaseline(tx, ty, text, size, r, g, b);
}

inline void drawScreenArt(const ScreenArt& s)
{
    if (s.art.tex == 0) return;
    double x, y, w, h;
    screenArtRect(s, x, y, w, h);
    drawSpriteUV(s.art.tex, x, y, w, h, false,
                  s.art.u0, s.art.v0, s.art.u1, s.art.v1);
}

// The buttons are part of the picture and cannot light up on their own, so
// hovering draws a soft glow over the one under the cursor.
inline void drawScreenArtHover(const ScreenArt& s, int hovered)
{
    if (hovered < 0 || hovered >= s.buttonCount) return;
    double bx, by, bw, bh;
    screenArtButton(s, hovered, bx, by, bw, bh);
    fillRectAlpha(bx, by, bw, bh, 255, 210, 120, 0.16);
}


// ---- the four result screens -------------------------------------------
ScreenArt artLayer1Cleared  = emptyScreenArt();
ScreenArt artLayer3Unlocked = emptyScreenArt();
ScreenArt artLayer4Unlocked = emptyScreenArt();
ScreenArt artGameFinished   = emptyScreenArt();

inline void setScreenButton(ScreenArt& s, int i,
                            double x0, double x1, double y0, double y1)
{
    s.bx0[i]=x0; s.bx1[i]=x1; s.by0[i]=y0; s.by1[i]=y1;
}

inline void loadScreenArt()
{
    // The button rectangles were measured off each picture. If you swap a
    // picture for one with the buttons somewhere else, update the numbers
    // here or the cursor will stop matching what you can see.
    artLayer1Cleared.art = loadSpriteAny("Images/Layer unlocked/leyarr1Cleared.png",
                                          "Images/leyarr1Cleared.png", 0);
    artLayer1Cleared.buttonCount = 2;
    setScreenButton(artLayer1Cleared, 0, 0.305, 0.695, 0.692, 0.795);  // CONTINUE
    setScreenButton(artLayer1Cleared, 1, 0.305, 0.695, 0.815, 0.920);  // MAIN MENU

    artLayer3Unlocked.art = loadSpriteAny("Images/Layer unlocked/layer3Unlcoked.png",
                                           "Images/layer3Unlcoked.png", 0);
    artLayer3Unlocked.buttonCount = 2;
    setScreenButton(artLayer3Unlocked, 0, 0.378, 0.628, 0.645, 0.728);
    setScreenButton(artLayer3Unlocked, 1, 0.378, 0.628, 0.758, 0.843);

    artLayer4Unlocked.art = loadSpriteAny("Images/Layer unlocked/level4Unlocked.png",
                                           "Images/level4Unlocked.png", 0);
    artLayer4Unlocked.buttonCount = 2;
    setScreenButton(artLayer4Unlocked, 0, 0.345, 0.655, 0.640, 0.732);
    setScreenButton(artLayer4Unlocked, 1, 0.345, 0.655, 0.760, 0.855);

    artGameFinished.art = loadSpriteAny("Images/Layer unlocked/level4GameFinished.png",
                                         "Images/level4GameFinished.png", 0);
    artGameFinished.buttonCount = 1;
    setScreenButton(artGameFinished, 0, 0.344, 0.656, 0.752, 0.897);   // MAIN MENU
}

// Where the hero's bar sits, and how big. Same in every layer.
// The hero's bar was wide enough to run under the layer title, so it is a
// little smaller now and every other line is placed around it.
const double HERO_BAR_X = 14.0;
const double HERO_BAR_W = 240.0;                  // ends at x 254
const double HERO_BAR_H = 56.0;
const double HERO_BAR_Y = SCREEN_HEIGHT - 68.0;   // bottom edge, y counts up
// where to print the numbers so they land on the bar's own fill track
const double HERO_BAR_TEXT_X = HERO_BAR_X + 82.0;
const double HERO_BAR_TEXT_Y = HERO_BAR_Y + 18.0;

// ---- ONE PLACE FOR EVERY IN-SCENE TEXT SIZE AND POSITION ----------------
// These are picked so nothing can overlap. The hero's bar ends at x 254, the
// longest layer title runs x 280 to 720, and the kill count sits past x 900.
const double HUD_TITLE_SIZE   = 28.0;
const double HUD_TITLE_Y      = SCREEN_HEIGHT - 36.0;
const double HUD_SUB_SIZE     = 21.0;
const double HUD_SUB_Y        = SCREEN_HEIGHT - 60.0;
const double HUD_KILLS_SIZE   = 23.0;
// The kill count used to be the top right corner. The special move bar has
// that corner now, so the count sits on the line below it. The right margin
// went from 18 to 30 as well: a glyph cell is wider than its advance, so the
// last letter of a right-aligned line overhangs, and 18 px left it touching
// the edge of the screen.
const double HUD_KILLS_Y      = SCREEN_HEIGHT - 80.0;
const double HUD_KILLS_RIGHT  = SCREEN_WIDTH - 30.0;

// ---- THE SPECIAL MOVE BAR, DIRECTLY UNDER THE HERO'S HEALTH BAR ---------
// hSpecialMoveHealthBar.png is 5.1572 times wider than it is tall, so the
// height follows from the width and the artwork is never squashed. It is left
// aligned with the health bar above it and a little narrower, which reads as
// one stack rather than two separate widgets.
const double SPECIAL_BAR_W      = 224.0;
const double SPECIAL_BAR_H      = SPECIAL_BAR_W / 5.1572;
const double SPECIAL_BAR_X      = HERO_BAR_X;
const double SPECIAL_BAR_Y      = HERO_BAR_Y - 6.0 - SPECIAL_BAR_H;

// The words under it. The picture has SPECIAL MOVE painted into a little
// cartouche, but that cartouche is a fifth of the bar wide, so at this size
// the lettering came out about four pixels tall and could not be read. It has
// been painted out of the picture and is written here instead, in the game's
// own alphabet, at a size that can be. The key that fires it leads the line.
const double SPECIAL_LABEL_SIZE = 16.0;
const double SPECIAL_LABEL_X    = SPECIAL_BAR_X + 2.0;
const double SPECIAL_LABEL_Y    = SPECIAL_BAR_Y - 4.0 - SPECIAL_LABEL_SIZE;
const double HUD_HP_SIZE      = 21.0;
const double HUD_BOSSLBL_SIZE = 21.0;
const double HUD_BANNER_SIZE  = 46.0;
const double HUD_BANNER_SUB   = 24.0;


// ---------------------------------------------------------------------------
//  ENEMY HEALTH BAR SIZES
//
//  ENEMY_BAR_SCALE shrinks every enemy bar at once - the wide boss bar at the
//  top of the screen in all four layers, and the little bar that appears over
//  an ordinary enemy's head. Change the one number to resize them all; the
//  wide bar stays centred and keeps the same top edge whatever the scale.
// ---------------------------------------------------------------------------
const double ENEMY_BAR_SCALE   = 0.8;

const double BOSS_BAR_W        = 560.0 * ENEMY_BAR_SCALE;   // = 448
const double BOSS_BAR_H        = 92.0  * ENEMY_BAR_SCALE;   // = 73.6
const double BOSS_BAR_H_L4     = 86.0  * ENEMY_BAR_SCALE;   // = 68.8
const double BOSS_BAR_X        = (SCREEN_WIDTH - BOSS_BAR_W) / 2.0;
const double BOSS_BAR_Y        = SCREEN_HEIGHT - 190.0;
const double BOSS_BAR_LABEL_Y  = BOSS_BAR_Y - 26.0;

const double GRUNT_BAR_W       = 40.0 * ENEMY_BAR_SCALE;    // = 32
const double GRUNT_BAR_H       = 5.0  * ENEMY_BAR_SCALE;    // = 4


// ===========================================================================
//
//              * * *  C O N V E R S A T I O N S  * * *
//
//  A line of speech in a panel above whoever is talking, one line at a time,
//  typed out letter by letter. While a conversation is running the layer stops
//  updating, so nothing moves and nothing can hit you; the pictures are still
//  drawn, so the fight is frozen on screen rather than hidden behind a menu.
//
//  CLICK or ENTER: finish typing the line, then move to the next one.
//  ESC:            skip the rest of the conversation.
//
//  The words themselves live at the top of the layer file they belong to
//  (Level1.h, Level2.h, Level3.h, Level4.h), so each layer's script is next to
//  the layer that plays it.
//
//  Only plain keyboard characters can be drawn - the alphabet picture holds
//  ASCII 32 to 126 and nothing else. Curly quotes and a real ellipsis come out
//  blank, so write " ' and ... instead.
// ===========================================================================
enum Speaker { SPK_KARL, SPK_WARDEN, SPK_MEAFESTO };

struct DialogueLine
{
    int         speaker;
    const char* text;
};

const DialogueLine* dlgScript = 0;
int  dlgCount = 0;
int  dlgIndex = 0;
int  dlgChars = 0;          // letters revealed so far in this line
int  dlgTick  = 0;
bool dlgKeyWasDown = true;  // starts true so a key still held cannot skip a line

const double DLG_TEXT_SIZE  = 20.0;
const double DLG_NAME_SIZE  = 16.0;
const double DLG_MAX_TEXT_W = 360.0;   // longer lines wrap onto the next row
const double DLG_PAD        = 14.0;
const double DLG_ROW        = 25.0;    // distance between two rows of speech
const double DLG_ABOVE_HEAD = 18.0;    // gap between the head and the panel
const int    DLG_TYPE_SPEED = 1;       // letters per tick (62 ticks a second)
const int    DLG_MAX_LINES  = 3;
const int    DLG_LINE_CHARS = 96;

inline bool dialogueActive()
{
    return dlgScript != 0 && dlgIndex >= 0 && dlgIndex < dlgCount;
}

inline void endDialogue()
{
    dlgScript = 0; dlgCount = 0; dlgIndex = 0; dlgChars = 0;
}

inline void startDialogue(const DialogueLine* script, int count)
{
    if (script == 0 || count <= 0) { endDialogue(); return; }
    dlgScript = script;
    dlgCount  = count;
    dlgIndex  = 0;
    dlgChars  = 0;
    dlgTick   = 0;
    dlgKeyWasDown = true;
}

// Greedy word wrap. Returns how many rows were used and fills `out`.
inline int dialogueWrap(const char* text, double size, double maxW,
                        char out[DLG_MAX_LINES][DLG_LINE_CHARS])
{
    for (int i = 0; i < DLG_MAX_LINES; i++) out[i][0] = 0;
    if (text == 0) return 0;

    int  row = 0;
    int  len = 0;
    char word[DLG_LINE_CHARS];
    int  wlen = 0;
    int  i = 0;

    for (;; i++)
    {
        char c = text[i];
        if (c != ' ' && c != 0)
        {
            if (wlen < DLG_LINE_CHARS - 1) word[wlen++] = c;
            continue;
        }
        word[wlen] = 0;

        if (wlen > 0)
        {
            // would this word still fit on the row we are on?
            char trial[DLG_LINE_CHARS * 2];
            int  t = 0;
            for (int k = 0; k < len && t < DLG_LINE_CHARS * 2 - 2; k++) trial[t++] = out[row][k];
            if (len > 0 && t < DLG_LINE_CHARS * 2 - 2) trial[t++] = ' ';
            for (int k = 0; k < wlen && t < DLG_LINE_CHARS * 2 - 2; k++) trial[t++] = word[k];
            trial[t] = 0;

            if (len > 0 && gameTextWidth(trial, size) > maxW && row + 1 < DLG_MAX_LINES)
            {
                row++;
                len = 0;
            }

            if (len > 0 && len < DLG_LINE_CHARS - 1) out[row][len++] = ' ';
            for (int k = 0; k < wlen && len < DLG_LINE_CHARS - 1; k++)
                out[row][len++] = word[k];
            out[row][len] = 0;
            wlen = 0;
        }

        if (c == 0) break;
    }
    return row + 1;
}

// How many letters the current line holds once it has been wrapped.
inline int dialogueLineLength()
{
    if (!dialogueActive()) return 0;
    char rows[DLG_MAX_LINES][DLG_LINE_CHARS];
    int n = dialogueWrap(dlgScript[dlgIndex].text, DLG_TEXT_SIZE, DLG_MAX_TEXT_W, rows);
    int total = 0;
    for (int i = 0; i < n; i++)
        for (int k = 0; rows[i][k]; k++) total++;
    return total;
}

// One press finishes the line; the next press moves on.
inline void dialogueAdvance()
{
    if (!dialogueActive()) return;

    int total = dialogueLineLength();
    if (dlgChars < total) { dlgChars = total; return; }

    dlgIndex++;
    dlgChars = 0;
    if (dlgIndex >= dlgCount) endDialogue();
}

inline void dialogueSkipAll()
{
    endDialogue();
}

inline void updateDialogue()
{
    if (!dialogueActive()) return;

    dlgTick++;
    uiTick++;                       // keeps the prompt breathing
    dlgChars += DLG_TYPE_SPEED;

    // ENTER or the right arrow. NOT the space bar: space is the jump button,
    // and a space still held as the last line closes would make the hero jump
    // the moment the layer starts moving again.
    bool held = (isKeyPressed(13) != 0) || (isSpecialKeyPressed(GLUT_KEY_RIGHT) != 0);
    if (held && !dlgKeyWasDown) dialogueAdvance();
    dlgKeyWasDown = held;
}

// ---------------------------------------------------------------------------
//  THE SPEECH BOX
//
//  Each speaker has his own picture: a portrait, his name already painted on,
//  a framed panel for the words and a little arrow in the corner. The game
//  writes nothing but the words themselves - the name comes from the artwork.
//
//  u0/u1 and v0/v1 mark the panel inside each picture that the words go in,
//  measured 0..1 across it and up from its bottom, so a picture can be swapped
//  for one laid out differently by changing four numbers.
//
//  If the pictures are missing the old panel-above-the-head is used instead,
//  so the conversations still run.
// ---------------------------------------------------------------------------
struct TalkBox
{
    Sprite art;
    double u0, u1;
    double v0, v1;
    double tr, tg, tb;      // the colour of the arrow that points at the head
};

TalkBox talkKarl, talkWarden, talkMeafesto;

// The box follows whoever is speaking instead of sitting at the bottom of the
// screen, so it reads as that character talking. It is smaller than it was for
// the same reason: a box wide enough to fill the bottom of the screen cannot
// sit over somebody's head without covering half the fight.
// The line that tells you how to move the conversation on. It is pinned to the
// floor strip the hero walks along, at the very bottom of the screen, rather
// than following the box around: under the box it cut a band straight across
// the middle of the scene. Down here the fade behind it runs off the bottom
// edge, so it reads as the floor darkening rather than a strip laid over the
// picture, and it never has to move whoever is talking.
const double DLG_PROMPT_Y      = 18.0;
const double DLG_PROMPT_SIZE   = 13.0;

// HALF the size it used to be. The box was 430 wide and took up most of the
// arena; at 215 it sits over one character's head without hiding the fight.
// Every one of the game's 42 spoken lines still fits inside it in two rows at
// this writing size - that was measured off the alphabet picture, not guessed.
const double DLG_BOX_W         = 215.0;
const double DLG_BOX_ABOVE     = 16.0;   // gap between the head and the box
const double DLG_BOX_TEXT_SIZE = 11.0;
const double DLG_BOX_ROW       = 12.0;

// The arrow under the box, pointing down at whoever is speaking.
const double DLG_TAIL_HALF     = 8.0;    // half the width of its base
const double DLG_TAIL_GAP      = 2.0;    // how far above the head the tip stops

inline void loadTalkBoxes()
{
    talkKarl.art = loadSpriteAny("Images/GameStory/talkKarl.png",
                                  "Images/GameStory/PlayerTalk.png", 0);
    talkKarl.u0 = 0.2581; talkKarl.u1 = 0.8785;
    talkKarl.v0 = 0.1950; talkKarl.v1 = 0.5750;
    talkKarl.tr = 196; talkKarl.tg = 44; talkKarl.tb = 40;

    talkWarden.art = loadSpriteAny("Images/GameStory/talkWarden.png",
                                    "Images/GameStory/Level2BossWardenTalks.png", 0);
    talkWarden.u0 = 0.2606; talkWarden.u1 = 0.8789;
    talkWarden.v0 = 0.1950; talkWarden.v1 = 0.5900;
    talkWarden.tr = 196; talkWarden.tg = 44; talkWarden.tb = 40;

    talkMeafesto.art = loadSpriteAny("Images/GameStory/talkMeafesto.png",
                                      "Images/GameStory/Level4BossTalks.png", 0);
    talkMeafesto.u0 = 0.2549; talkMeafesto.u1 = 0.8760;
    talkMeafesto.v0 = 0.2350; talkMeafesto.v1 = 0.5850;
    talkMeafesto.tr = 232; talkMeafesto.tg = 170; talkMeafesto.tb = 48;
}

// anchorX / anchorTopY are the middle of the speaker's head. The box sits
// above that and slides sideways and downwards if it would leave the screen,
// so it is always fully readable whoever is talking and wherever they stand.
// Returns false if this speaker has no picture, so the caller can fall back.
// bx / by come back as where it was actually drawn.
inline bool drawDialogueBox(const TalkBox& b, const char* text, int charsShown,
                            double anchorX, double anchorTopY,
                            double& bx, double& by)
{
    if (b.art.tex == 0) return false;

    double h = DLG_BOX_W / b.art.aspect;

    double y = anchorTopY + DLG_BOX_ABOVE;
    if (y + h > SCREEN_HEIGHT - 96.0) y = SCREEN_HEIGHT - 96.0 - h;   // under the HUD
    if (y < GROUND_Y + 16.0) y = GROUND_Y + 16.0;

    double x = anchorX - DLG_BOX_W / 2.0;
    if (x < 10.0) x = 10.0;
    if (x + DLG_BOX_W > SCREEN_WIDTH - 10.0) x = SCREEN_WIDTH - 10.0 - DLG_BOX_W;

    bx = x; by = y;

    // ---- the arrow, drawn FIRST so the box sits on top of its base ------
    // It runs from under the box down to just above the speaker's head, so
    // there is never any doubt which of the two characters is talking. If the
    // box had to slide sideways to stay on screen the arrow stays with the
    // head, only stepping in far enough to keep its base under the panel
    // rather than out over the portrait on the left.
    {
        double tipY = anchorTopY + DLG_TAIL_GAP;
        if (tipY > y - 4.0) tipY = y - 4.0;      // always below the box

        double tx = anchorX;
        double leftLimit  = x + DLG_BOX_W * 0.30;
        double rightLimit = x + DLG_BOX_W * 0.90;
        if (tx < leftLimit)  tx = leftLimit;
        if (tx > rightLimit) tx = rightLimit;

        if (tipY < y - 2.0)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4d(0.05, 0.04, 0.06, 0.92);
            glBegin(GL_TRIANGLES);
                glVertex2d(tx - DLG_TAIL_HALF, y + 2.0);
                glVertex2d(tx + DLG_TAIL_HALF, y + 2.0);
                glVertex2d(tx, tipY);
            glEnd();
            glColor4d(b.tr / 255.0, b.tg / 255.0, b.tb / 255.0, 0.95);
            glBegin(GL_LINE_STRIP);
                glVertex2d(tx - DLG_TAIL_HALF, y + 2.0);
                glVertex2d(tx, tipY);
                glVertex2d(tx + DLG_TAIL_HALF, y + 2.0);
            glEnd();
            glColor4d(1, 1, 1, 1);
            glDisable(GL_BLEND);
        }
    }

    drawSpriteUV(b.art.tex, x, y, DLG_BOX_W, h, false,
                  b.art.u0, b.art.v0, b.art.u1, b.art.v1);

    double tx0 = x + b.u0 * DLG_BOX_W;
    double tx1 = x + b.u1 * DLG_BOX_W;
    double ty0 = y + b.v0 * h;
    double ty1 = y + b.v1 * h;

    char rows[DLG_MAX_LINES][DLG_LINE_CHARS];
    int  n = dialogueWrap(text, DLG_BOX_TEXT_SIZE, tx1 - tx0, rows);
    if (n <= 0) return true;

    // the block of writing sits in the middle of the panel, however many rows
    double mid  = (ty0 + ty1) / 2.0;
    double rowY = mid + (n * DLG_BOX_ROW) / 2.0 - DLG_BOX_ROW;

    int left = charsShown;
    for (int i = 0; i < n; i++)
    {
        char shown[DLG_LINE_CHARS];
        int  len = 0;
        while (rows[i][len]) len++;

        int take = left; if (take > len) take = len; if (take < 0) take = 0;
        for (int k = 0; k < take; k++) shown[k] = rows[i][k];
        shown[take] = 0;
        left -= take;

        drawGameText(tx0, rowY, shown, DLG_BOX_TEXT_SIZE, 238, 228, 210);
        rowY -= DLG_BOX_ROW;
        if (left <= 0) break;
    }
    return true;
}


// ---------------------------------------------------------------------------
//  THE SPEECH PANEL
//
//  anchorX / anchorTopY are the middle of the speaker's head. The panel sits
//  above that with a small tail pointing back down at them, and slides sideways
//  if it would run off the screen so it is always fully readable.
// ---------------------------------------------------------------------------
inline void drawDialogueBubble(double anchorX, double anchorTopY,
                               const char* name, const char* text,
                               int charsShown,
                               double nr, double ng, double nb)
{
    char rows[DLG_MAX_LINES][DLG_LINE_CHARS];
    int  n = dialogueWrap(text, DLG_TEXT_SIZE, DLG_MAX_TEXT_W, rows);
    if (n <= 0) return;

    double textW = 0.0;
    for (int i = 0; i < n; i++)
    {
        double lw = gameTextWidth(rows[i], DLG_TEXT_SIZE);
        if (lw > textW) textW = lw;
    }
    double nameW = gameTextWidth(name, DLG_NAME_SIZE);
    if (nameW > textW) textW = nameW;

    double w = textW + DLG_PAD * 2.0;
    double h = DLG_PAD * 2.0 + DLG_NAME_SIZE + 6.0 + n * DLG_ROW;

    double y = anchorTopY + DLG_ABOVE_HEAD;
    if (y + h > SCREEN_HEIGHT - 96.0) y = SCREEN_HEIGHT - 96.0 - h;  // under the HUD
    if (y < GROUND_Y + 40.0) y = GROUND_Y + 40.0;

    double x = anchorX - w / 2.0;
    if (x < 12.0) x = 12.0;
    if (x + w > SCREEN_WIDTH - 12.0) x = SCREEN_WIDTH - 12.0 - w;

    drawPanel(x, y, w, h, nr, ng, nb);

    // the tail, pointing back down at whoever is speaking
    double tx = anchorX;
    if (tx < x + 18.0)     tx = x + 18.0;
    if (tx > x + w - 18.0) tx = x + w - 18.0;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4d(0.06, 0.05, 0.08, 0.88);
    glBegin(GL_TRIANGLES);
        glVertex2d(tx - 11, y + 1);
        glVertex2d(tx + 11, y + 1);
        glVertex2d(tx, y - 15);
    glEnd();
    glColor4d(nr / 255.0, ng / 255.0, nb / 255.0, 0.85);
    glBegin(GL_LINE_STRIP);
        glVertex2d(tx - 11, y + 1);
        glVertex2d(tx, y - 15);
        glVertex2d(tx + 11, y + 1);
    glEnd();
    glColor4d(1, 1, 1, 1);
    glDisable(GL_BLEND);

    // the speaker's name along the top of the panel
    double ny = y + h - DLG_PAD - DLG_NAME_SIZE;
    drawGameText(x + DLG_PAD, ny, name, DLG_NAME_SIZE, nr, ng, nb);

    // the words, revealed letter by letter across the rows in order
    int left = charsShown;
    double ty = ny - 8.0;
    for (int i = 0; i < n; i++)
    {
        ty -= DLG_ROW;

        char shown[DLG_LINE_CHARS];
        int  len = 0;
        while (rows[i][len]) len++;

        int take = left;
        if (take > len) take = len;
        if (take < 0)   take = 0;
        for (int k = 0; k < take; k++) shown[k] = rows[i][k];
        shown[take] = 0;
        left -= take;

        drawGameText(x + DLG_PAD, ty, shown, DLG_TEXT_SIZE, 226, 214, 196);
        if (left <= 0) break;
    }
}


struct Button
{
    double      x, y, w, h;
    char        label[32];
    bool        hover;
};

inline bool pointInButton(const Button& b, int mx, int my)
{
    return (mx >= b.x && mx <= b.x + b.w && my >= b.y && my <= b.y + b.h);
}

// ---------------------------------------------------------------------------
//  THE BUTTON PICTURE
//
//  Every button in the game - the menu, and the pause overlay - is drawn from
//  ONE picture: Images/stausBar.png. Drop that file in and all of them change
//  together, with no new buttons added and none taken away. The label is
//  written on top in the game's own alphabet, so it is the frame that comes
//  from the picture, not the words.
//
//  Until that file exists the old painted button is used instead, so the game
//  still works either way.
// ---------------------------------------------------------------------------
Sprite menuButtonArt = emptySprite();

// ---------------------------------------------------------------------------
//  THE RESUME BUTTON
//
//  Resume.png already has the word RESUME painted into the plate, so this one
//  button is drawn as the picture on its own with no lettering written over
//  the top. It keeps the exact rectangle the RESUME button already had, so the
//  pause menu does not move and the mouse still lines up with it.
// ---------------------------------------------------------------------------
Sprite resumeButtonArt = emptySprite();

// ---------------------------------------------------------------------------
//  THE DEATH SCREEN BANNER
//
//  TO ADD IT: drop your picture in Images/ as playerDeathBar.png. It is drawn
//  across the middle of the death screen with its proportions kept, whatever
//  shape it is, and the run's own numbers are written underneath it - the
//  layer you died on, how long you lasted, what you killed and how far through
//  the waves you got. Until the file exists the old painted panel is used, so
//  the death screen works either way.
// ---------------------------------------------------------------------------
Sprite deathBarArt = emptySprite();

// Where the run's own numbers go inside the death plate, as fractions of the
// picture. The first rect is the framed box in the middle of it; the second is
// the writing line under that box. Both were measured off the picture, so if
// you swap the picture for one laid out differently these four numbers are all
// you change.
const double DEATH_BOX_U0 = 0.1688, DEATH_BOX_U1 = 0.8313;
const double DEATH_BOX_V0 = 0.3471, DEATH_BOX_V1 = 0.5872;   // up from the bottom
const double DEATH_SUB_V  = 0.2308;                          // the lower writing line

// The same idea for the screen that asks your name. The rect is the framed
// field the letters you type appear in.
Sprite namePlateArt = emptySprite();
const double NAME_BOX_U0 = 0.1313, NAME_BOX_U1 = 0.8769;
const double NAME_BOX_V0 = 0.3564, NAME_BOX_V1 = 0.5714;

inline void loadButtonArt()
{
    menuButtonArt = loadSpriteAny("Images/stausBar.png",
                                   "Images/statusBar.png",
                                   "Images/Layer unlocked/stausBar.png");

    resumeButtonArt = loadSpriteAny("Images/Resume.png",
                                     "Images/resume.png",
                                     "Images/Layer unlocked/Resume.png");

    // deathPlate.png is failedMessageMessage.png with the two sample numbers
    // ("Hostiles purged before you fell: 12" and "You fell in Layer 3") taken
    // back out of it, so the game can write the real ones in their place. The
    // original is kept as the fallback.
    deathBarArt = loadSpriteAny("Images/deathPlate.png",
                                 "Images/failedMessageMessage.png",
                                 "Images/playerDeathBar.png",
                                 "Images/GameOver.png");

    // gamePlayerName.png with the sample name taken out of the field, same idea.
    namePlateArt = loadSpriteAny("Images/namePlate.png",
                                  "Images/gamePlayerName.png",
                                  "Images/playerNameBar.png");
}

// A button whose artwork already carries its own word. Same rectangle, same
// hover behaviour as drawButton, but nothing is written on top.
inline void drawButtonPicture(const Button& b, const Sprite& art)
{
    if (art.tex == 0) return;

    double x = b.x, y = b.y, w = b.w, h = b.h;
    double pulse = 0.5 + 0.5 * sin(uiTick * 0.09);

    fillRectAlpha(x + 3, y - 4, w, h, 0, 0, 0, 0.42);

    double grow = b.hover ? (3.0 + 2.0 * pulse) : 0.0;
    double tr = b.hover ? 255 : 208;
    double tg = b.hover ? 236 : 198;
    double tb = b.hover ? 216 : 196;

    if (b.hover)
        glowRect(x, y, w, h, 210, 60, 45, 0.30 + 0.30 * pulse, 10);

    drawSpriteUV(art.tex, x - grow, y - grow * 0.5,
                  w + grow * 2.0, h + grow, false,
                  art.u0, art.v0, art.u1, art.v1, tr, tg, tb, 255);
}

inline void drawButton(const Button& b)
{
    double x = b.x, y = b.y, w = b.w, h = b.h;

    // gentle breathing on the hovered button
    double pulse = 0.5 + 0.5 * sin(uiTick * 0.09);

    // ---- the picture version --------------------------------------------
    if (menuButtonArt.tex != 0)
    {
        fillRectAlpha(x + 3, y - 4, w, h, 0, 0, 0, 0.42);

        // hovered buttons warm up and lift very slightly
        double grow = b.hover ? (3.0 + 2.0 * pulse) : 0.0;
        double tr = b.hover ? 255 : 196;
        double tg = b.hover ? 226 : 190;
        double tb = b.hover ? 206 : 190;

        if (b.hover)
            glowRect(x, y, w, h, 210, 60, 45, 0.30 + 0.30 * pulse, 10);

        drawSpriteUV(menuButtonArt.tex, x - grow, y - grow * 0.5,
                      w + grow * 2.0, h + grow, false,
                      menuButtonArt.u0, menuButtonArt.v0,
                      menuButtonArt.u1, menuButtonArt.v1, tr, tg, tb, 255);

        double lsize = h * 0.46;
        double ly2   = y + h / 2.0 - lsize * 0.5 + lsize * GF_BASELINE_UP * 0.0;
        if (gameFontTex != 0)
        {
            drawGameTextCentered(x + w / 2.0, ly2 + 1, b.label, lsize, 0, 0, 0, 170);
            if (b.hover) drawGameTextCentered(x + w / 2.0, ly2 + 2, b.label, lsize, 255, 236, 214);
            else         drawGameTextCentered(x + w / 2.0, ly2 + 2, b.label, lsize, 214, 202, 186);
        }
        else
        {
            double cx2 = x + w / 2.0, ty2 = y + h / 2.0 - 6;
            iSetColor(0, 0, 0);
            drawCenteredText(cx2 + 1, ty2 - 1, const_cast<char*>(b.label),
                              GLUT_BITMAP_HELVETICA_18, 10);
            if (b.hover) iSetColor(255, 240, 225);
            else         iSetColor(206, 206, 214);
            drawCenteredText(cx2, ty2, const_cast<char*>(b.label),
                              GLUT_BITMAP_HELVETICA_18, 10);
        }
        return;
    }

    // ---- drop shadow ----------------------------------------------------
    fillRectAlpha(x + 3, y - 4, w, h, 0, 0, 0, 0.45);

    if (b.hover)
    {
        // ---- hovered: crimson, lit from below, with a halo ---------------
        glowRect(x, y, w, h, 210, 40, 40, 0.55 + 0.45 * pulse, 9);
        fillRectGradient(x, y, w, h, 150, 18, 18, 0.97, 74, 10, 14, 0.97);
        // sheen across the upper half
        fillRectGradient(x + 2, y + h * 0.52, w - 4, h * 0.46 - 2,
                          255, 255, 255, 0.00, 255, 220, 220, 0.13);
    }
    else
    {
        // ---- resting: cold graphite -------------------------------------
        fillRectGradient(x, y, w, h, 40, 38, 46, 0.94, 20, 19, 25, 0.94);
        fillRectGradient(x + 2, y + h * 0.55, w - 4, h * 0.43 - 2,
                          255, 255, 255, 0.00, 255, 255, 255, 0.07);
    }

    // ---- border + inner bevel -------------------------------------------
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (b.hover) glColor4d(1.00, 0.66, 0.60, 0.95);
    else         glColor4d(0.62, 0.62, 0.70, 0.55);
    glBegin(GL_LINE_LOOP);
        glVertex2d(x, y); glVertex2d(x+w, y); glVertex2d(x+w, y+h); glVertex2d(x, y+h);
    glEnd();
    glColor4d(1, 1, 1, b.hover ? 0.22 : 0.10);
    glBegin(GL_LINE_LOOP);
        glVertex2d(x+3, y+3); glVertex2d(x+w-3, y+3);
        glVertex2d(x+w-3, y+h-3); glVertex2d(x+3, y+h-3);
    glEnd();
    glColor4d(1,1,1,1);
    glDisable(GL_BLEND);

    // ---- label, with a shadow so it reads over any backdrop --------------
    // Written with the game's own alphabet picture, the same as everything
    // else. The plain white lettering underneath is only reached if that
    // picture is missing.
    double cx = x + w / 2.0, ty = y + h / 2.0 - 6;
    if (gameFontTex != 0)
    {
        double lsz = h * 0.42;
        double ly  = y + h / 2.0 - lsz * 0.5;
        drawGameTextCentered(cx + 1, ly - 1, b.label, lsz, 0, 0, 0, 175);
        if (b.hover) drawGameTextCentered(cx, ly, b.label, lsz, 255, 240, 225);
        else         drawGameTextCentered(cx, ly, b.label, lsz, 212, 206, 200);
    }
    else
    {
        iSetColor(0, 0, 0);
        drawCenteredText(cx + 1, ty - 1, const_cast<char*>(b.label),
                          GLUT_BITMAP_HELVETICA_18, 10);
        if (b.hover) iSetColor(255, 240, 225);
        else         iSetColor(206, 206, 214);
        drawCenteredText(cx, ty, const_cast<char*>(b.label),
                          GLUT_BITMAP_HELVETICA_18, 10);
    }

    // ---- hover markers on both sides ------------------------------------
    if (b.hover)
    {
        double off = 12 + 4 * pulse;
        iSetColor(230, 90, 70);
        double lx[3] = { x - off, x - off + 9, x - off };
        double ly[3] = { y + h/2 - 7, y + h/2, y + h/2 + 7 };
        iFilledPolygon(lx, ly, 3);
        double rx[3] = { x + w + off, x + w + off - 9, x + w + off };
        double ry[3] = { y + h/2 - 7, y + h/2, y + h/2 + 7 };
        iFilledPolygon(rx, ry, 3);
    }
}

#endif
