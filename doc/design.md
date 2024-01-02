<!-- Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl> -->
<!-- design.md: Explore and learn game design -->
# Design

## Goal
Why am I developing a game?

I just want to program something interactive and create a standard library that I can reuse.
However, I realize that rewriting thins is usually more fun, so maybe I won't be using this in practice.
The experience gained from creating such a game and designing a standard library will never be wasted time.
Whether it will be used or not.
I don't care too much about the end result. I like the journey too much.


## Inspiration

- [2001 A Space Odyssey](https://www.imdb.com/title/tt0062622/)
- [Boarding Party](https://steamcommunity.com/sharedfiles/filedetails/?id=889907960)

## Space Game: Quest for Nothing
Something has happened to the universe. Everything seems to be slowing down.
Guess what, the universe is a simulation things just got bloated and busy.
What is the root problem? How are you going to solve it?

- Doom like shooter. Indoor environment with interesting lighting.
- Dark indoor environments with strong lights
- Randomly generated levels and enemies.
- Fully synthesized music and sound effects.
- Lots of gore and particles. Blood on the wall, bullets flying around, aliens thorn into pieces.
- Kill all aliens, start self-destruct sequence. Fly away and see huge explosion.
- Music also reflects the transition from complexity to simplicity.
- Some psychological aspects.
- Entropy reversal?

## Implementation
A very simplified sequence might look like:

1. Dock
2. Kill all aliens
3. Start self-destruct sequence
4. Escape

or

1. Dock
2. Talk to aliens
3. Solve some problems
4. Leave

Memory can be split in three primary arenas:
- Game*,  mem_game
- Level*, mem_level
- Frame*, mem_frame

Platform Targets:
- Linux
- Windows
- Wasm
- Wii


## Features

- CO-OP Multiplayer
- Procedural generated spaceships and aliens
- Pretty colored Lighting
- Over use bloom effect, buttons, lights, eyes.
- The Cat-Cannon, shoots a cat, which attacks the target, and the returns back to the owner. A boomerang, but a cat.

# Theme

This game should feel exactly like the movie "2001 A Space Odyssey".
After analyzing the movie I noticed the following design ideas.

High contrast between "Artificial" and "Natural".

## Natural

- Dirty
- High detailed

This means

- textured polygons

## Artificial

- Clean
- Flat polygons
- White with a few strong contrasting colors
- Every button and indicator Glows

This means

- Flat, untextured polygons
- Emissive colors

- Glowing buttons and indicator lights
- Strong contrasting colors. Mostly white with Black, Red, Green, Blue, Yellow, where appropriate.
- Life and natural things are very 'dirty' and not flat at all. They contrast the human made things.

## Lighting
- roughness: mirror like <--> diffuse
- color
- Emissive color

Idea: we could make the world with polygons directly. No textures.

## Color

Analysis from the following video combined with my own interpretations.

- [The Colors of Stanley Kubrick — Color Theory from The Shining to 2001: A Space Odyssey and More](https://www.youtube.com/watch?v=yVdhm9P8I6o)

### Red

- Threat
- Danger
- Emotional intensity

### Orange

- Destructive
- Candles
- Fire

### Yellow & Gold

- Grandeur
- Isolate character from background

### Green

- Natural
- Life
- Death

### Blue

- Moonlight
- Sin
- Guilt

### Purple

- Temptation

### White

- Contrast
- for other colors
- Purity

### Natural Things
- Textured high detail

### Artificial Things
- Flat polygons

# Other Design ideas
- You wake up in a spaceship, deserted. But all aliens were hiding.
- You can talk to an alien.
- You can kill or hurt an alien.
- You can leave the ship and go to earth.
- You can leave the ship and go to another ship.
- You evolve into a creature that you have made yourself, peaceful or angry.
- Other alien ships can dock with the current ship and provide reinforcements.
- Destroy a power generator -> it is dark.
- Score counter, which is the 'performance' of the simulation.
- Game speed keeps increasing.
- Some way to introduce temporary slow motion.
- Slow motion 'tokens' can be collected.
- Slow motion is stackable.
- Using too much slow motion reverses time. (cool hidden feature?)
- Reversing time past the start will exit the simulation and start a completely different side story.
- You can still re-enter the game, but now you are Neo or something.
- The slow motion tool works by temporarily increasing the complexity of the simulation.
