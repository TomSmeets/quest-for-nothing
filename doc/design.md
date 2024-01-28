---
title: Quest For Nothing
author: Tom Smeets
toc: true
---
<!-- Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl> -->
<!-- design.md: Explore and learn game design -->

# Motivation

I like creating something big from scrach where I can explain every detail.

A game is interesting because it is so diverse. It includes topics such as:

- Game Design
- Performance
- Low Latency
- Music
- Sound Effects
- Distribution
- Multiple Platforms
- Level Design
- Networking
- 3D Math
- Publishing A Game
- Procedural Generation

# Game Design

## General Idea
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

## Inspiration

- [2001 A Space Odyssey](https://www.imdb.com/title/tt0062622/)
- [Boarding Party](https://steamcommunity.com/sharedfiles/filedetails/?id=889907960)

## The Game Loop

1. Dock with alien spaceship
2. Perform Some Task
3. Escape

### The Task
- Self destruct
- Some Repair
- Gather Intel

### Challanges
- Aliens might attack you
- Aliens might talk to you
- Items and weapons found can be collected

### Weapons
- The Milo Gun: Cat shooting gun
- ...

## Features
- CO-OP Multiplayer
- Procedural generated spaceships and aliens
- Pretty colored Lighting
- Over use bloom effect, buttons, lights, eyes.
- The Cat-Cannon, shoots a cat, which attacks the target, and the returns back to the owner. A boomerang, but a cat.

## Theme

This game should feel like the movie "2001 A Space Odyssey".
High contrast between "Natural" and "Artificial".

### Natural
- Not Perfect / Dirty
- High detailed
- Textured polygons
- Green

### Artificial
- Perfrect / Clean
- Flat, untextured polygons
- White with a few strong contrasting colors
- Emissive Material in Flat colors
- Glowing buttons and indicator lights


### Color

Based on: [The Colors of Stanley Kubrick — Color Theory from The Shining to 2001: A Space Odyssey and More](https://www.youtube.com/watch?v=yVdhm9P8I6o)

#### Red

- Threat
- Danger
- Emotional intensity

#### Orange

- Destructive
- Candles
- Fire

#### Yellow & Gold

- Grandeur
- Isolate character from background

#### Green

- Natural
- Life
- Death

#### Blue

- Moonlight
- Sin
- Guilt

#### Purple

- Temptation

#### White

- Contrast
- for other colors
- Purity

## Other Design ideas
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
- Speed increases when killing aliens. But next level resets speed again.
- Level is a spaceship where you go through the entire ship to initiaite a self destruct sequence. Then you need to return back to your ship.
