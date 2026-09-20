#pragma once
#ifndef LEVELSETTINGS_H
#define LEVELSETTINGS_H

// ===========================================================================
//
//                    * * *  L E V E L   S E T T I N G S  * * *
//
//   THIS IS THE FILE TO EDIT WHEN YOU WANT TO CHANGE THE WAVES.
//   You do not have to touch any other file. Change a number here, press F5,
//   and the game plays differently.
//
//   Everything below is read once when a layer starts.
//
// ===========================================================================


// ---------------------------------------------------------------------------
//  WHAT ONE WAVE IS
//
//  A wave is just "how many of each enemy walk in together".
//    smallEnemies = the little enemy   (normalEnemeylevel1.png)
//    bigEnemies   = the big enemy      (level1Boss.png)
//    warspanEnemies  = WARSPAN, the layer 3 crawler  (Level 3/level3EnemyWarspan.png)
//  They all walk in from off screen at the same time.
// ---------------------------------------------------------------------------
struct Wave
{
    int smallEnemies;
    int bigEnemies;
    int warspanEnemies;
};
// NOTE: these are NOT called "small" and "big". Windows' own headers contain
// #define small char, so "int small;" turned into "int char;" and the whole
// file failed to compile. The names below just avoid that clash - the
// { 2, 1, 0 } lines just fill the three fields in order. Leaving a number off
// the end of a line is allowed and counts as a zero, so { 2, 1 } still means
// "2 small, 1 big, no warspan".


// ---------------------------------------------------------------------------
//  LEVEL 1 WAVES
//
//  ONE LINE = ONE WAVE. They run top to bottom.
//  When the last wave is cleared, layer 1 is finished.
//
//  TO ADD A WAVE:     add another  { small, big, warspan },  line.
//  TO REMOVE A WAVE:  delete a line.
//  TO MAKE IT HARDER: raise the numbers.
//
//  You do NOT have to update any count anywhere - the number of waves is
//  worked out from how many lines you wrote.
// ---------------------------------------------------------------------------
const Wave LEVEL1_WAVES[] =
{
    { 2, 1, 0 },   // wave 1  ->  1 small enemy + 1 big enemy
    { 2, 1, 0 },  
	{ 2, 1, 0 },
	{ 2, 1, 0 },
	{ 3, 1, 0 },// wave 2  ->  1 small enemy + 1 big enemy
};


// ---------------------------------------------------------------------------
//  LEVEL 2 WAVES  (the enemies BEFORE the Warden)
//
//  Same rules. When these waves are cleared, THE WARDEN arrives.
// ---------------------------------------------------------------------------
const Wave LEVEL2_WAVES[] =
{
    { 2, 1, 0 }, 
	{ 2, 1, 0 },
	{ 3, 1, 0 },
	{ 3, 1, 0 },// wave 1  ->  2 small enemies + 1 big enemy
};


// ---------------------------------------------------------------------------
//  LEVEL 3 WAVES  (the enemies BEFORE the swamp thing)
//
//  TWO WAVES OF WARSPAN, the new layer 3 crawler. Both lines below ask for warspan
//  only, which is why the first two numbers are 0.
//
//  Same rules as every other table: add a line for another wave, change the
//  third number to send in more or fewer warspan. When these waves are cleared
//  the SWAMP THING rises, and after that the GATE HOUND.
// ---------------------------------------------------------------------------
const Wave LEVEL3_WAVES[] =
{
    { 0, 0, 2 },
	{ 0, 0, 2 }, // wave 1  ->  2 warspan
    { 0, 0, 3 },
	{ 0, 0, 3 },// wave 2  ->  3 warspan
};


// ---------------------------------------------------------------------------
//  WHICH LAYER THE "START" BUTTON BEGINS ON
//
//  Normally 1. Set it to 2, 3 or 4 while you are working on a later layer so
//  you do not have to play through the earlier ones every time you press F5.
//  SET IT BACK TO 1 BEFORE YOU HAND THE GAME IN.
// ---------------------------------------------------------------------------
const int START_LEVEL = 1;      // 1, 2, 3 or 4


// ---------------------------------------------------------------------------
//  HOW LONG THE GAME PAUSES BETWEEN WAVES  (62 ticks = about 1 second)
// ---------------------------------------------------------------------------
const int WAVE_CLEAR_DELAY_TICKS = 200;


// ---------------------------------------------------------------------------
//  THE HERO'S SPECIAL MOVE  --  the H key
//
//  It is a wide, heavy blow that hits EVERYTHING standing near you, on both
//  sides at once, and it is unlocked from this layer onwards.
// ---------------------------------------------------------------------------
const int SPECIAL_MOVE_FROM_LEVEL = 2;   // 2 = layer 2 up to the final layer


