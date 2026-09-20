
#pragma once
#ifndef ENEMY_H
#define ENEMY_H

#include "GameUtility.h"
#include "Player.h"

// ENEMY_BRUTE is the level-2 character ("The Warden") - 200 health.
// ENEMY_SWAMP and ENEMY_HOUND are the two level-3 characters.
enum EnemyType { ENEMY_SHADOW, ENEMY_PRISONER, ENEMY_BRUTE,
                 ENEMY_SWAMP, ENEMY_HOUND };

// The bosses get the wide health bar at the top of the screen instead of the
// little bar over their head, and they shrug off hits mid-swing.
inline bool isBossType(EnemyType t)
{
    return t == ENEMY_BRUTE || t == ENEMY_SWAMP || t == ENEMY_HOUND;
}

struct Enemy
{
    double    x, y;
    double    health, maxHealth;
    double    speed;
    double    damage;         // how hard its hit lands
    double    touchRange;     // horizontal reach
    double    reachHeight;    // jump above this and it cannot touch you
    double    knockback;      // horizontal push left over from the last hit
    double    kbResist;       // 1.0 = normal, lower = shrugs off hits
    EnemyType type;
    bool      alive;
    bool      facingRight;
    bool      enraged;
    int       attackCooldown;
    int       windupTimer;    // telegraph before a heavy swing (boss only)
    int       windupTicks;    // how long that telegraph lasts for this type
    int       attackAnimTimer;// counts down across the whole swing, for drawing
    int       attackAnimLen;
    int       cooldownTicks;  // this type's gap between attacks
    double    rageSpeed;      // speed once badly hurt. 0 = this type never rages
    int       hitFlashTimer;
    bool      moving;         // was it chasing this tick?
    double    walkPhase;      // advances with distance travelled
};

std::vector<Enemy> enemies;

// One image per enemy kind. All three are measured automatically on load.
Sprite smallEnemyArt = emptySprite();   // level 1 small enemy
Sprite bigEnemyArt   = emptySprite();   // level 1 big enemy
Sprite bruteArt      = emptySprite();   // level 2 boss, The Warden
Sprite swampArt      = emptySprite();   // level 3, the swamp thing
Sprite houndArt      = emptySprite();   // level 3 boss, the gate hound
SpriteAnim bruteAttackAnim = emptyAnim();   // his swing
SpriteAnim swampAttackAnim = emptyAnim();   // the vine lash
SpriteAnim houndAttackAnim = emptyAnim();   // the fire breath

// walk cycles. The level 1 boss has real artwork for each direction; the
// other two have one sheet that gets mirrored.
DirAnim smallWalk = emptyDirAnim();
DirAnim bigWalk   = emptyDirAnim();
DirAnim bruteWalk = emptyDirAnim();

// swings for the level 1 pair (they are used in level 2 as well)
SpriteAnim smallAttackAnim = emptyAnim();
SpriteAnim bigAttackAnim   = emptyAnim();

