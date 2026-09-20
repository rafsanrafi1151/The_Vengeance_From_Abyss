
#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include "GameUtility.h"
#include "Audio.h"        // sfxPlay for the hit sounds

// ---------------------------------------------------------------------------
//  PLAYER
//  x  = horizontal centre of the body
//  y  = the FEET line (so y == GROUND_Y means standing on the floor)
// ---------------------------------------------------------------------------
struct Player
{
    double x, y;
    double vx, vy;

    double health;
    bool   facingRight;

    bool   onGround;
    int    coyoteTimer;        // grace ticks to still allow a ground jump
    int    jumpBufferTimer;    // jump pressed slightly too early is remembered
    int    airJumpsLeft;       // double jump budget
    int    landTimer;          // landing squash animation
    int    hitStunTimer;       // reduced control right after taking a hit

    int    attackCooldown;
    int    attackAnimTimer;
    int    hitFlashTimer;

    int    specialCooldown;    // H is locked out until this runs down
    int    specialAnimTimer;   // counts down across the whole special move
    bool   specialHitDone;     // the blow inside the move only lands once

    double walkPhase;      // advances with distance travelled, drives the walk

    // Where he was one step ago. The game thinks 62 times a second but draws
    // far more often than that, and these let a drawn frame sit BETWEEN two
    // steps instead of snapping from one to the next. See snapshotActors().
    double px, py;
};

Player player;

Sprite     playerArt       = emptySprite();   // standing / running
SpriteAnim playerAttackAnim = emptyAnim();     // the swing, played on K
SpriteAnim playerSpecialAnim = emptyAnim();    // the special move, played on H
DirAnim    playerWalk       = emptyDirAnim();  // the stride, one sheet per direction

bool kickKeyWasDown    = false;   // K  -> kick
bool jumpKeyWasDown    = false;   // SPACE / W / Up -> jump
bool specialKeyWasDown = false;   // H  -> the special move

// Is the special move available right now? It is unlocked from
// SPECIAL_MOVE_FROM_LEVEL (LevelSettings.h) onwards, which is layer 2.
inline bool specialUnlocked()
{
    return currentLevel >= SPECIAL_MOVE_FROM_LEVEL;
}

void loadPlayerImages()
{
    // TO CHANGE THE PLAYER IMAGE: put your PNG in Images/CharacterImages/
    // and use one of these names, or add your own name to the list.
    playerArt = loadSpriteAny("Images/CharacterImages/RealPlayer.png",
                               "Images/CharacterImages/Player.png");

    // TO CHANGE THE ATTACK ANIMATION: replace playerImageAnimation.png with a
    // sheet whose frames sit side by side in ONE row, all the same size, then
    // update the five numbers below (frames, aspect, heightScale, anchorX,
    // anchorY) to match how that sheet is cut.
    playerAttackAnim = loadAnim("Images/CharacterImages/playerImageAnimation.png",
                                 4,        // frames
                                 1.2625,   // cell width / height
                                 1.4800,   // cell height vs the character height
                                 0.3211,   // body centre across the cell
                                 0.1931);  // feet up from the cell bottom

    // THE SPECIAL MOVE (H).  SpecialMovePlayer.png, one row of equal cells.
    // How many cells is PLAYER_SPECIAL_FRAMES in LevelSettings.h - change that
    // one number if the move ever shows two bodies at once.
    //
    // loadAnimCutout, not loadAnim: if this sheet still has its background
    // baked in it is ignored rather than drawn as a black block, and the kick
    // animation below is used for the move instead. Cut the background out of
    // the PNG and the real artwork takes over with no code change.
    playerSpecialAnim = loadAnimCutout("Images/CharacterImages/SpecialMovePlayer.png",
                                        PLAYER_SPECIAL_FRAMES,
                                        1.2778,   // cell width / height
                                        1.4000,   // the swirl is wider than the hero
                                        0.4993,   // body centre across the cell
                                        0.0500,   // he is inside the swirl, not under it
                                        false);   // artwork faces RIGHT
    if (playerSpecialAnim.tex == 0)
        playerSpecialAnim = loadAnimCutout("Images/Enime/PlayerSpecialMovement.png",
                                            PLAYER_SPECIAL_FRAMES,
                                            1.2778, 1.4000, 0.4993, 0.0500, false);
    // last resort: reuse the kick sheet so the move always shows something
    if (playerSpecialAnim.tex == 0) playerSpecialAnim = playerAttackAnim;

    // Walk cycle. Same sheet rules: one row, equal cells.
    // Walk cycles, one sheet per direction. Both sheets happen to be DRAWN
    // facing left, so both are flagged facesLeft and the right-hand one is
    // mirrored automatically when he walks right.
    playerWalk.left  = loadAnim("Images/CharacterImages/walking animationLeftPlayer.png",
                                 3,        // frames
                                 0.5883,   // cell width / height
                                 1.0000,   // cell height vs the character height
                                 0.4989,   // body centre across the cell
                                 0.0075,   // feet up from the cell bottom
                                 true);    // artwork faces LEFT
    if (playerWalk.left.tex == 0)
        playerWalk.left = loadAnim("Images/CharacterImages/walkinganimationLeftPlayer.png",
                                    3, 0.5883, 1.0000, 0.4989, 0.0075, true);

    // RIGHT-facing walk: walkAnimationPlayer.png, three frames, genuinely
    // drawn facing right so nothing is mirrored.
    playerWalk.right = loadAnim("Images/CharacterImages/walkAnimationPlayer.png",
                                 3,        // frames
                                 0.6390,   // cell width / height
                                 1.0185,   // cell height vs the character height
                                 0.4990,   // body centre across the cell
                                 0.0000,   // feet on the cell floor
                                 false);   // artwork faces RIGHT
    if (playerWalk.right.tex == 0)
        playerWalk.right = loadAnim("Images/CharacterImages/walkinganimationRightPlayer.png",
                                     3, 0.6632, 1.0000, 0.5136, 0.0077, true);
    if (playerWalk.right.tex == 0)
        playerWalk.right = playerWalk.left;
    if (playerWalk.left.tex == 0)
        playerWalk.left  = playerWalk.right;
}

