
#pragma once
#ifndef MENU_H
#define MENU_H

#include "GameUtility.h"

Button menuButtons[3] = {
    { (SCREEN_WIDTH - 300) / 2.0, 285, 300, 58, "PLAY", false },
    { (SCREEN_WIDTH - 300) / 2.0, 202, 300, 58, "INSTRUCTIONS", false },
    { (SCREEN_WIDTH - 300) / 2.0, 124, 300, 58, "EXIT", false }
};

Button instructionsButtons[1] = {
    { (SCREEN_WIDTH - 220) / 2.0, 60, 220, 50, "BACK", false }
};

int menuAmbientTick = 0;
extern int bgImage;

void updateMenuAmbient()
{
    menuAmbientTick++;
    uiTick++;              // drives the button glow
}


void drawMenuAmbience()
{
    iSetColor(14, 14, 14);
    for (int i = 0; i < 6; i++)
    {
        double t = menuAmbientTick * 0.01 + i * 1.3;
        double bx = 100 + i * 150 + 40 * sin(t);
        double by = 120 + 60 * cos(t * 0.7 + i);
        iFilledCircle(bx, by, 18 + 6 * sin(t * 1.7), 18);
    }
}

void drawMenu()
{
    iClear();

    iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImage);

    // a gentle darkening behind the buttons so the labels always read
    dimScreen(0.28);
    fillRectGradient(0, 0, SCREEN_WIDTH, 430, 6, 4, 10, 0.62, 6, 4, 10, 0.0);

    double cx = SCREEN_WIDTH / 2.0;

    // ---- title -----------------------------------------------------------
 /*   iSetColor(0, 0, 0);
    drawCenteredText(cx + 3, 517, (char*)"VENGEANCE FROM THE ABYSS",
                      GLUT_BITMAP_TIMES_ROMAN_24, 15);
    iSetColor(206, 42, 34);
    drawCenteredText(cx, 520, (char*)"VENGEANCE FROM THE ABYSS",
                      GLUT_BITMAP_TIMES_ROMAN_24, 15);

    fillRectAlpha(cx - 200, 508, 400, 2, 190, 50, 40, 0.8);

    iSetColor(150, 145, 150);
    drawCenteredText(cx, 484, (char*)"A blood oath sworn in the dark.",
                      GLUT_BITMAP_HELVETICA_18, 9); */

    for (int i = 0; i < 3; i++)
        drawButton(menuButtons[i]);

    drawStatusText(cx, 40, "SPACE jump    K kick    H special    ESC pause",
                    17, 142, 134, 138);
}