void loadEnemyImages()
{
    // ---- LEVEL 1, small enemy ------------------------------------------
    // TO CHANGE IT: drop your PNG in Images/Enime/ under one of these names.
    smallEnemyArt = loadSpriteAny("Images/Enime/normalEnemeylevel1.png",
                                   "Images/Enime/normalEnemylevel1.png",
                                   "Images/Enime/Enime1.png");

    // ---- LEVEL 1, big enemy --------------------------------------------
    bigEnemyArt   = loadSpriteAny("Images/Enime/level1Boss.png",
                                   "Images/Enime/enemyBoss.png",
                                   "Images/Enime/EnemyBoss.png");
    // level1Boss.png is drawn facing LEFT. Saying so here stops his standing
    // pose being mirrored, which made him face the wrong way the moment he
    // stopped walking and squared up to attack.
    bigEnemyArt.facesLeft = true;

    // ---- LEVEL 2 boss ---------------------------------------------------
    // Falls back to the level 1 big enemy if no dedicated art is present.
    bruteArt      = loadSpriteAny("Images/Enime/enemyLevel2.png",
                                   "Images/Enime/Enime2.png",
                                   "Images/Enime/Boss.png");
    if (bruteArt.tex == 0) bruteArt = bigEnemyArt;

    // His swing. Same sheet rules as the player: one row, equal cells.
    bruteAttackAnim = loadAnim("Images/Enime/enemyLevel2Animation.png",
                                3,        // frames
                                1.3222,   // cell width / height
                                1.2132,   // cell height vs the character height
                                0.4937,   // body centre across the cell
                                0.0000);  // feet up from the cell bottom

    // ---- WALK CYCLES ----------------------------------------------------
    // The small enemy now has its own artwork for each direction, so nothing
    // is mirrored: level1SmallEnemyWalk.png walks right, ...WalkLeft.png walks
    // left. ENEMY_SHADOW is the grunt in level 1 AND in the level 2 wave, so
    // wiring it here covers both levels.
    smallWalk.right = loadAnim("Images/Enime/level1SmallEnemyWalk.png",
                                2,        // frames
                                0.7026,   // cell width / height
                                1.0000,   // cell height vs the character height
                                0.4969,   // body centre across the cell
                                0.0129,   // feet up from the cell bottom
                                false);   // artwork faces RIGHT
    smallWalk.left  = loadAnim("Images/Enime/level1SmallEnemyWalkLeft.png",
                                2, 0.7026, 1.0000, 0.5031, 0.0129,
                                true);    // artwork faces LEFT
    // If either sheet is missing, fall back to mirroring the one that loaded.
    if (smallWalk.right.tex == 0) smallWalk.right = smallWalk.left;
    if (smallWalk.left.tex  == 0) smallWalk.left  = smallWalk.right;

    // The level 1 boss has proper artwork for BOTH directions, so neither is
    // a mirror image: level1BossWalks.png walks right, ...EnemyWalks2 walks left.
    bigWalk.right = loadAnim("Images/Enime/level1BossWalks.png",
                              3,        // frames
                              0.7855,   // cell width / height
                              1.0000,   // cell height vs the character height
                              0.4934,   // body centre across the cell
                              0.0208,   // feet up from the cell bottom
                              false);   // artwork faces RIGHT
    bigWalk.left  = loadAnim("Images/Enime/level1BossEnemyWalks2.png",
                              3, 0.8286, 1.0000, 0.5000, 0.0214,
                              true);    // artwork faces LEFT
    if (bigWalk.right.tex == 0) bigWalk.right = bigWalk.left;
    if (bigWalk.left.tex  == 0) bigWalk.left  = bigWalk.right;

    bruteWalk.right = loadAnim("Images/Enime/level2BossWalks.png",
                                3, 0.8658, 1.0000, 0.4317, 0.0096, true);
    if (bruteWalk.right.tex == 0)
        bruteWalk.right = loadAnim("Images/Enime/level2EnemyWalk.png",
                                    2, 0.8358, 1.0000, 0.5597, 0.0125, false);
    bruteWalk.left = bruteWalk.right;

    // ---- SWINGS for the small and the big enemy -------------------------
    // Both were cut out of smallEnemyAnimations.png, which holds the whole
    // pose reference sheet for these two characters.
    smallAttackAnim = loadAnim("Images/Enime/smallEnemyAttack.png",
                                4, 1.5298, 1.0203, 0.4978, 0.0464);
    // level1BossAttack.png holds a guard pose and a punch, drawn facing RIGHT.
    // No mirrored copy is needed: animFlipFor turns it round for the left side.
    bigAttackAnim   = loadAnim("Images/Enime/level1BossAttack.png",
                                2,        // frames
                                1.2063,   // cell width / height
                                0.8314,   // a crouched stance is shorter than standing
                                0.3684,   // body centre across the cell
                                0.0238,   // feet up from the cell bottom
                                false);   // artwork faces RIGHT
    if (bigAttackAnim.tex == 0)
        bigAttackAnim = loadAnim("Images/Enime/bigEnemyAttack.png",
                                  1, 1.5985, 0.9496, 0.5735, 0.0455, false);

    // ---- LEVEL 3 --------------------------------------------------------
    // TO CHANGE THE LEVEL 3 ART: swap the files in Images/Enime/Level 3/.
    // Both must be PNGs with a transparent background, not JPEGs renamed to
    // .png, or they draw as solid rectangles.
    swampArt = loadSpriteAny("Images/Enime/Level 3/level3enemy.png",
                              "Images/Enime/level3enemy.png",
                              "Images/Enime/Level3/level3enemy.png");

    houndArt = loadSpriteAny("Images/Enime/Level 3/level3enemyBoss.png",
                              "Images/Enime/level3enemyBoss.png",
                              "Images/Enime/Level3/level3enemyBoss.png");

    // If the level 3 art is missing, borrow level 2's so the level still runs.
    if (swampArt.tex == 0) swampArt = bigEnemyArt;
    if (houndArt.tex == 0) houndArt = bruteArt;

    // Both swings were cut out of the 12-pose reference sheets. Both are drawn
    // facing RIGHT (the vines lash right, the fire breathes right), which is
    // the same direction the standing poses face, so nothing flips oddly when
    // an attack ends.
    swampAttackAnim = loadAnim("Images/Enime/Level 3/level3enemyAttack.png",
                                3,        // gather, spread, lash
                                1.2685,   // cell width / height
                                1.0000,   // cell height vs the character height
                                0.4987,   // body centre across the cell
                                0.0067,   // feet up from the cell bottom
                                false);   // artwork faces RIGHT
    if (swampAttackAnim.tex == 0)
        swampAttackAnim = loadAnim("Images/Enime/level3enemyAttack.png",
                                    3, 1.2685, 1.0000, 0.4987, 0.0067, false);

    houndAttackAnim = loadAnim("Images/Enime/Level 3/level3enemyBossAttack.png",
                                3,        // breath builds, burst, fade
                                1.5546, 1.0000, 0.4973, 0.0126, false);
    if (houndAttackAnim.tex == 0)
        houndAttackAnim = loadAnim("Images/Enime/level3enemyBossAttack.png",
                                    3, 1.5546, 1.0000, 0.4973, 0.0126, false);
}