void resetPlayer()
{
    player.x = (ARENA_LEFT + ARENA_RIGHT) / 2.0;
    player.y = GROUND_Y;
    player.vx = 0.0;
    player.vy = 0.0;

    player.health = PLAYER_MAX_HEALTH;
    player.facingRight = true;

    player.onGround        = true;
    player.coyoteTimer     = 0;
    player.jumpBufferTimer = 0;
    player.airJumpsLeft    = MAX_AIR_JUMPS;
    player.landTimer       = 0;
    player.hitStunTimer    = 0;

    player.attackCooldown  = 0;
    player.attackAnimTimer = 0;
    player.hitFlashTimer   = 0;
    player.specialCooldown  = 0;
    player.specialAnimTimer = 0;
    player.specialHitDone   = false;
    player.walkPhase       = 0.0;

    jumpKeyWasDown    = false;
    kickKeyWasDown    = false;
    specialKeyWasDown = false;
}


// ---------------------------------------------------------------------------
//  Kept for the enemies (and as a fallback if the player image fails to load)
// ---------------------------------------------------------------------------
void drawHumanoidSilhouette(double cx, double baseY, double scale,
                             bool facingRight, bool armRaised,
                             double eyeR, double eyeG, double eyeB,
                             bool flashWhite)
{
    double dir = facingRight ? 1.0 : -1.0;

    double legH   = 34 * scale;
    double torsoH = 30 * scale;
    double torsoW = 22 * scale;
    double headR  = 10 * scale;

    double hipY     = baseY + legH;
    double shoulderY = hipY + torsoH;
    double headY    = shoulderY + headR + 2 * scale;

    if (flashWhite) iSetColor(235, 235, 235);
    else            iSetColor(8, 8, 8);


    double capeX[4] = { cx - 4 * dir, cx - 4 * dir, cx - 20 * dir * scale, cx - 4 * dir };
    double capeY[4] = { shoulderY + 4 * scale, hipY - 4 * scale, baseY, baseY + 6 * scale };
    iFilledPolygon(capeX, capeY, 4);


    iFilledRectangle(cx - 9 * scale, baseY, 7 * scale, legH);
    iFilledRectangle(cx + 2 * scale, baseY, 7 * scale, legH);


    double tx[4] = { cx - torsoW / 2, cx + torsoW / 2, cx + torsoW / 2.6, cx - torsoW / 2.6 };
    double ty[4] = { hipY, hipY, shoulderY, shoulderY };
    iFilledPolygon(tx, ty, 4);


    iFilledRectangle(cx - 10 * scale * dir - 3 * scale, shoulderY - 26 * scale, 6 * scale, 22 * scale);


    if (armRaised)
    {
        iRotate(cx + 6 * dir * scale, shoulderY, facingRight ? -55 : 55);
        iFilledRectangle(cx + 4 * scale * dir, shoulderY - 6 * scale, 26 * scale * dir, 7 * scale);
        iUnRotate();
    }
    else
    {
        iFilledRectangle(cx + 4 * scale * dir - (dir > 0 ? 0 : 6 * scale), shoulderY - 24 * scale, 6 * scale, 20 * scale);
    }


    iFilledCircle(cx, headY, headR, 24);


    if (!flashWhite)
    {
        iSetColor(eyeR, eyeG, eyeB);
        iFilledCircle(cx + 3 * dir * scale, headY, 1.6 * scale, 10);
    }
}


