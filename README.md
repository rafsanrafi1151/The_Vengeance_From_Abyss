# Vengeance from the Abyss

A 2D side-scrolling beat-em-up written in C++ with the iGraphics library
(a thin wrapper over OpenGL and GLUT). Four layers, four bosses, a story
told in animated speech boxes, and a special move.

Built as a CSE 1200 Software Development project.

---

## The game

Karl is convicted of a murder he did not commit and thrown into the Abyss.
Four layers stand between him and the truth.

| Layer | What is down there |
|-------|--------------------|
| 1 | The waste. Waves of shades and prisoners. |
| 2 | The Warden's gate. Waves, then the Warden, 200 HP. |
| 3 | The rot and the hound. Waves of warspan, then the swamp thing, then the gate hound. |
| 4 | Meafesto. No ordinary enemies, and he has two lives. |

## Controls

| Key | Action |
|-----|--------|
| `A` / `D` or arrows | Run. Hold to build speed. |
| `SPACE` | Jump. Tap for a hop, hold for the full jump. Press again in mid-air to double jump. |
| `S` / `Down` | Fast fall |
| `K` | Kick |
| `H` | Special move. Hits everything around you, both sides at once. Unlocked from layer 2. |
| `ESC` | Pause, or skip a conversation |
| `ENTER` or click | Advance a conversation |

The game runs fullscreen. The picture is drawn in a fixed 1000 x 600 world
and letterboxed onto the monitor, so it keeps its shape at any resolution
and the mouse stays lined up with the buttons.

## Building it

1. Open `GameBuilding.vcxproj` in Visual Studio (built with the v120
   toolset, Win32).
2. Build and run.

Every path in the project file is relative, so it builds wherever you put
the folder. `GLUT32.DLL` has to sit next to the executable, and the game
loads its pictures and music with relative paths, so the working directory
must be the folder holding the project file. That is what Visual Studio
does by default.

## Changing how it plays

`LevelSettings.h` is the file to edit. Nothing else has to be touched.

- The wave tables. One line is one wave: `{ small, big, warspan }`. Add a
  line for another wave, delete one to remove it. The counts work
  themselves out.
- `START_LEVEL` jumps straight to a later layer while you are working on
  it. Set it back to 1 before handing the game in.
- `WAVE_CLEAR_DELAY_TICKS`, the pause between waves.
- `SPECIAL_MOVE_FROM_LEVEL`, which layer the special move unlocks in.
- The frame counts for the animation sheets.

The game runs on a fixed 16 millisecond tick, so 62 ticks is one second.

## How the code is laid out

| File | What is in it |
|------|---------------|
| `iMain.cpp` | Entry point, the draw and update switch, input routing, which music plays |
| `GameUtility.h` | Constants, sprite and animation types, the bitmap alphabet, health bars, fullscreen, the conversation system |
| `LevelSettings.h` | The waves and the knobs worth turning |
| `Player.h` | The hero: movement, jumping, the kick, the special move |
| `Enemy.h` | Enemy types, art, the chase, combat, speech box placement |
| `Level1.h` .. `Level4.h` | One layer each: flow, HUD, banners, result screen, and that layer's dialogue script |
| `Menu.h` | Main menu, instructions, and the story pages |
| `Audio.h` | Music through `PlaySound`, hit sounds through `waveOut` |
| `iGraphics.h`, `stb_image.h` | The library and the image decoder |

### Two things worth knowing before you change the art

Every animation is **one row of equal cells** in a single PNG, with a
transparent background. A sheet that still has its background baked in is
ignored rather than drawn as a black rectangle, so the character falls back
to its standing picture instead of looking broken.

The writing uses a **bitmap alphabet**: `Images/gameFont.png`, a 16 by 6
grid holding ASCII 32 to 126 in order. Drop a replacement in as
`Images/gameFront.png` on the same grid and every letter's width is
measured off the picture automatically. Only plain keyboard characters can
be drawn, so write `"` `'` and `...`, never the curly kind.

## Assets

The pictures and music in this repository were gathered while building a
coursework project and are **not covered by the licence on the code**. Some
are AI generated, some are derived from reference material that belongs to
its own rights holders. If you fork this, replace the contents of `Images/`
and `Music/` with art you own or are licensed to use before you publish or
distribute anything.

The iGraphics library and `stb_image.h` carry their own licences.