// The level 3 pair have no walk sheets, so emptyAnim sends them down the
// "draw the standing pose" branch instead. Drop a sheet in and wire it here
// and they will start walking with no other change.
SpriteAnim noWalkAnim = emptyAnim();

inline const SpriteAnim& enemyWalkAnim(const Enemy& e)
{
    if (e.type == ENEMY_BRUTE)  return dirAnimFor(bruteWalk, e.facingRight);
    if (e.type == ENEMY_SHADOW) return dirAnimFor(smallWalk, e.facingRight);
    if (e.type == ENEMY_SWAMP)  return noWalkAnim;
    if (e.type == ENEMY_HOUND)  return noWalkAnim;
    return dirAnimFor(bigWalk, e.facingRight);
}

inline const SpriteAnim& enemyAttackAnim(const Enemy& e)
{
    if (e.type == ENEMY_BRUTE)  return bruteAttackAnim;
    if (e.type == ENEMY_SHADOW) return smallAttackAnim;
    if (e.type == ENEMY_SWAMP)  return swampAttackAnim;
    if (e.type == ENEMY_HOUND)  return houndAttackAnim;
    return bigAttackAnim;
}

inline const Sprite& enemyArt(const Enemy& e);

inline double enemyDrawHeight(const Enemy& e)
{
    if (e.type == ENEMY_BRUTE)  return BRUTE_DRAW_H;
    if (e.type == ENEMY_SHADOW) return SHADOW_DRAW_H;
    if (e.type == ENEMY_SWAMP)  return SWAMP_DRAW_H;
    if (e.type == ENEMY_HOUND)  return HOUND_DRAW_H;
    return PRISONER_DRAW_H;
}

inline const Sprite& enemyArt(const Enemy& e)
{
    if (e.type == ENEMY_BRUTE)  return bruteArt;
    if (e.type == ENEMY_SHADOW) return smallEnemyArt;
    if (e.type == ENEMY_SWAMP)  return swampArt;
    if (e.type == ENEMY_HOUND)  return houndArt;
    return bigEnemyArt;
}

inline double enemyDrawWidth(const Enemy& e)
{
    return enemyDrawHeight(e) * enemyArt(e).aspect;
}