// ---------------------------------------------------------------------------
//  DRAWING
// ---------------------------------------------------------------------------
void drawPlayerShadow()
{
    // The shadow stays on the floor and shrinks the higher the player is,
    // which is what actually sells the height of a jump.
    double height = player.y - GROUND_Y;
    double t      = clampd(1.0 - height / 170.0, 0.25, 1.0);

    iSetColor(10 * t, 10 * t, 10 * t);
    iFilledEllipse(player.x, GROUND_Y + 3, 26 * t, 7 * t, 20);
}

void drawKickArc()
{
    if (player.attackAnimTimer <= 0) return;

    double dir  = player.facingRight ? 1.0 : -1.0;
    double life = (double)player.attackAnimTimer / (double)PLAYER_ATTACK_ANIM_LEN; // 1 -> 0

    double ox   = player.x + dir * 10.0;
    double oy   = player.y + PLAYER_DRAW_H * 0.34;   // hip height - it is a kick

    // the crescent sweeps outward and thins as the swing finishes
    double rOut = PLAYER_ATTACK_RANGE * (0.70 + 0.30 * (1.0 - life));
    double rIn  = rOut - (10.0 + 14.0 * life);
    if (rIn < 8.0) rIn = 8.0;

    // swing from high to low across the front of the body
    double sweep = 55.0;
    double mid   = -38.0 + 52.0 * (1.0 - life);   // degrees, 0 = straight ahead (low -> forward)
    double a0    = (mid - sweep * 0.5) * 3.14159265 / 180.0;
    double a1    = (mid + sweep * 0.5) * 3.14159265 / 180.0;

    const int STEPS = 8;
    double px[2 * (STEPS + 1)];
    double py[2 * (STEPS + 1)];

    for (int i = 0; i <= STEPS; i++)
    {
        double a = a0 + (a1 - a0) * ((double)i / (double)STEPS);
        px[i] = ox + dir * rOut * cos(a);
        py[i] = oy + rOut * sin(a);
    }
    for (int i = 0; i <= STEPS; i++)
    {
        double a = a1 - (a1 - a0) * ((double)i / (double)STEPS);
        px[STEPS + 1 + i] = ox + dir * rIn * cos(a);
        py[STEPS + 1 + i] = oy + rIn * sin(a);
    }

    iSetColor(240, 230, 205);
    iFilledPolygon(px, py, 2 * (STEPS + 1));
}