void drawInstructions()
{
    iClear();
    iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImage);
    dimScreen(0.72);

    double cx = SCREEN_WIDTH / 2.0;

    // Everything on this screen is written with the game's own alphabet
    // picture (Images/gameFront.png) rather than the plain white lettering
    // that comes with the graphics library, so it matches the rest of the
    // game. drawStatusText falls back to the old lettering only if the
    // picture is missing.
    drawStatusText(cx + 2, 536, "THE ABYSS GAUNTLET", 40, 40, 6, 6);
    drawStatusText(cx,     538, "THE ABYSS GAUNTLET", 40, 206, 44, 38);

    double y = 492;
    const double BODY = 19.0;
    const char* intro[] = {
        "Falsely convicted, you are cast into the Abyss.",
        "Clear Layer 1, then face THE WARDEN in Layer 2.",
        "Layer 3 is the ROT and the GATE HOUND.",
        "Layer 4 is MEAFESTO alone - and he has two lives.",
        "Every layer starts you on full health."
    };
    for (int i = 0; i < 5; i++)
    {
        drawStatusText(cx, y, intro[i], BODY, 208, 194, 176);
        y -= 24;
    }

    y -= 12;
    drawStatusText(cx, y, "CONTROLS", 24, 224, 176, 96);
    y -= 30;

    // Two columns: the keys on the left, what they do on the right. The old
    // screen lined these up with runs of spaces, which only works when every
    // letter is the same width - this alphabet is proportional, so the two
    // halves are drawn at fixed positions instead.
    const char* keys[] = {
        "A / D  or  LEFT / RIGHT",
        "SPACE",
        "SPACE in mid-air",
        "S  or  DOWN",
        "K",
        "H",
        "ESC"
    };
    const char* does[] = {
        "Run (hold to build speed)",
        "Jump (tap = hop, hold = full jump)",
        "Double jump",
        "Fast fall / slam back down",
        "Kick (hits everyone in range)",
        "Special move (from Layer 2 on)",
        "Pause"
    };
    double keyRight = cx - 24.0;
    double descLeft = cx + 24.0;
    for (int i = 0; i < 7; i++)
    {
        if (gameFontTex != 0)
        {
            drawGameTextRight(keyRight, y, keys[i], BODY, 226, 200, 150);
            drawGameText(descLeft, y, does[i], BODY, 196, 184, 168);
        }
        else
        {
            iSetColor(226, 200, 150);
            iText(cx - 250, y, (char*)keys[i], GLUT_BITMAP_HELVETICA_18);
            iSetColor(196, 184, 168);
            iText(descLeft, y, (char*)does[i], GLUT_BITMAP_HELVETICA_18);
        }
        y -= 24;
    }

    y -= 10;
    drawStatusText(cx, y, "Jump over a hostile and it cannot touch you.",
                    BODY, 176, 166, 152);
    y -= 24;
    drawStatusText(cx, y, "The Warden winds up before he swings - that is your window.",
                    BODY, 176, 166, 152);

    drawButton(instructionsButtons[0]);
}

void updateMenuHover(int mx, int my)
{
    for (int i = 0; i < 3; i++)
        menuButtons[i].hover = pointInButton(menuButtons[i], mx, my);
}

void updateInstructionsHover(int mx, int my)
{
    instructionsButtons[0].hover = pointInButton(instructionsButtons[0], mx, my);
}

// ===========================================================================
//
//                    * * *  T H E   S T O R Y  * * *
//
//  PLAY no longer drops you straight into layer 1. It shows the story first,
//  one page at a time, and then the game begins.
//
//  TO ADD A PAGE: drop another picture in Images/GameStory/ named with the
//  next number - 1.png, then 2.png, then 3.png and so on. They are shown in
//  that order. The loader stops at the first number that is missing, so there
//  is nothing to update here; STORY_MAX_PAGES is only an upper limit.
//
//  Any picture shape works. Each page is fitted inside the screen with its
//  proportions kept, so a wide comic page is never squashed to fit.
//
// ===========================================================================
const int STORY_MAX_PAGES = 12;

Sprite storyPages[STORY_MAX_PAGES];
int    storyPageCount = 0;    // how many were actually found
int    storyPage      = 0;    // which one is on screen
int    storyTick      = 0;    // free running, drives the prompt pulse
int    storyFade      = 0;    // counts down at the start of each page
bool   storyKeyWasDown = false;   // edge detection for the page turn key

const int STORY_FADE_TICKS = 18;   // the short fade as a page comes up

// The bottom 52 pixels are kept clear of the artwork for the page counter,
// the prompt and the SKIP button, so nothing is ever written across a panel.
const double STORY_UI_STRIP = 52.0;
const double STORY_SIDE_PAD = 12.0;

Button storyButtons[1] = {
    { SCREEN_WIDTH - 146.0, 6, 128, 40, "SKIP", false }
};

void loadStoryImages()
{
    storyPageCount = 0;
    for (int i = 0; i < STORY_MAX_PAGES; i++)
        storyPages[i] = emptySprite();

    for (int i = 0; i < STORY_MAX_PAGES; i++)
    {
        char path[96];
        sprintf(path, "Images/GameStory/%d.png", i + 1);
        Sprite s = loadSprite(path);

        if (s.tex == 0)
        {
            // try the same number as a .jpg before giving up on it
            sprintf(path, "Images/GameStory/%d.jpg", i + 1);
            s = loadSprite(path);
        }
        if (s.tex == 0) break;       // that number is missing: the story ends here

        storyPages[storyPageCount] = s;
        storyPageCount++;
    }
}

