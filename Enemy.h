
#pragma once
#ifndef ENEMY_H
#define ENEMY_H

#include "GameUtility.h"
#include "Player.h"

// ENEMY_BRUTE is the level-2 character ("The Warden") - 200 health.
// ENEMY_SWAMP and ENEMY_HOUND are the two level-3 characters.
// ENEMY_L4BOSS is MEAFESTO, the layer 4 boss. He is the only enemy with two
// lives: `secondLife` flips to true after the transformation and from then on
// he uses different art, speed and damage.
// ENEMY_WARSPAN is the layer 3 crawler that arrives in waves before the swamp
// thing. He is an ordinary enemy, not a boss.
enum EnemyType { ENEMY_SHADOW, ENEMY_PRISONER, ENEMY_BRUTE,
                 ENEMY_SWAMP, ENEMY_HOUND,
                 ENEMY_L4BOSS, ENEMY_SELLOID,
                 ENEMY_WARSPAN };

// The bosses get the wide health bar at the top of the screen instead of the
// little bar over their head, and they shrug off hits mid-swing.
inline bool isBossType(EnemyType t)
{
    return t == ENEMY_BRUTE || t == ENEMY_SWAMP || t == ENEMY_HOUND
        || t == ENEMY_L4BOSS;
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
    bool      secondLife;     // MEAFESTO only: true once he has transformed
    bool      hasSummoned;    // MEAFESTO only: the half-health summon fires once
    bool      entered;        // has it walked in off the edge of the arena yet?
    int       hitFlashTimer;
    bool      moving;         // was it chasing this tick?
    double    walkPhase;      // advances with distance travelled
    double    px, py;         // where it was one step ago - see snapshotActors()
};

std::vector<Enemy> enemies;