// ---------------------------------------------------------------------------
//  THE SPECIAL MOVE'S SHOCKWAVE
//
//  A pair of rings that race out to SPECIAL_RANGE on both sides and fade, so
//  the reach of the move is something you can see rather than guess. This is
//  drawn whether or not the sheet loaded, which is what makes the move read
//  even while SpecialMovePlayer.png is still waiting to be cut out.
// ---------------------------------------------------------------------------
void drawSpecialShockwave()
{
    if (player.specialAnimTimer <= 0) return;

    double done = (double)(SPECIAL_ANIM_LEN - player.specialAnimTimer)
                   / (double)SPECIAL_ANIM_LEN;               // 0 -> 1
    double cy   = player.y + PLAYER_DRAW_H * 0.30;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int ring = 0; ring < 3; ring++)
    {
        double t = done - ring * 0.13;
        if (t <= 0.0 || t >= 1.0) continue;

        double r     = SPECIAL_RANGE * t;
        double fade  = (1.0 - t) * 0.85;

        glColor4d(1.0, 0.72 - 0.25 * t, 0.30, fade);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 30; i++)
        {
            double a = i * 2.0 * 3.14159265 / 30.0;
            glVertex2d(player.x + r * cos(a), cy + r * 0.42 * sin(a));
        }
        glEnd();
    }

    // There used to be a white bar drawn straight across the screen at the
    // moment the blow landed. It read as a glitch rather than an impact, so it
    // is gone; the rings above already show the reach and the timing.

    glColor4d(1, 1, 1, 1);
    glDisable(GL_BLEND);
}

void drawPlayer()
{
    drawPlayerShadow();
    drawSpecialShockwave();

    // --- squash and stretch -------------------------------------------------
    // stretched while rising / falling fast, squashed for a few ticks on landing
    double stretch = 1.0;
    if (!player.onGround)
        stretch = 1.0 + clampd(fabs(player.vy) * 0.012, 0.0, 0.16);
    else if (player.landTimer > 0)
        stretch = 1.0 - 0.20 * ((double)player.landTimer / (double)LAND_SQUASH_TICKS);

    double h = PLAYER_DRAW_H * stretch;

    // --- damage tint --------------------------------------------------------
    double tr = 255, tg = 255, tb = 255;
    if (player.hitFlashTimer > 0 && (player.hitFlashTimer / 2) % 2 == 0)
    {
        tr = 255; tg = 110; tb = 110;
    }

    bool special   = player.specialAnimTimer > 0 && playerSpecialAnim.tex != 0;
    bool attacking = !special && player.attackAnimTimer > 0 && playerAttackAnim.tex != 0;
    const SpriteAnim& stride = dirAnimFor(playerWalk, player.facingRight);
    bool walking   = !special && !attacking && player.onGround
                      && fabs(player.vx) > 0.35 && stride.tex != 0;

    if (special)
    {
        // the special move owns the whole body while it lasts, and the hero
        // swells a little so the blow has some weight behind it
        int done  = SPECIAL_ANIM_LEN - player.specialAnimTimer;
        int frame = done * playerSpecialAnim.count / SPECIAL_ANIM_LEN;

        double swell = 1.0 + (SPECIAL_DRAW_SCALE - 1.0)
                        * sin(3.14159265 * (double)done / (double)SPECIAL_ANIM_LEN);

        drawAnimFrame(playerSpecialAnim, frame, player.x, player.y,
                       PLAYER_DRAW_H * swell,
                       animFlipFor(playerSpecialAnim, player.facingRight),
                       tr, tg * 0.92, tb * 0.80, 255);
    }
    else if (attacking)
    {
        // walk through the sheet across the length of the swing
        int done  = PLAYER_ATTACK_ANIM_LEN - player.attackAnimTimer;   // 0 -> len
        int frame = done * playerAttackAnim.count / PLAYER_ATTACK_ANIM_LEN;

        drawAnimFrame(playerAttackAnim, frame, player.x, player.y, PLAYER_DRAW_H,
                       animFlipFor(playerAttackAnim, player.facingRight),
                       tr, tg, tb, 255);
    }
    else if (walking)
    {
        int frame = (int)(player.walkPhase / WALK_CYCLE_PX) % stride.count;

        // heroWalk.png only holds two poses and they are close together, so on
        // their own the swap is barely visible. A small rise-and-settle driven
        // by the same phase makes the stride read as actual walking.
        double cycle = player.walkPhase / (WALK_CYCLE_PX * stride.count);
        double step  = sin(cycle * 2.0 * 3.14159265);      // -1 .. 1
        double bob   = WALK_BOB_PX * fabs(step);
        double squash = 1.0 - WALK_SQUASH * (1.0 - fabs(step));

        drawAnimFrame(stride, frame, player.x, player.y + bob, h * squash,
                       animFlipFor(stride, player.facingRight),
                       tr, tg, tb, 255);
    }
    else if (playerArt.tex != 0)
    {
        // width and the crop come from the image itself - nothing to tune
        drawSpriteArt(playerArt, player.x, player.y, h,
                       spriteFlipFor(playerArt, player.facingRight),
                       tr, tg, tb, 255);
    }
    else
    {
        // fallback so the game is still playable if the PNG is missing
        bool flash = player.hitFlashTimer > 0 && (player.hitFlashTimer / 2) % 2 == 0;
        drawHumanoidSilhouette(player.x, player.y, 1.0, player.facingRight,
                                player.attackAnimTimer > 0, 180, 40, 40, flash);
    }

    if (!attacking && !special) drawKickArc();
}