// PLAY calls this. With no pictures in Images/GameStory/ it simply starts the
// game, so a missing folder can never leave you stuck on a blank screen.
void startStory()
{
    if (storyPageCount <= 0)
    {
        startNewGameFromMenu();
        return;
    }

    storyPage = 0;
    storyTick = 0;
    storyFade = STORY_FADE_TICKS;
    // ENTER was just pressed to get here from the name box. Starting with the
    // key marked as already down stops that same press turning the first page.
    storyKeyWasDown = true;
    gameState = STATE_STORY;
}

void storySkip()
{
    startNewGameFromMenu();
}

// Click, ENTER, SPACE or the right arrow: next page, or into the game.
void storyAdvance()
{
    if (storyPage + 1 < storyPageCount)
    {
        storyPage++;
        storyFade = STORY_FADE_TICKS;
    }
    else
    {
        startNewGameFromMenu();
    }
}

void storyBack()
{
    if (storyPage > 0)
    {
        storyPage--;
        storyFade = STORY_FADE_TICKS;
    }
}

void updateStory()
{
    storyTick++;
    uiTick++;                          // keeps the SKIP button breathing
    if (storyFade > 0) storyFade--;
}

void drawStory()
{
    iClear();

    if (storyPage < 0) storyPage = 0;
    if (storyPage >= storyPageCount) storyPage = storyPageCount - 1;

    const Sprite& page = storyPages[storyPage];

    // ---- fit the page in the space above the strip, proportions kept ----
    // A page wider than the box loses height, a taller one loses width, and
    // either way it is centred in the box. Nothing is ever stretched, so a
    // wide comic page and a tall portrait page both come out true.
    double boxX = STORY_SIDE_PAD;
    double boxY = STORY_UI_STRIP;
    double boxW = SCREEN_WIDTH  - STORY_SIDE_PAD * 2.0;
    double boxH = SCREEN_HEIGHT - STORY_UI_STRIP - STORY_SIDE_PAD;

    double pw = boxW;
    double ph = pw / page.aspect;
    if (ph > boxH)
    {
        ph = boxH;
        pw = ph * page.aspect;
    }
    double px = boxX + (boxW - pw) / 2.0;
    double py = boxY + (boxH - ph) / 2.0;

    // the page rises into place over the first few ticks
    double t     = 1.0 - (double)storyFade / (double)STORY_FADE_TICKS;  // 0 -> 1
    double alpha = 255.0 * (0.25 + 0.75 * t);
    double lift  = 10.0 * (1.0 - t);

    drawSpriteUV(page.tex, px, py - lift, pw, ph, false,
                  page.u0, page.v0, page.u1, page.v1,
                  255, 255, 255, alpha);

    // ---- the strip along the bottom -------------------------------------
    fillRectGradient(0, 0, SCREEN_WIDTH, STORY_UI_STRIP + 10,
                      4, 3, 5, 0.92, 4, 3, 5, 0.0);

    // ---- the page counter, bottom left ----------------------------------
    char counter[32];
    sprintf(counter, "%d / %d", storyPage + 1, storyPageCount);
    if (gameFontTex != 0)
        drawGameText(20, 17, counter, 21, 168, 156, 144);
    else
    { iSetColor(170, 160, 150); iText(20, 17, counter, GLUT_BITMAP_8_BY_13); }

    // ---- the prompt, bottom centre --------------------------------------
    double pulse = 0.5 + 0.5 * sin(storyTick * 0.08);
    const char* prompt = (storyPage + 1 < storyPageCount)
                          ? "CLICK OR PRESS ENTER FOR THE NEXT PAGE"
                          : "CLICK OR PRESS ENTER TO BEGIN";
    // (ESC goes back to the menu, SKIP jumps straight into the game)
    if (gameFontTex != 0)
        drawStatusText(SCREEN_WIDTH / 2.0 - 40, 17, prompt, 20,
                        150 + 90 * pulse, 140 + 80 * pulse, 128 + 60 * pulse);
    else
    { iSetColor(150 + 90 * pulse, 140 + 80 * pulse, 130);
      drawCenteredText(SCREEN_WIDTH / 2.0 - 40, 22, const_cast<char*>(prompt),
                        GLUT_BITMAP_HELVETICA_18, 9); }

    drawButton(storyButtons[0]);
}