// is anything of this exact kind still standing?
inline bool anyAliveOfType(EnemyType t)
{
    for (size_t i = 0; i < enemies.size(); i++)
        if (enemies[i].alive && enemies[i].type == t) return true;
    return false;
}

inline bool bossIsAlive()
{
    return anyAliveOfType(ENEMY_BRUTE);
}

// the boss currently on screen, or 0. Used by the wide health bar.
inline const Enemy* findBoss()
{
    for (size_t i = 0; i < enemies.size(); i++)
        if (isBossType(enemies[i].type)) return &enemies[i];
    return 0;
}

inline int aliveEnemyCount()
{
    int n = 0;
    for (size_t i = 0; i < enemies.size(); i++)
        if (enemies[i].alive) n++;
    return n;
}


Enemy makeEnemy(EnemyType type, double x, double y)
{
    Enemy e;
    e.x = x;
    e.y = y;
    e.type = type;
    e.knockback = 0.0;
    e.alive = true;
    e.enraged = false;
    e.facingRight = (x > player.x);
    e.attackCooldown = 0;
    e.windupTimer = 0;
    e.attackAnimTimer = 0;
    e.attackAnimLen = 1;
    e.cooldownTicks = ENEMY_ATTACK_COOLDOWN;
    e.rageSpeed = 0.0;
    e.hitFlashTimer = 0;
    e.moving = false;
    e.walkPhase = 0.0;

    if (type == ENEMY_BRUTE)
    {
        e.speed       = BRUTE_SPEED;
        e.maxHealth   = BRUTE_MAX_HEALTH;      // 200
        e.damage      = BRUTE_DAMAGE;
        e.touchRange  = BRUTE_TOUCH_RANGE;
        e.reachHeight = BRUTE_REACH_HEIGHT;
        e.kbResist    = BRUTE_KNOCKBACK_RESIST;
        e.windupTicks = BRUTE_WINDUP_TICKS;
        e.cooldownTicks = BRUTE_ATTACK_COOLDOWN;
        e.rageSpeed   = BRUTE_RAGE_SPEED;
    }
    else if (type == ENEMY_SWAMP)
    {
        e.speed       = SWAMP_SPEED;
        e.maxHealth   = SWAMP_MAX_HEALTH;      // 150
        e.damage      = SWAMP_DAMAGE;
        e.touchRange  = SWAMP_TOUCH_RANGE;
        e.reachHeight = SWAMP_REACH_HEIGHT;
        e.kbResist    = SWAMP_KNOCKBACK_RESIST;
        e.windupTicks = SWAMP_WINDUP_TICKS;
        e.cooldownTicks = SWAMP_ATTACK_COOLDOWN;
        e.rageSpeed   = SWAMP_RAGE_SPEED;
    }
    else if (type == ENEMY_HOUND)
    {
        e.speed       = HOUND_SPEED;
        e.maxHealth   = HOUND_MAX_HEALTH;      // 260
        e.damage      = HOUND_DAMAGE;
        e.touchRange  = HOUND_TOUCH_RANGE;
        e.reachHeight = HOUND_REACH_HEIGHT;
        e.kbResist    = HOUND_KNOCKBACK_RESIST;
        e.windupTicks = HOUND_WINDUP_TICKS;
        e.cooldownTicks = HOUND_ATTACK_COOLDOWN;
        e.rageSpeed   = HOUND_RAGE_SPEED;
    }
    else if (type == ENEMY_SHADOW)
    {
        e.speed       = SHADOW_SPEED;
        e.maxHealth   = SHADOW_MAX_HEALTH;
        e.damage      = ENEMY_TOUCH_DAMAGE;
        e.touchRange  = ENEMY_TOUCH_RANGE;
        e.reachHeight = ENEMY_REACH_HEIGHT;
        e.kbResist    = 1.0;
        e.windupTicks = 0;
    }
    else
    {
        e.speed       = PRISONER_SPEED;
        e.maxHealth   = PRISONER_MAX_HEALTH;
        e.damage      = ENEMY_TOUCH_DAMAGE;
        e.touchRange  = ENEMY_TOUCH_RANGE;
        e.reachHeight = ENEMY_REACH_HEIGHT;
        e.kbResist    = 1.0;
        e.windupTicks = 0;
    }

    e.health = e.maxHealth;
    return e;
}