// ---------------------------------------------------------------------------
//  THE SPECIAL MOVE CUE IN THE HUD
//
//  Sits on its own line under the kill count, right hand side, so it cannot
//  run into the layer title in the middle or the health bar on the left.
//  Only drawn in the layers where the move is actually unlocked.
// ---------------------------------------------------------------------------
inline void drawSpecialCue()
{
    if (!specialUnlocked()) return;

    // The bar reads as a CHARGE, not a countdown: it empties the moment the
    // move fires and fills back up over SPECIAL_COOLDOWN, so a full bar means
    // the move is ready.
    double charge = 1.0;
    if (player.specialCooldown > 0)
        charge = 1.0 - (double)player.specialCooldown / (double)SPECIAL_COOLDOWN;
    charge = clampd(charge, 0.0, 1.0);

    if (drawImageBar(specialBarArt, SPECIAL_BAR_X, SPECIAL_BAR_Y,
                      SPECIAL_BAR_W, SPECIAL_BAR_H, charge))
    {
        // once it is full the frame breathes, so "ready" is visible at a glance
        if (player.specialCooldown <= 0)
        {
            double pulse = 0.5 + 0.5 * sin(uiTick * 0.11);
            glowRect(SPECIAL_BAR_X + SPECIAL_BAR_W * 0.20, SPECIAL_BAR_Y + 6,
                      SPECIAL_BAR_W * 0.70, SPECIAL_BAR_H - 12,
                      215, 55, 40, 0.10 + 0.20 * pulse, 7);
        }

        // the words, written under the bar at a size that can be read
        if (gameFontTex != 0)
        {
            bool ready = (player.specialCooldown <= 0);
            drawGameText(SPECIAL_LABEL_X + 1, SPECIAL_LABEL_Y - 1,
                          "H   SPECIAL MOVE", SPECIAL_LABEL_SIZE, 0, 0, 0, 190);
            if (ready)
                drawGameText(SPECIAL_LABEL_X, SPECIAL_LABEL_Y,
                              "H   SPECIAL MOVE", SPECIAL_LABEL_SIZE, 248, 226, 170);
            else
                drawGameText(SPECIAL_LABEL_X, SPECIAL_LABEL_Y,
                              "H   SPECIAL MOVE", SPECIAL_LABEL_SIZE, 138, 128, 118);
        }
        return;
    }

    // ---- no artwork: fall back to the old line of writing ---------------
    if (gameFontTex == 0) return;
    if (player.specialCooldown <= 0)
    {
        double pulse = 0.5 + 0.5 * sin(uiTick * 0.12);
        drawGameText(SPECIAL_LABEL_X, SPECIAL_LABEL_Y, "H   SPECIAL MOVE",
                      SPECIAL_LABEL_SIZE,
                      220 + 35 * pulse, 170 + 50 * pulse, 90);
    }
    else
    {
        char cue[40];
        sprintf(cue, "H   SPECIAL MOVE  %d%%", (int)(100.0 * charge));
        drawGameText(SPECIAL_LABEL_X, SPECIAL_LABEL_Y, cue,
                      SPECIAL_LABEL_SIZE, 128, 120, 112);
    }
}

// old name kept so nothing else breaks
void drawPlayerSilhouette() { drawPlayer(); }