void updateStoryHover(int mx, int my)
{
    storyButtons[0].hover = pointInButton(storyButtons[0], mx, my);
}

void handleStoryClick(int mx, int my)
{
    if (pointInButton(storyButtons[0], mx, my)) { storySkip(); return; }
    storyAdvance();
}

// ENTER or the right arrow moves on, the left arrow goes back, ESC returns to
// the menu. Deliberately NOT the space bar: space is the jump button, and a
// space still held down as the last page turns would make the hero jump the
// moment layer 1 starts.
//
// Called once per key press from fixedUpdate, never every tick, so one press
// never flips through the whole story. (storyKeyWasDown is declared further
// up, because startStory has to set it.)
void storyKeys()
{
    bool nextHeld = (isKeyPressed(13) != 0)
                     || (isSpecialKeyPressed(GLUT_KEY_RIGHT) != 0);
    bool backHeld = (isSpecialKeyPressed(GLUT_KEY_LEFT) != 0);

    bool held = nextHeld || backHeld;
    if (held && !storyKeyWasDown)
    {
        if (nextHeld) storyAdvance();
        else          storyBack();
    }
    storyKeyWasDown = held;
}


// ===========================================================================
//
//                  * * *  T Y P I N G   Y O U R   N A M E  * * *
//
//  PLAY asks for a name before the story runs. It is kept between runs, so
//  playing again just needs ENTER.
//
//  iGraphics has no "a character was typed" callback - it only keeps a table
//  of which keys are down. So this watches that table and takes a letter the
//  tick it goes down, which is what stops one press filling the box.
// ===========================================================================
bool nameKeyWasDown[256];
int  nameCaretTick = 0;

void startNameEntry()
{
    for (int i = 0; i < 256; i++) nameKeyWasDown[i] = (isKeyPressed(i) != 0);
    nameCaretTick = 0;
    gameState = STATE_NAME_ENTRY;
}

// Everything the alphabet picture can draw, minus the pipe, which is the
// character scores.txt uses to separate its fields.
inline bool nameAllowed(int c)
{
    if (c == '|') return false;
    return c >= 32 && c <= 126;
}

void updateNameEntry()
{
    nameCaretTick++;
    uiTick++;

    for (int c = 0; c < 256; c++)
    {
        bool down = (isKeyPressed(c) != 0);
        bool fresh = down && !nameKeyWasDown[c];
        nameKeyWasDown[c] = down;
        if (!fresh) continue;

        if (c == 8)                                   // backspace
        {
            if (playerNameLen > 0) playerName[--playerNameLen] = 0;
        }
        else if (c == 13)                             // enter
        {
            if (playerNameLen == 0)                   // nothing typed: a default
            {
                const char* fallback = "PLAYER";
                playerNameLen = 0;
                while (fallback[playerNameLen])
                { playerName[playerNameLen] = fallback[playerNameLen]; playerNameLen++; }
                playerName[playerNameLen] = 0;
            }
            startStory();
            return;
        }
        else if (nameAllowed(c) && playerNameLen < PLAYER_NAME_MAX)
        {
            playerName[playerNameLen++] = (char)c;
            playerName[playerNameLen] = 0;
        }
    }
}