// spawn x just outside the arena, alternating sides
inline double offscreenSpawnX()
{
    bool fromLeft = (rand() % 2) == 0;
    return fromLeft ? (ARENA_LEFT - 30 - rand() % 120)
                     : (ARENA_RIGHT + 30 + rand() % 120);
}

// Spawns exactly this many of each kind, all walking in from off screen.
// This is what the wave tables in LevelSettings.h drive. Appends, so it can
// also be used to top a fight up.
void spawnWaveCounts(int smallCount, int bigCount)
{
    for (int i = 0; i < smallCount; i++)
        enemies.push_back(makeEnemy(ENEMY_SHADOW, offscreenSpawnX(), GROUND_Y));

    for (int i = 0; i < bigCount; i++)
        enemies.push_back(makeEnemy(ENEMY_PRISONER, offscreenSpawnX(), GROUND_Y));
}

// Clears the arena and spawns row number `index` of a wave table.
// Every level uses this, so all three tables behave the same way.
void spawnWaveFrom(const Wave* table, int count, int index)
{
    enemies.clear();
    Wave w = waveAt(table, count, index);
    spawnWaveCounts(w.small, w.big);
}

// Level 1's waves, straight out of LEVEL1_WAVES in LevelSettings.h.
void spawnWave(int waveIndex)
{
    spawnWaveFrom(LEVEL1_WAVES, LEVEL1_WAVE_COUNT, waveIndex);
}

// The new level-2 character, walking in from the right.
void spawnBrute()
{
    enemies.push_back(makeEnemy(ENEMY_BRUTE, ARENA_RIGHT + 90, GROUND_Y));
}

// LEVEL 3, fight one: the swamp thing rises on the right.
void spawnSwamp()
{
    enemies.push_back(makeEnemy(ENEMY_SWAMP, ARENA_RIGHT + 90, GROUND_Y));
}

// LEVEL 3, fight two: the gate hound comes through from the right.
void spawnHound()
{
    enemies.push_back(makeEnemy(ENEMY_HOUND, ARENA_RIGHT + 110, GROUND_Y));
}


// ---------------------------------------------------------------------------
//  DRAWING
// ---------------------------------------------------------------------------

// Fallback art, used only if the PNGs fail to load.
void drawShadowBlob(const Enemy& e, bool flashWhite)
{
    if (flashWhite) iSetColor(235, 235, 235);
    else            iSetColor(5, 5, 5);

    iFilledCircle(e.x, e.y + 30, 13, 16);
    iFilledCircle(e.x - 8, e.y + 22, 9, 14);
    iFilledCircle(e.x + 9, e.y + 24, 10, 14);
    iFilledCircle(e.x, e.y + 14, 11, 14);

    if (!flashWhite)
    {
        double dir = e.facingRight ? 1.0 : -1.0;
        iSetColor(200, 20, 20);
        iFilledCircle(e.x + 3 * dir, e.y + 31, 1.6, 8);
    }
}

void drawEnemyShadow(const Enemy& e)
{
    double halfW = enemyDrawWidth(e) * 0.42;
    if (halfW < 12.0) halfW = 12.0;
    iSetColor(10, 10, 10);
    iFilledEllipse(e.x, GROUND_Y + 3, halfW, halfW * 0.28, 18);
}

void drawEnemyHealthBar(const Enemy& e)
{
    // every boss gets the wide bar at the top of the screen instead
    if (isBossType(e.type)) return;
    if (e.health >= e.maxHealth) return;   // only once they have been hit

    double h   = enemyDrawHeight(e);
    double w   = 40.0;
    double y   = e.y + h + 8.0;
    double pct = clampd(e.health / e.maxHealth, 0.0, 1.0);

    iSetColor(30, 30, 30);
    iFilledRectangle(e.x - w / 2.0, y, w, 5);
    iSetColor(190, 40, 40);
    iFilledRectangle(e.x - w / 2.0, y, w * pct, 5);
}