// ---------------------------------------------------------------------------
//  DAMAGE (called from Enemy.h)
// ---------------------------------------------------------------------------
void damagePlayer(double amount, double fromX)
{
    player.health -= amount;
    player.hitFlashTimer = HIT_FLASH_TICKS;
    player.hitStunTimer  = PLAYER_HITSTUN;

    // an enemy connected with the hero. Same sound in every level.
    sfxPlay(SFX_ENEMY_HIT);

    double away = (player.x >= fromX) ? 1.0 : -1.0;
    player.vx = away * PLAYER_KNOCKBACK_X;
    if (player.onGround)
    {
        player.vy = PLAYER_KNOCKBACK_Y;
        player.onGround = false;
    }

    if (player.health <= 0)
    {
        player.health = 0;
        gameState = STATE_GAMEOVER;
    }
}


// ---------------------------------------------------------------------------
//  CONTROLLER
// ---------------------------------------------------------------------------
void updatePlayerInput()
{
    // ---- raw input ---------------------------------------------------------
    bool leftHeld  = (isKeyPressed('a') != 0) || (isKeyPressed('A') != 0) || (isSpecialKeyPressed(GLUT_KEY_LEFT)  != 0);
    bool rightHeld = (isKeyPressed('d') != 0) || (isKeyPressed('D') != 0) || (isSpecialKeyPressed(GLUT_KEY_RIGHT) != 0);
    bool downHeld  = (isKeyPressed('s') != 0) || (isKeyPressed('S') != 0) || (isSpecialKeyPressed(GLUT_KEY_DOWN)  != 0);

    // SPACE is the jump button; W / Up still work as an alternative.
    bool jumpHeld  = (isKeyPressed(' ') != 0) || (isKeyPressed('w') != 0) || (isKeyPressed('W') != 0)
                     || (isSpecialKeyPressed(GLUT_KEY_UP) != 0);

    // K is the kick button.
    bool kickHeld  = (isKeyPressed('k') != 0) || (isKeyPressed('K') != 0);

    // H is the special move.
    bool specialHeld = (isKeyPressed('h') != 0) || (isKeyPressed('H') != 0);

    bool jumpPressed  =  jumpHeld && !jumpKeyWasDown;
    bool jumpReleased = !jumpHeld &&  jumpKeyWasDown;
    jumpKeyWasDown = jumpHeld;

    // ---- timers ------------------------------------------------------------
    if (player.attackCooldown > 0)  player.attackCooldown--;
    if (player.attackAnimTimer > 0) player.attackAnimTimer--;
    if (player.specialCooldown > 0) player.specialCooldown--;
    if (player.hitFlashTimer > 0)   player.hitFlashTimer--;
    if (player.landTimer > 0)       player.landTimer--;
    if (player.hitStunTimer > 0)    player.hitStunTimer--;
    if (player.coyoteTimer > 0)     player.coyoteTimer--;
    if (player.jumpBufferTimer > 0) player.jumpBufferTimer--;

    if (jumpPressed) player.jumpBufferTimer = JUMP_BUFFER_TICKS;

    // ---- horizontal --------------------------------------------------------
    double dirInput = 0.0;
    if (leftHeld)  dirInput -= 1.0;
    if (rightHeld) dirInput += 1.0;

    // during hitstun the knockback wins, the stick barely matters
    double control = (player.hitStunTimer > 0) ? 0.25 : 1.0;

    if (dirInput != 0.0)
    {
        double accel = (player.onGround ? PLAYER_RUN_ACCEL : PLAYER_AIR_ACCEL) * control;

        // turning around snaps much faster than accelerating from a standstill
        if (signd(player.vx) != 0.0 && signd(player.vx) != dirInput)
            accel *= PLAYER_TURN_BOOST;

        player.vx += dirInput * accel;

        if (player.vx >  PLAYER_MAX_SPEED) player.vx =  PLAYER_MAX_SPEED;
        if (player.vx < -PLAYER_MAX_SPEED) player.vx = -PLAYER_MAX_SPEED;

        if (player.hitStunTimer <= 0)
            player.facingRight = (dirInput > 0);
    }
    else
    {
        player.vx *= player.onGround ? PLAYER_GROUND_FRICTION : PLAYER_AIR_FRICTION;
        if (fabs(player.vx) < PLAYER_VEL_EPSILON) player.vx = 0.0;
    }

    player.x += player.vx;

    // the walk cycle is driven by distance travelled, so it never moonwalks
    if (player.onGround) player.walkPhase += fabs(player.vx);
    else                 player.walkPhase  = 0.0;

    // ---- jump --------------------------------------------------------------
    if (player.jumpBufferTimer > 0 && player.hitStunTimer <= 0)
    {
        if (player.onGround || player.coyoteTimer > 0)
        {
            player.vy              = JUMP_VELOCITY;
            player.onGround        = false;
            player.coyoteTimer     = 0;
            player.jumpBufferTimer = 0;
            player.landTimer       = 0;
        }
        else if (player.airJumpsLeft > 0)
        {
            player.vy              = AIR_JUMP_VELOCITY;
            player.airJumpsLeft--;
            player.jumpBufferTimer = 0;

            // a double jump re-commits to the direction you are holding
            if (dirInput != 0.0) player.facingRight = (dirInput > 0);
        }
    }

    // variable jump height: let go early and the rise is cut short
    if (jumpReleased && player.vy > 0.0)
        player.vy *= JUMP_CUT_MULT;

    // ---- gravity -----------------------------------------------------------
    double g = GRAVITY;
    if (player.vy < 0.0)              g *= GRAVITY_FALL_MULT;   // fall faster than you rise
    if (downHeld && !player.onGround) g *= FAST_FALL_MULT;      // S / Down = fast fall

    player.vy -= g;
    if (player.vy < -MAX_FALL_SPEED) player.vy = -MAX_FALL_SPEED;

    bool wasOnGround = player.onGround;
    player.y += player.vy;

    // ---- ground / ceiling collision ---------------------------------------
    if (player.y <= GROUND_Y)
    {
        player.y = GROUND_Y;

        if (!wasOnGround)
            player.landTimer = LAND_SQUASH_TICKS;   // landing squash

        player.vy           = 0.0;
        player.onGround     = true;
        player.airJumpsLeft = MAX_AIR_JUMPS;
        player.coyoteTimer  = COYOTE_TICKS;
    }
    else
    {
        if (wasOnGround)
            player.coyoteTimer = COYOTE_TICKS;      // walked off / got knocked up

        player.onGround = false;

        if (player.y > PLAYER_CEILING_Y)
        {
            player.y  = PLAYER_CEILING_Y;
            if (player.vy > 0.0) player.vy = 0.0;
        }
    }

    // ---- arena walls -------------------------------------------------------
    double minX = ARENA_LEFT  + PLAYER_HALF_W;
    double maxX = ARENA_RIGHT - PLAYER_HALF_W;
    if (player.x < minX) { player.x = minX; if (player.vx < 0) player.vx = 0; }
    if (player.x > maxX) { player.x = maxX; if (player.vx > 0) player.vx = 0; }

    // ---- the special move (H) ---------------------------------------------
    // The move runs on its own timer: the animation starts the moment H goes
    // down, and the blow itself lands SPECIAL_HIT_AT ticks later, at the point
    // in the swing where it looks like it should. Locked until layer 2.
    if (player.specialAnimTimer > 0)
    {
        player.specialAnimTimer--;

        int done = SPECIAL_ANIM_LEN - player.specialAnimTimer;
        if (!player.specialHitDone && done >= SPECIAL_HIT_AT)
        {
            player.specialHitDone = true;
            performPlayerSpecial();
        }
    }
    else if (specialHeld && !specialKeyWasDown && specialUnlocked()
              && player.specialCooldown <= 0 && player.hitStunTimer <= 0)
    {
        player.specialCooldown  = SPECIAL_COOLDOWN;
        player.specialAnimTimer = SPECIAL_ANIM_LEN;
        player.specialHitDone   = false;

        // it takes over from a kick that is still finishing
        player.attackAnimTimer  = 0;
    }
    specialKeyWasDown = specialHeld;

    // ---- kick (K) ----------------------------------------------------------
    if (kickHeld && !kickKeyWasDown && player.attackCooldown <= 0
         && player.hitStunTimer <= 0 && player.specialAnimTimer <= 0)
    {
        player.attackCooldown  = PLAYER_ATTACK_COOLDOWN;
        player.attackAnimTimer = PLAYER_ATTACK_ANIM_LEN;
        performPlayerAttack();
    }
    kickKeyWasDown = kickHeld;
}

#endif