void drawNameEntry()
{
    iClear();
    iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImage);
    dimScreen(0.66);

    double cx = SCREEN_WIDTH / 2.0;

    // ---- the plate ------------------------------------------------------
    // gamePlayerName.png carries ENTER THE PLAYER NAME, the divider and the
    // two hints along the bottom already. All the game has to add is the name
    // being typed, into the framed field in the middle of it.
    if (namePlateArt.tex != 0)
    {
        double pw = 700.0;
        double ph = pw / namePlateArt.aspect;
        if (ph > 300.0) { ph = 300.0; pw = ph * namePlateArt.aspect; }

        double px = cx - pw / 2.0;
        double py = 210.0;

        drawSpriteUV(namePlateArt.tex, px, py, pw, ph, false,
                      namePlateArt.u0, namePlateArt.v0,
                      namePlateArt.u1, namePlateArt.v1);

        double fy0 = py + NAME_BOX_V0 * ph;
        double fy1 = py + NAME_BOX_V1 * ph;
        double fh  = fy1 - fy0;

        double size = fh * 0.66;
        if (size > 34.0) size = 34.0;

        // the field is only so wide, so a long name shrinks to fit rather
        // than running out over the frame
        double fw = (NAME_BOX_U1 - NAME_BOX_U0) * pw - 24.0;
        double tw = gameTextWidth(playerName, size);
        if (tw > fw && tw > 0.0) { size *= fw / tw; tw = fw; }

        double tx = cx - tw / 2.0;
        double ty = fy0 + (fh - size) / 2.0 + size * 0.14;

        if (playerNameLen > 0)
            drawStatusText(cx, ty, playerName, size, 240, 228, 208);

        // a caret that blinks, so an empty field still looks like it is waiting
        if ((nameCaretTick / 18) % 2 == 0)
            fillRectAlpha(tx + tw + 3, ty + 2, 3, size * 0.72, 235, 200, 150, 0.9);

        // what the board already holds, so there is something to beat
        if (scoreCount > 0)
        {
            char best[96];
            sprintf(best, "BEST SO FAR    %s    %d", scoreTable[0].name, scoreTable[0].score);
            drawStatusText(cx, py - 42.0, best, 22, 206, 172, 96);
        }
        return;
    }

    // ---- the painted panel, for when the picture is not there -----------
    double pw = 560, ph = 210;
    double px = cx - pw / 2.0, py = 210;
    drawPanel(px, py, pw, ph, 190, 60, 50);

    drawStatusText(cx, py + ph - 62, "ENTER THE PLAYER NAME", 34, 226, 200, 150);

    double bw = 420, bh = 56;
    double bx = cx - bw / 2.0, by = py + 72;
    fillRectGradient(bx, by, bw, bh, 8, 6, 10, 0.92, 22, 16, 24, 0.92);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4d(0.78, 0.30, 0.26, 0.85);
    glBegin(GL_LINE_LOOP);
        glVertex2d(bx, by); glVertex2d(bx + bw, by);
        glVertex2d(bx + bw, by + bh); glVertex2d(bx, by + bh);
    glEnd();
    glColor4d(1, 1, 1, 1);
    glDisable(GL_BLEND);

    double size = 30.0;
    double tw   = gameTextWidth(playerName, size);
    double tx   = cx - tw / 2.0;
    double ty   = by + (bh - size) / 2.0 + 2.0;
    if (playerNameLen > 0)
        drawStatusText(cx, ty, playerName, size, 240, 228, 208);

    if ((nameCaretTick / 18) % 2 == 0)
        fillRectAlpha(tx + tw + 3, ty + 2, 3, size * 0.72, 235, 200, 150, 0.9);

    drawStatusText(cx, py + 34, "ENTER TO BEGIN        ESC TO GO BACK",
                    19, 168, 150, 136);

    if (scoreCount > 0)
    {
        char best[96];
        sprintf(best, "BEST SO FAR    %s    %d", scoreTable[0].name, scoreTable[0].score);
        drawStatusText(cx, 150, best, 22, 206, 172, 96);
    }
}


// ===========================================================================
//
//                    * * *  T H E   S C O R E B O A R D  * * *
//
//  Shown once a run is finished, straight after the closing screen. The row
//  this run just earned is lit up so you can see where you landed.
// ===========================================================================
Button scoreboardButtons[1] = {
    { (SCREEN_WIDTH - 250) / 2.0, 36, 250, 54, "MAIN MENU", false }
};

void showScoreboard()
{
    gameState = STATE_SCOREBOARD;
}