// red ring that swells while the Warden loads up a swing - your cue to jump
void drawBruteWindup(const Enemy& e)
{
    if (e.windupTimer <= 0) return;
    if (enemyAttackAnim(e).tex != 0) return;  // the animation is the telegraph now

    double t = 1.0 - (double)e.windupTimer / (double)e.windupTicks;  // 0 -> 1
    double r = e.touchRange * (0.35 + 0.75 * t);
    double cy = e.y + enemyDrawHeight(e) * 0.35;

    iSetColor(200, 40, 40);
    iCircle(e.x, cy, r, 26);
    iCircle(e.x, cy, r - 3, 26);
}

void drawEnemy(const Enemy& e)
{
    if (!e.alive) return;

    bool flash = e.hitFlashTimer > 0 && (e.hitFlashTimer / 2) % 2 == 0;

    drawEnemyShadow(e);

    const Sprite& art = enemyArt(e);

    if (art.tex != 0)
    {
        double h = enemyDrawHeight(e);

        double tr = 255, tg = 255, tb = 255;
        if (e.enraged)                          { tr = 255; tg = 200; tb = 190; }
        if (e.windupTimer > 0)                  { tr = 255; tg = 190; tb = 130; }
        if (flash)                              { tr = 255; tg = 120; tb = 120; }

        const SpriteAnim& swing = enemyAttackAnim(e);
        const SpriteAnim& stride = enemyWalkAnim(e);

        if (e.attackAnimTimer > 0 && swing.tex != 0)
        {
            int done  = e.attackAnimLen - e.attackAnimTimer;
            int frame = done * swing.count / e.attackAnimLen;
            drawAnimFrame(swing, frame, e.x, e.y, h,
                           animFlipFor(swing, e.facingRight), tr, tg, tb, 255);
        }
        else if (e.moving && stride.tex != 0)
        {
            int frame = (int)(e.walkPhase / WALK_CYCLE_PX) % stride.count;
            drawAnimFrame(stride, frame, e.x, e.y, h,
                           animFlipFor(stride, e.facingRight), tr, tg, tb, 255);
        }
        else
        {
            drawSpriteArt(art, e.x, e.y, h,
                           spriteFlipFor(art, e.facingRight), tr, tg, tb, 255);
        }
    }
    else if (e.type == ENEMY_SHADOW)
    {
        drawShadowBlob(e, flash);
    }
    else
    {
        double sc = isBossType(e.type) ? 1.4 : 0.8;
        drawHumanoidSilhouette(e.x, e.y, sc, e.facingRight, false, 200, 20, 20, flash);
    }

    drawBruteWindup(e);
    drawEnemyHealthBar(e);
}

void drawAllEnemies()
{
    // draw the boss last so he sits in front of the grunts
    for (size_t i = 0; i < enemies.size(); i++)
        if (!isBossType(enemies[i].type)) drawEnemy(enemies[i]);

    for (size_t i = 0; i < enemies.size(); i++)
        if (isBossType(enemies[i].type)) drawEnemy(enemies[i]);
}


// ---------------------------------------------------------------------------
//  COMBAT
// ---------------------------------------------------------------------------
void performPlayerAttack()
{
    bool landed = false;
    double dir = player.facingRight ? 1.0 : -1.0;

    for (size_t i = 0; i < enemies.size(); i++)
    {
        Enemy& e = enemies[i];
        if (!e.alive) continue;

        double dx = e.x - player.x;
        double dy = fabs(e.y - player.y);
        bool inFront = (dir > 0) ? (dx > 0 && dx <= PLAYER_ATTACK_RANGE)
                                  : (dx < 0 && -dx <= PLAYER_ATTACK_RANGE);

        if (inFront && dy <= PLAYER_ATTACK_VSPAN)
        {
            e.health -= PLAYER_ATTACK_DAMAGE;
            e.hitFlashTimer = HIT_FLASH_TICKS;
            e.knockback = dir * ENEMY_KNOCKBACK * e.kbResist;
            landed = true;
            lastHitX = e.x;
            lastHitY = e.y + enemyDrawHeight(e) * 0.45;

            // a solid hit interrupts a grunt's swing, but never a boss's
            if (!isBossType(e.type)) e.windupTimer = 0;

            if (e.health <= 0)
            {
                e.health = 0;
                e.alive = false;
                killCount++;
            }
        }
    }

    if (landed)
    {
        hitPauseTimer = HIT_PAUSE_TICKS;

        // the kick connected. Same sound in every level, and it plays once per
        // kick however many enemies were caught by it.
        sfxPlay(SFX_PLAYER_HIT);
    }
}