// One image per enemy kind. All three are measured automatically on load.
Sprite smallEnemyArt = emptySprite();   // level 1 small enemy
Sprite bigEnemyArt   = emptySprite();   // level 1 big enemy
Sprite bruteArt      = emptySprite();   // level 2 boss, The Warden
Sprite swampArt      = emptySprite();   // level 3, the swamp thing
Sprite houndArt      = emptySprite();   // level 3 boss, the gate hound
Sprite l4Boss1Art    = emptySprite();   // level 4 boss, first life
Sprite l4Boss2Art    = emptySprite();   // level 4 boss, second life
Sprite selloidArt    = emptySprite();   // the summoned wraith
Sprite warspanArt       = emptySprite();   // level 3 waves, WARSPAN
SpriteAnim bruteAttackAnim = emptyAnim();   // his swing
SpriteAnim swampAttackAnim = emptyAnim();   // the vine lash
SpriteAnim houndAttackAnim = emptyAnim();   // the fire breath
SpriteAnim l4Boss1AttackAnim = emptyAnim();
SpriteAnim l4Boss2AttackAnim = emptyAnim();
SpriteAnim selloidAttackAnim = emptyAnim();
SpriteAnim warspanAttackAnim    = emptyAnim();
SpriteAnim l4TransformAnim   = emptyAnim();   // the change between his two lives
DirAnim    l4Boss1Walk       = emptyDirAnim();
DirAnim    l4Boss2Walk       = emptyDirAnim();   // the transformed form's stride

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

    // THE WARDEN now has proper artwork for BOTH directions, so neither side
    // is a mirror image any more: Level2BossWardenRightSide.png walks right,
    // Level2BossWardenLeftSide.png walks left. Three frames each - leg up,
    // legs passing, leg down.
    bruteWalk.right = loadAnim("Images/Enime/Level2BossWardenRightSide.png",
                                3,        // frames
                                0.9786,   // cell width / height
                                1.0000,   // cell height vs the character height
                                0.4956,   // body centre across the cell
                                0.0000,   // feet on the cell floor
                                false);   // artwork faces RIGHT
    bruteWalk.left  = loadAnim("Images/Enime/Level2BossWardenLeftSide.png",
                                3, 1.0310, 1.0000, 0.4979, 0.0000,
                                true);    // artwork faces LEFT

    // If either new sheet is missing, fall back to the old mirrored one so the
    // level still runs.
    if (bruteWalk.right.tex == 0 && bruteWalk.left.tex == 0)
    {
        bruteWalk.right = loadAnim("Images/Enime/level2BossWalks.png",
                                    3, 0.8658, 1.0000, 0.4317, 0.0096, true);
        if (bruteWalk.right.tex == 0)
            bruteWalk.right = loadAnim("Images/Enime/level2EnemyWalk.png",
                                        2, 0.8358, 1.0000, 0.5597, 0.0125, false);
    }
    if (bruteWalk.right.tex == 0) bruteWalk.right = bruteWalk.left;
    if (bruteWalk.left.tex  == 0) bruteWalk.left  = bruteWalk.right;

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

    // ---- LEVEL 3, WARSPAN --------------------------------------------------
    // The crawler that comes in waves. level3EnemyWarspan.png is already a clean
    // cut-out, so his width and crop are measured straight off the picture.
    // The picture files still carry his old name on disk. Rename them to
    // level3EnemyWarspan.png / level3WarspanAttack.png whenever you like - the
    // new name is looked for first and the old one is the fallback.
    warspanArt = loadSpriteAny("Images/Enime/Level 3/level3EnemyWarspan.png",
                                "Images/Enime/Level 3/level3EnemyZunu.png",
                                "Images/Enime/level3EnemyZunu.png");
    warspanArt.facesLeft = false;    // he is drawn facing RIGHT
    if (warspanArt.tex == 0) warspanArt = smallEnemyArt;

    // His slash. level3NormallEnemyAnimation.png is a whole reference page -
    // four rows of poses, captions, frame numbers and a dark background - so
    // the game cannot read it directly. level3WarspanAttack.png is its BASIC
    // ATTACK row, cut out and laid down as one row of five equal cells, which
    // is the shape every animation in this game has to be in.
    //
    // loadAnimCutout, not loadAnim: a sheet that still has its background
    // baked in is ignored rather than drawn as a black block, so he falls back
    // to his standing picture instead of looking broken.
    warspanAttackAnim = loadAnimCutout("Images/Enime/Level 3/level3WarspanAttack.png",
                                        WARSPAN_ATTACK_FRAMES,
                                        2.2338,   // cell width / height - the slash is wide
                                        1.1407,   // cell height vs the character height
                                        0.4985,   // body centre across the cell
                                        0.0000,   // feet on the cell floor
                                        false);   // artwork faces RIGHT
    if (warspanAttackAnim.tex == 0)
        warspanAttackAnim = loadAnimCutout("Images/Enime/Level 3/level3ZunuAttack.png",
                                            WARSPAN_ATTACK_FRAMES,
                                            2.2338, 1.1407, 0.4985, 0.0000, false);

    houndAttackAnim = loadAnim("Images/Enime/Level 3/level3enemyBossAttack.png",
                                3,        // breath builds, burst, fade
                                1.5546, 1.0000, 0.4973, 0.0126, false);
    if (houndAttackAnim.tex == 0)
        houndAttackAnim = loadAnim("Images/Enime/level3enemyBossAttack.png",
                                    3, 1.5546, 1.0000, 0.4973, 0.0126, false);

    // ---- LEVEL 4 --------------------------------------------------------
    // MEAFESTO's two forms, his minions, and the change between the two.
    l4Boss1Art = loadSpriteAny("Images/Enime/Level 4/level4Boss1stLife.png",
                                "Images/Enime/level4Boss1stLife.png", 0);
    // level4Boss2ndLifeIdle.png is his standing pose cut out of the same sheet
    // as the animations, so the still and the moving art are the same creature.
    // (level4Boss2ndLife.png was a crop of the reference sheet with the words
    // "MAIN VIEW", the colour swatches and a black background still on it, so
    // it drew as a lettered black box. It is only the fallback now.)
    l4Boss2Art = loadSpriteAny("Images/Enime/Level 4/level4Boss2ndLifeIdle.png",
                                "Images/Enime/Level 4/level4Boss2ndLife.png",
                                "Images/Enime/level4Boss2ndLife.png");
    // LAYER 4's ordinary enemy. level4NormalEnemy.png is a four view
    // turnaround, so the side view was cut out of it into
    // level4NormalEnemyIdle.png - the other three are front and back views and
    // would read as the wrong character in a side-on fight.
    selloidArt = loadSpriteAny("Images/Enime/Level 4/level4NormalEnemyIdle.png",
                                "Images/Enime/Level 4/selloidIdle.png",
                                "Images/Enime/Level 4/selloid.png");
    selloidArt.facesLeft = true;   // the side view is drawn facing LEFT

    if (l4Boss1Art.tex == 0) l4Boss1Art = houndArt;
    if (l4Boss2Art.tex == 0) l4Boss2Art = l4Boss1Art;
    if (selloidArt.tex == 0) selloidArt = smallEnemyArt;

    // His first form walks facing LEFT; the swing is drawn facing RIGHT.
    l4Boss1Walk.left  = loadAnim("Images/Enime/Level 4/level4Boss1stWalk.png",
                                  6,        // frames
                                  0.8286,   // cell width / height
                                  1.0500,   // cell height vs the character height
                                  0.5230,   // body centre across the cell
                                  0.0095,   // feet up from the cell bottom
                                  true);    // artwork faces LEFT
    l4Boss1Walk.right = l4Boss1Walk.left;

    l4Boss1AttackAnim = loadAnim("Images/Enime/Level 4/level4Boss1stAttack.png",
                                  3,        // gather, slash, energy blast
                                  0.9931, 1.0360, 0.4965, 0.0000, false);
    // The transformed boss's animation now comes from level4BosssAnimation.png.
    // That file is a whole character reference page - main view, four view
    // angles, an FX panel, and rows for idle / walk / run / attack / breath /
    // jump / roar / hurt / death - so the game cannot read it as it stands.
    // Three strips were cut out of it into the shape the game needs:
    //    level4Boss2ndSwipe.png    <- the ATTACK (MELEE SWIPE) row, 5 poses
    //    level4Boss2ndWalk.png     <- the WALK CYCLE row, 4 poses
    //    level4Boss2ndLifeIdle.png <- one IDLE pose, his standing picture
    // Put a new reference page in and those three strips have to be re-cut;
    // the counts live in LevelSettings.h.
    l4Boss2AttackAnim = loadAnimCutout("Images/Enime/Level 4/level4Boss2ndSwipe.png",
                                        L4BOSS2_ANIM_FRAMES,
                                        1.3643,   // cell width / height
                                        0.9149,   // cell height vs the character height
                                        0.4943,   // body centre across the cell
                                        0.0233,   // the swipe dips below his feet
                                        false);   // artwork faces RIGHT
    if (l4Boss2AttackAnim.tex == 0)
        l4Boss2AttackAnim = loadAnim("Images/Enime/Level 4/level4Boss2ndAttack.png",
                                      3, 1.1667, 1.0909, 0.4898, 0.0000, false);

    // ...and his second form now WALKS, off the same sheet's walk cycle,
    // instead of sliding along in his standing pose.
    l4Boss2Walk.right = loadAnimCutout("Images/Enime/Level 4/level4Boss2ndWalk.png",
                                        L4BOSS2_WALK_FRAMES,
                                        0.9862,   // cell width / height
                                        1.0284,   // cell height vs the character height
                                        0.4965,   // body centre across the cell
                                        0.0000,   // feet on the cell floor
                                        false);   // artwork faces RIGHT
    l4Boss2Walk.left = l4Boss2Walk.right;   // one sheet, mirrored for the other way
    // His swing. level4NormalEnemyAnimation.png is a whole reference page -
    // idle, walk, run, jump, land, four attacks, hit, death and a rage pose,
    // every one of them captioned - so the game cannot read it as it stands.
    // The four ATTACK poses were cut out of it into one row of equal cells.
    selloidAttackAnim = loadAnimCutout("Images/Enime/Level 4/level4NormalEnemyAttack.png",
                                        4,        // slash, double claw, spin, leap
                                        1.6709,   // cell width / height
                                        0.8404,   // cell height vs the character height
                                        0.4987,   // body centre across the cell
                                        0.0000,   // feet on the cell floor
                                        false);   // artwork faces RIGHT
    if (selloidAttackAnim.tex == 0)
        selloidAttackAnim = loadAnim("Images/Enime/Level 4/selloidAttack.png",
                                      3, 1.6754, 1.0411, 0.4974, 0.0000, false);

    // Not a character pose - this one is a picture panel the level draws big
    // and centred while he changes form.
    // level4TransformSoft.png is the same eleven pictures with their edges
    // faded out. They are full rectangles of red smoke, and a hard rectangle
    // standing in the arena reads as a photo pasted on the screen; fading the
    // edges turns it into a column of fire around him instead. The original
    // sheet is the fallback.
    l4TransformAnim   = loadAnim("Images/Enime/Level 4/level4TransformSoft.png",
                                  11, 1.2632, 1.0000, 0.5000, 0.0000, false);
    if (l4TransformAnim.tex == 0)
        l4TransformAnim = loadAnim("Images/Enime/Level 4/level4Transform.png",
                                    11, 1.2632, 1.0000, 0.5000, 0.0000, false);
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
    if (e.type == ENEMY_SELLOID) return noWalkAnim;
    if (e.type == ENEMY_WARSPAN)   return noWalkAnim;   // he has no stride sheet
    if (e.type == ENEMY_L4BOSS)
        return dirAnimFor(e.secondLife ? l4Boss2Walk : l4Boss1Walk, e.facingRight);
    return dirAnimFor(bigWalk, e.facingRight);
}