// ---------------------------------------------------------------------------
//  HOW MANY PICTURES ARE IN EACH ANIMATION SHEET
//
//  An animation sheet is one PNG holding several poses side by side in a
//  single row. The game slices it into this many equal pieces, so if a
//  character's animation looks squashed or shows two bodies at once, the
//  number below is wrong - count the poses in the PNG and put that number in.
//
//  The three newest sheets, with the counts they were actually cut to:
// ---------------------------------------------------------------------------
const int WARSPAN_ATTACK_FRAMES    = 5;   // Level 3/level3NormallEnemyAnimation.png
const int L4BOSS2_ANIM_FRAMES   = 5;   // Level 4/level4BosssAnimation.png
const int L4BOSS2_WALK_FRAMES   = 4;   // ... the walk cycle off the same sheet
const int PLAYER_SPECIAL_FRAMES = 5;   // CharacterImages/SpecialMovePlayer.png


// ---------------------------------------------------------------------------
//  HOW A RUN IS SCORED
//
//  The score is written to scores.txt when you put MEAFESTO down, next to the
//  name you typed at the start, and the board is shown straight afterwards.
//  Only finished runs are saved; dying does not put you on the board.
//
//      score = kills x SCORE_PER_KILL
//            + layers cleared x SCORE_PER_LAYER
//            + whatever is left of SCORE_TIME_BONUS after the clock is charged
//
//  So killing more is worth more, getting deeper is worth more, and taking
//  your time slowly gives the speed bonus away. The bonus never goes below 0,
//  so a slow run still scores on kills and depth.
// ---------------------------------------------------------------------------
const int SCORE_PER_KILL      = 100;
const int SCORE_PER_LAYER     = 500;
const int SCORE_TIME_BONUS    = 3000;   // the most the speed bonus can be worth
const int SCORE_TIME_COST     = 3;      // taken off the bonus for every second

// How many names the board remembers, and how many of them it shows.
const int SCORE_MAX_ENTRIES   = 64;
const int SCORE_SHOW_ROWS     = 8;


// ===========================================================================
//  HOW FAST THE WHOLE GAME RUNS
//
//  1.00 is full speed - 62.5 game steps a second, which is what every speed,
//  cooldown and animation length in the game was written against.
//
//  Lowering this does not change any of those numbers. It just runs the steps
//  further apart, so everything slows down together and keeps its timing
//  relative to everything else: the hero, the enemies, the swings, the
//  animations, the conversations. The picture stays exactly as smooth - it is
//  the game that slows down, not the drawing.
//
//  0.60 = six tenths of full speed, which is 37.5 steps a second.
//  Put it back to 1.00 for the original pace. The run clock and the score
//  stay honest either way: they count real seconds, not steps.
// ===========================================================================
const double GAME_SPEED = 0.60;


// ===========================================================================
//  HOW THE ENEMIES IN A WAVE ARRIVE
//
//  A wave used to dump everybody on screen at once. Now the wave is a queue:
//  a few walk in, and every time one of them dies the next one from that same
//  wave walks in after a short wait. The wave is only over when the queue is
//  empty AND nothing is left standing, so no wave is any easier than it was -
//  the same enemies still have to be beaten, they just arrive spread out.
//
//  ENEMY_FEED_SECONDS is that wait, in REAL seconds. It stays 1.8 seconds on
//  the clock whatever GAME_SPEED is set to.
//
//  WAVE_ON_SCREEN is how many are allowed to be up at the same time.
//    1 = strictly one at a time. Nobody walks in until the last one is dead.
//    2 = normally two at once, the rest queued behind them  (the default)
//    9 = nobody ever waits for a death, but the wave still files in one every
//        ENEMY_FEED_SECONDS rather than landing together
//  There is no setting that puts the old all-at-once behaviour back; the whole
//  point of the queue is that they arrive spread out.
//
//  These apply to the enemy waves in layers 1, 2 and 3. Bosses are not waves
//  and are not affected: the Warden, the rot, the hound and Meafesto arrive
//  exactly as they did.
// ===========================================================================
const double ENEMY_FEED_SECONDS = 1.8;
const int    WAVE_ON_SCREEN     = 2;

// ===========================================================================
//  Below here is plumbing. You do not need to change any of it.
// ===========================================================================

// How many waves each table holds, counted automatically from the lines above.
const int LEVEL1_WAVE_COUNT = (int)(sizeof(LEVEL1_WAVES) / sizeof(LEVEL1_WAVES[0]));
const int LEVEL2_WAVE_COUNT = (int)(sizeof(LEVEL2_WAVES) / sizeof(LEVEL2_WAVES[0]));
const int LEVEL3_WAVE_COUNT = (int)(sizeof(LEVEL3_WAVES) / sizeof(LEVEL3_WAVES[0]));

// Reads one row out of a table, clamped so a bad index can never crash.
inline Wave waveAt(const Wave* table, int count, int index)
{
    Wave empty;
    empty.smallEnemies = 0;
    empty.bigEnemies   = 0;
    empty.warspanEnemies  = 0;
    if (table == 0 || count <= 0) return empty;
    if (index < 0)      index = 0;
    if (index >= count) index = count - 1;
    return table[index];
}

#endif