void updateEnemies()
{
    for (size_t i = 0; i < enemies.size(); i++)
    {
        Enemy& e = enemies[i];
        if (!e.alive) continue;

        if (e.attackCooldown > 0)  e.attackCooldown--;
        if (e.hitFlashTimer > 0)   e.hitFlashTimer--;
        if (e.attackAnimTimer > 0) e.attackAnimTimer--;

        double dx          = player.x - e.x;
        double heightAbove = player.y - e.y;   // > 0 while the player is airborne

        e.y = GROUND_Y;

        // ---- bosses get angrier as they lose health --------------------------
        // rageSpeed is 0 for the ordinary enemies, so they never trigger this.
        if (e.rageSpeed > 0.0 && !e.enraged &&
            e.health <= e.maxHealth * BOSS_RAGE_THRESHOLD)
        {
            e.enraged = true;
            e.speed   = e.rageSpeed;
        }

        // ---- winding up: planted in place, then the hit lands ---------------
        if (e.windupTimer > 0)
        {
            e.windupTimer--;
            e.facingRight = (dx > 0);

            if (e.windupTimer == 0)
            {
                // you had the whole telegraph to jump or back off
                bool stillThere = fabs(player.x - e.x) <= e.touchRange * 1.15 &&
                                  (player.y - e.y)     <= e.reachHeight;
                if (stillThere && player.hitStunTimer <= 0)
                    damagePlayer(e.damage, e.x);

                e.attackCooldown = e.cooldownTicks;

                if (gameState == STATE_GAMEOVER) return;
            }
            continue;    // no movement while committed to the swing
        }

        // ---- knockback overrides the chase while it lasts -------------------
        if (fabs(e.knockback) > 0.1)
        {
            e.x += e.knockback;
            e.knockback *= 0.75;
        }
        else
        {
            e.knockback = 0.0;
        }

        e.facingRight = (dx > 0);

        // ---- chase horizontally - everyone is stuck to the floor ------------
        if (fabs(dx) > e.touchRange * 0.7 && e.knockback == 0.0)
            e.x += signd(dx) * e.speed;

        // ---- keep the wide sprites from overlapping -------------------------
        for (size_t j = 0; j < enemies.size(); j++)
        {
            if (j == i || !enemies[j].alive) continue;

            double minGap = (enemyDrawWidth(e) + enemyDrawWidth(enemies[j])) * 0.42;
            double sep    = e.x - enemies[j].x;

            if (fabs(sep) < minGap)
            {
                double push = (sep == 0.0) ? ((i < j) ? -0.5 : 0.5) : signd(sep) * 0.5;
                e.x += push;
            }
        }

        e.x = clampd(e.x, ARENA_LEFT - 160, ARENA_RIGHT + 160);

        // ---- can it actually reach the player? ------------------------------
        bool inReach = fabs(dx) <= e.touchRange && heightAbove <= e.reachHeight;

        if (inReach && e.attackCooldown <= 0 && player.hitStunTimer <= 0)
        {
            if (e.windupTicks > 0)
            {
                e.windupTimer     = e.windupTicks;   // boss telegraphs first
                e.attackAnimLen   = e.windupTicks + BRUTE_SWING_TICKS;
                e.attackAnimTimer = e.attackAnimLen;
            }
            else
            {
                e.attackCooldown  = e.cooldownTicks;
                e.attackAnimLen   = GRUNT_ATTACK_ANIM;   // the swing we draw
                e.attackAnimTimer = GRUNT_ATTACK_ANIM;
                damagePlayer(e.damage, e.x);

                if (gameState == STATE_GAMEOVER) return;
            }
        }
    }
}

#endif