inline const SpriteAnim& enemyAttackAnim(const Enemy& e)
{
    if (e.type == ENEMY_BRUTE)  return bruteAttackAnim;
    if (e.type == ENEMY_SHADOW) return smallAttackAnim;
    if (e.type == ENEMY_SWAMP)  return swampAttackAnim;
    if (e.type == ENEMY_HOUND)  return houndAttackAnim;
    if (e.type == ENEMY_SELLOID) return selloidAttackAnim;
    if (e.type == ENEMY_WARSPAN)   return warspanAttackAnim;
    if (e.type == ENEMY_L4BOSS)
        return e.secondLife ? l4Boss2AttackAnim : l4Boss1AttackAnim;
    return bigAttackAnim;
}

inline const Sprite& enemyArt(const Enemy& e);

inline double enemyDrawHeight(const Enemy& e)
{
    if (e.type == ENEMY_BRUTE)  return BRUTE_DRAW_H;
    if (e.type == ENEMY_SHADOW) return SHADOW_DRAW_H;
    if (e.type == ENEMY_SWAMP)  return SWAMP_DRAW_H;
    if (e.type == ENEMY_HOUND)  return HOUND_DRAW_H;
    if (e.type == ENEMY_SELLOID) return SELLOID_DRAW_H;
    if (e.type == ENEMY_WARSPAN)   return WARSPAN_DRAW_H;
    if (e.type == ENEMY_L4BOSS)
        return e.secondLife ? L4BOSS_DRAW_H_LIFE2 : L4BOSS_DRAW_H;
    return PRISONER_DRAW_H;
}