void drawScoreboard()
{
    iClear();
    iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImage);
    dimScreen(0.78);

    double cx = SCREEN_WIDTH / 2.0;

    drawStatusText(cx + 2, SCREEN_HEIGHT - 76, "SCOREBOARD", 46, 0, 0, 0);
    drawStatusText(cx,     SCREEN_HEIGHT - 74, "SCOREBOARD", 46, 232, 196, 120);

    fillRectAlpha(cx - 330, SCREEN_HEIGHT - 92, 660, 2, 200, 120, 60, 0.7);

    if (scoreCount == 0)
    {
        drawStatusText(cx, 330, "NOBODY HAS WALKED OUT OF THE ABYSS YET", 24, 170, 158, 146);
        drawButton(scoreboardButtons[0]);
        return;
    }

    // column headings
    double top = SCREEN_HEIGHT - 132.0;
    double row = 44.0;
    double colRank = cx - 320, colName = cx - 262, colScore = cx + 60;
    double colKills = cx + 168, colTime = cx + 262;

    drawGameText(colRank,  top, "#",     18, 150, 136, 124);
    drawGameText(colName,  top, "NAME",  18, 150, 136, 124);
    drawGameText(colScore, top, "SCORE", 18, 150, 136, 124);
    drawGameText(colKills, top, "KILLS", 18, 150, 136, 124);
    drawGameText(colTime,  top, "TIME",  18, 150, 136, 124);

    int shown = scoreCount;
    if (shown > SCORE_SHOW_ROWS) shown = SCORE_SHOW_ROWS;

    for (int i = 0; i < shown; i++)
    {
        double y = top - 42.0 - row * i;   // clear of the column headings
        bool mine = (i == scoreJustSet);

        double r = 214, g = 200, b = 182;
        if (mine)
        {
            double pulse = 0.5 + 0.5 * sin(uiTick * 0.10);
            fillRectAlpha(cx - 336, y - 8, 672, row - 6,
                           150, 60, 40, 0.24 + 0.16 * pulse);
            r = 255; g = 214; b = 150;
        }
        else if (i == 0) { r = 236; g = 206; b = 132; }

        char buf[64];
        sprintf(buf, "%d", i + 1);
        drawGameText(colRank, y, buf, 24, r * 0.8, g * 0.8, b * 0.8);
        drawGameText(colName, y, scoreTable[i].name, 24, r, g, b);

        sprintf(buf, "%d", scoreTable[i].score);
        drawGameText(colScore, y, buf, 24, r, g, b);

        sprintf(buf, "%d", scoreTable[i].kills);
        drawGameText(colKills, y, buf, 24, r, g, b);

        formatClock(scoreTable[i].seconds, buf);
        drawGameText(colTime, y, buf, 24, r, g, b);
    }

    if (scoreJustSet >= SCORE_SHOW_ROWS)
    {
        char note[96];
        sprintf(note, "YOUR RUN CAME %d OF %d    %d POINTS",
                 scoreJustSet + 1, scoreCount, scoreTable[scoreJustSet].score);
        drawStatusText(cx, 108, note, 21, 226, 180, 120);
    }

    drawButton(scoreboardButtons[0]);
}

void updateScoreboardHover(int mx, int my)
{
    scoreboardButtons[0].hover = pointInButton(scoreboardButtons[0], mx, my);
}

void handleScoreboardClick(int mx, int my)
{
    if (pointInButton(scoreboardButtons[0], mx, my))
    {
        scoreJustSet = -1;
        currentLevel = 1;
        gameState = STATE_MENU;
    }
}


void handleMenuClick(int mx, int my)
{
    // PLAY asks for a name, then shows the story pages, then starts layer 1.
    if (pointInButton(menuButtons[0], mx, my))       startNameEntry();
    else if (pointInButton(menuButtons[1], mx, my))  gameState = STATE_INSTRUCTIONS;
    else if (pointInButton(menuButtons[2], mx, my))  exit(0);
}

void handleInstructionsClick(int mx, int my)
{
    if (pointInButton(instructionsButtons[0], mx, my))
        gameState = STATE_MENU;
}

#endif 