inline const Sprite& enemyArt(const Enemy& e)
{
    if (e.type == ENEMY_BRUTE)  return bruteArt;
    if (e.type == ENEMY_SHADOW) return smallEnemyArt;
    if (e.type == ENEMY_SWAMP)  return swampArt;
    if (e.type == ENEMY_HOUND)  return houndArt;
    if (e.type == ENEMY_SELLOID) return selloidArt;
    if (e.type == ENEMY_WARSPAN)   return warspanArt;
    if (e.type == ENEMY_L4BOSS) return e.secondLife ? l4Boss2Art : l4Boss1Art;
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
    e.secondLife = false;
    e.hasSummoned = false;
    e.entered = (x >= ARENA_LEFT && x <= ARENA_RIGHT);
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
    else if (type == ENEMY_L4BOSS)
    {
        e.speed       = L4BOSS_SPEED;
        e.maxHealth   = L4BOSS_LIFE1_HEALTH;   // his FIRST life
        e.damage      = L4BOSS_DAMAGE;
        e.touchRange  = L4BOSS_TOUCH_RANGE;
        e.reachHeight = L4BOSS_REACH_HEIGHT;
        e.kbResist    = L4BOSS_KNOCKBACK_RESIST;
        e.windupTicks = L4BOSS_WINDUP_TICKS;
        e.cooldownTicks = L4BOSS_ATTACK_COOLDOWN;
        e.rageSpeed   = L4BOSS_RAGE_SPEED;
    }
    else if (type == ENEMY_SELLOID)
    {
        e.speed       = SELLOID_SPEED;
        e.maxHealth   = SELLOID_MAX_HEALTH;
        e.damage      = SELLOID_DAMAGE;
        e.touchRange  = SELLOID_TOUCH_RANGE;
        e.reachHeight = SELLOID_REACH_HEIGHT;
        e.kbResist    = 1.0;
        e.windupTicks = 0;
        e.cooldownTicks = SELLOID_ATTACK_COOLDOWN;
    }
    else if (type == ENEMY_WARSPAN)
    {
        e.speed       = WARSPAN_SPEED;
        e.maxHealth   = WARSPAN_MAX_HEALTH;
        e.damage      = WARSPAN_DAMAGE;
        e.touchRange  = WARSPAN_TOUCH_RANGE;
        e.reachHeight = WARSPAN_REACH_HEIGHT;
        e.kbResist    = 0.80;                  // low and heavy, slides less
        e.windupTicks = 0;
        e.cooldownTicks = WARSPAN_ATTACK_COOLDOWN;
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
void spawnWaveCounts(int smallCount, int bigCount, int warspanCount)
{
    for (int i = 0; i < smallCount; i++)
        enemies.push_back(makeEnemy(ENEMY_SHADOW, offscreenSpawnX(), GROUND_Y));

    for (int i = 0; i < bigCount; i++)
        enemies.push_back(makeEnemy(ENEMY_PRISONER, offscreenSpawnX(), GROUND_Y));

    for (int i = 0; i < warspanCount; i++)
        enemies.push_back(makeEnemy(ENEMY_WARSPAN, offscreenSpawnX(), GROUND_Y));
}

// Clears the arena and spawns row number `index` of a wave table, all at once.
// Kept for anything that wants a whole wave on screen immediately; the layers
// themselves use the queue below instead.
void spawnWaveFrom(const Wave* table, int count, int index)
{
    enemies.clear();
    Wave w = waveAt(table, count, index);
    spawnWaveCounts(w.smallEnemies, w.bigEnemies, w.warspanEnemies);
}


// ===========================================================================
//  FEEDING A WAVE IN, ONE ENEMY AT A TIME
//
//  A wave used to arrive all together. Now the wave's enemies go into a queue
//  and are let in a few at a time: WAVE_ON_SCREEN of them are up at once, and
//  every time one dies the next one in the queue walks in ENEMY_FEED_SECONDS
//  later. Both numbers are in LevelSettings.h.
//
//  The wave is not over until the queue is empty and nothing is left standing,
//  so exactly the same enemies still have to be beaten. They just arrive
//  spaced out instead of mobbing the hero at the start of every wave.
//
//  Order inside a wave is the order the table lists them: the small ones
//  first, then the big ones, then the warspan. So a wave builds up rather
//  than opening with its heaviest enemy.
// ===========================================================================
std::vector<int> waveQueue;     // the EnemyType of everyone still to walk in
int waveFeedTimer = 0;          // steps until the next one is let in

// Clears the arena and loads row `index` of a wave table into the queue.
// Nobody is spawned yet - the first one walks in on the next step.
void queueWaveFrom(const Wave* table, int count, int index)
{
    enemies.clear();
    waveQueue.clear();

    Wave w = waveAt(table, count, index);
    for (int i = 0; i < w.smallEnemies;   i++) waveQueue.push_back((int)ENEMY_SHADOW);
    for (int i = 0; i < w.bigEnemies;     i++) waveQueue.push_back((int)ENEMY_PRISONER);
    for (int i = 0; i < w.warspanEnemies; i++) waveQueue.push_back((int)ENEMY_WARSPAN);

    waveFeedTimer = 0;          // the first of a wave does not keep you waiting
}

// Level 1's waves, straight out of LEVEL1_WAVES in LevelSettings.h.
void queueWave(int waveIndex)
{
    queueWaveFrom(LEVEL1_WAVES, LEVEL1_WAVE_COUNT, waveIndex);
}

// Called once a step while a wave is being fought. Lets the next enemy in when
// there is room for one and the wait has run down.
void updateWaveFeed()
{
    if (waveQueue.empty()) return;

    // no room yet. The wait starts over, so the next one comes in a full
    // ENEMY_FEED_SECONDS after a death, not the moment a slot opens.
    if (aliveEnemyCount() >= WAVE_ON_SCREEN)
    {
        waveFeedTimer = ENEMY_FEED_TICKS;
        return;
    }

    if (waveFeedTimer > 0) { waveFeedTimer--; return; }

    enemies.push_back(makeEnemy((EnemyType)waveQueue[0], offscreenSpawnX(), GROUND_Y));
    waveQueue.erase(waveQueue.begin());
    waveFeedTimer = ENEMY_FEED_TICKS;
}

// How many of this wave have not walked in yet. The layer banners add this to
// the count on screen, so "hostiles remaining" still means what it says.
int waveStillToCome()
{
    return (int)waveQueue.size();
}

// The wave is done when there is nobody left to send and nobody left standing.
bool waveFinished()
{
    return waveQueue.empty() && aliveEnemyCount() == 0;
}

// Used by the layer resets, so a retry never inherits the queue from the run
// that just ended.
void clearWaveQueue()
{
    waveQueue.clear();
    waveFeedTimer = 0;
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

// LEVEL 4: MEAFESTO. He is the only enemy in the layer to begin with.
void spawnLevel4Boss()
{
    enemies.push_back(makeEnemy(ENEMY_L4BOSS, ARENA_RIGHT + 110, GROUND_Y));
}

// The wraiths he calls in at half health, one from each side.
void spawnSelloids(int count)
{
    for (int i = 0; i < count; i++)
    {
        double x = (i % 2 == 0) ? (ARENA_LEFT - 60 - rand() % 90)
                                 : (ARENA_RIGHT + 60 + rand() % 90);
        enemies.push_back(makeEnemy(ENEMY_SELLOID, x, GROUND_Y));
    }
}

// MEAFESTO, wherever he is in the list. 0 if he is not in the fight.
inline Enemy* findLevel4Boss()
{
    for (size_t i = 0; i < enemies.size(); i++)
        if (enemies[i].type == ENEMY_L4BOSS) return &enemies[i];
    return 0;
}

// Turns him into his second form: new art, new bar, faster and harder.
inline void beginSecondLife(Enemy& e)
{
    e.secondLife = true;
    e.maxHealth  = L4BOSS_LIFE2_HEALTH;
    e.health     = L4BOSS_LIFE2_HEALTH;   // a full bar again
    e.speed      = L4BOSS_SPEED_LIFE2;
    e.damage     = L4BOSS_DAMAGE_LIFE2;
    e.enraged    = false;
    e.alive      = true;
    e.knockback  = 0.0;
    e.windupTimer = 0;
    e.attackAnimTimer = 0;
    e.hitFlashTimer   = 0;
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
    double w   = GRUNT_BAR_W;           // ENEMY_BAR_SCALE in GameUtility.h
    double bh  = GRUNT_BAR_H;
    double y   = e.y + h + 8.0;
    double pct = clampd(e.health / e.maxHealth, 0.0, 1.0);

    iSetColor(30, 30, 30);
    iFilledRectangle(e.x - w / 2.0, y, w, bh);
    iSetColor(190, 40, 40);
    iFilledRectangle(e.x - w / 2.0, y, w * pct, bh);
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
//  WHERE A SPEECH PANEL GOES
//
//  The conversation system in GameUtility.h knows who is talking but not where
//  they are standing, because it cannot see the player or the enemy list. This
//  is the piece that joins the two: it turns a speaker into the middle of that
//  character's head, so the panel follows whoever is on screen instead of
//  sitting in a fixed place.
//
//  If the speaker is not in the fight (the boss has not walked in yet, say) the
//  panel falls back to the middle of the arena rather than vanishing.
// ---------------------------------------------------------------------------
inline void dialogueAnchor(int speaker, double& ax, double& ay)
{
    if (speaker == SPK_KARL)
    {
        ax = player.x;
        ay = player.y + PLAYER_DRAW_H;
        return;
    }

    EnemyType want = (speaker == SPK_WARDEN) ? ENEMY_BRUTE : ENEMY_L4BOSS;
    for (size_t i = 0; i < enemies.size(); i++)
    {
        if (enemies[i].type != want) continue;
        ax = enemies[i].x;
        ay = enemies[i].y + enemyDrawHeight(enemies[i]);
        return;
    }

    ax = SCREEN_WIDTH / 2.0;
    ay = GROUND_Y + 180.0;
}

inline void drawDialogue()
{
    if (!dialogueActive()) return;

    const DialogueLine& line = dlgScript[dlgIndex];

    // each speaker's colour, used by the fallback panel and the prompt
    const char* name = "KARL";
    double nr = 212, ng = 60, nb = 48;            // Karl, the hero's red
    const TalkBox* box = &talkKarl;
    if (line.speaker == SPK_WARDEN)
    { name = "THE WARDEN"; nr = 168; ng = 70;  nb = 220; box = &talkWarden;   }
    else if (line.speaker == SPK_MEAFESTO)
    { name = "MEAFESTO";   nr = 240; ng = 130; nb = 40;  box = &talkMeafesto; }

    double ax = 0.0, ay = 0.0;
    dialogueAnchor(line.speaker, ax, ay);

    double bx = 0.0, by = 0.0;

    if (!drawDialogueBox(*box, line.text, dlgChars, ax, ay, bx, by))
    {
        // no artwork: fall back to the plain panel above the speaker's head
        drawDialogueBubble(ax, ay, name, line.text, dlgChars, nr, ng, nb);
    }

    // The prompt lives down on the walking path, the same place every line.
    // The darkening runs off the bottom edge of the screen instead of being a
    // band across the middle of it.
    fillRectGradient(0, 0, SCREEN_WIDTH, DLG_PROMPT_Y + 26.0,
                      3, 2, 4, 0.72, 3, 2, 4, 0.0);

    double pulse = 0.5 + 0.5 * sin(uiTick * 0.09);
    char prompt[96];
    sprintf(prompt, "%d / %d    CLICK OR ENTER    --    ESC TO SKIP",
             dlgIndex + 1, dlgCount);
    drawStatusText(SCREEN_WIDTH / 2.0, DLG_PROMPT_Y, prompt, DLG_PROMPT_SIZE,
                    150 + 80 * pulse, 140 + 66 * pulse, 124 + 48 * pulse);
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


// ---------------------------------------------------------------------------
//  THE SPECIAL MOVE  (H)
//
//  The kick only reaches what is in front of you. This one goes off around the
//  hero, so everything within SPECIAL_RANGE on EITHER side takes the hit, and
//  it hits nearly three times as hard. That is what makes it worth a three
//  second cooldown, and why it is only unlocked from layer 2 onwards.
//
//  It also breaks a boss's telegraphed swing, which the kick deliberately does
//  not - it is the hero's answer to being cornered.
// ---------------------------------------------------------------------------
void performPlayerSpecial()
{
    bool landed = false;

    for (size_t i = 0; i < enemies.size(); i++)
    {
        Enemy& e = enemies[i];
        if (!e.alive) continue;

        double dx = e.x - player.x;
        double dy = fabs(e.y - player.y);

        if (fabs(dx) > SPECIAL_RANGE || dy > SPECIAL_VSPAN) continue;

        e.health -= SPECIAL_DAMAGE;
        e.hitFlashTimer = HIT_FLASH_TICKS * 2;

        // thrown away from the hero, whichever side it was standing on
        double away = (dx >= 0.0) ? 1.0 : -1.0;
        e.knockback = away * SPECIAL_KNOCKBACK * e.kbResist;

        // the blast interrupts anything, boss swings included
        e.windupTimer     = 0;
        e.attackAnimTimer = 0;
        e.attackCooldown  = e.cooldownTicks;

        landed = true;
        lastHitX = e.x;
        lastHitY = e.y + enemyDrawHeight(e) * 0.45;

        if (e.health <= 0)
        {
            e.health = 0;
            e.alive = false;
            killCount++;
        }
    }

    if (landed)
    {
        hitPauseTimer = HIT_PAUSE_TICKS * 2;   // a heavier hit stops the world longer
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

        // ---- keep them ON the path ------------------------------------------
        // Enemies walk in from off screen, so until one has set foot inside the
        // arena it is allowed to be outside it. From the moment it does, it is
        // held inside the same strip of floor the hero is held in. Without this
        // the separation shove and the knockback pushed them out to x -100 and
        // x 1100, standing in mid air off the end of the path where the hero
        // could never reach them.
        if (!e.entered && e.x >= ARENA_LEFT && e.x <= ARENA_RIGHT)
            e.entered = true;

        if (e.entered)
        {
            double margin = enemyDrawWidth(e) * 0.25;
            if (margin > 90.0) margin = 90.0;
            e.x = clampd(e.x, ARENA_LEFT + margin, ARENA_RIGHT - margin);
        }
        else
        {
            e.x = clampd(e.x, ARENA_LEFT - 160, ARENA_RIGHT + 160);
        }

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


// ===========================================================================
//  MAKING THE PICTURE MOVE SMOOTHLY
//
//  The game THINKS on a fixed clock - 62.5 steps a second - because all the
//  speeds, cooldowns and animation lengths are counted in steps, and that has
//  to stay exactly the same however fast or slow the computer is. But it DRAWS
//  as often as the screen will take, which is usually more often than that.
//
//  Without help, a drawn frame can only ever show the last completed step, so
//  the same position gets shown two or three times in a row and then jumps.
//  That is what reads as juddery movement even at a high frame rate.
//
//  The fix is to remember where everybody was one step ago and, when drawing,
//  put them part of the way between there and where they are now - as far as
//  the clock says the next step is due. Nothing in the game logic sees this:
//  the real positions are put straight back the moment the frame is finished.
//
//  Anything that MOVED by more than a step's worth is snapped rather than slid,
//  which is what keeps a freshly spawned enemy, a hero put back at the start of
//  a retry, and Meafesto being placed for his transformation from streaking
//  across the screen.
// ===========================================================================
double interpAlpha = 0.0;        // 0 = on the last step, 1 = on the next one

static double              ipSaveX, ipSaveY;
static std::vector<double> ipSaveEX, ipSaveEY;
static bool                ipApplied = false;

// Called once at the top of every game step, before anything has moved.
inline void snapshotActors()
{
    player.px = player.x;
    player.py = player.y;
    for (size_t i = 0; i < enemies.size(); i++)
    {
        enemies[i].px = enemies[i].x;
        enemies[i].py = enemies[i].y;
    }
}

// The furthest anything can legitimately travel in one step. Past this it was
// placed rather than moved, so it is snapped instead of slid.
const double IP_MAX_STEP = 40.0;

inline double ipMix(double prev, double cur, double a)
{
    double d = cur - prev;
    if (d > IP_MAX_STEP || d < -IP_MAX_STEP) return cur;
    return prev + d * a;
}

inline void applyRenderInterp()
{
    if (ipApplied) return;
    double a = interpAlpha;
    if (a < 0.0) a = 0.0;
    if (a > 1.0) a = 1.0;

    ipSaveX = player.x;  ipSaveY = player.y;
    player.x = ipMix(player.px, player.x, a);
    player.y = ipMix(player.py, player.y, a);

    ipSaveEX.resize(enemies.size());
    ipSaveEY.resize(enemies.size());
    for (size_t i = 0; i < enemies.size(); i++)
    {
        ipSaveEX[i] = enemies[i].x;
        ipSaveEY[i] = enemies[i].y;
        enemies[i].x = ipMix(enemies[i].px, enemies[i].x, a);
        enemies[i].y = ipMix(enemies[i].py, enemies[i].y, a);
    }
    ipApplied = true;
}

inline void restoreActors()
{
    if (!ipApplied) return;
    player.x = ipSaveX;  player.y = ipSaveY;
    for (size_t i = 0; i < enemies.size() && i < ipSaveEX.size(); i++)
    {
        enemies[i].x = ipSaveEX[i];
        enemies[i].y = ipSaveEY[i];
    }
    ipApplied = false;
}

#endif