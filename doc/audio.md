<!-- Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl> -->
<!-- audio.md - Learning about Music, Synthesizers, and Digital Audio -->
# Audio

During development I am probably listening to [this playlist](https://open.spotify.com/playlist/7bSrm9AXuFLnqxjLaTvzrD).
This game will probably have the same music vibe.

## Music Theory
- [Note Scale Reference](https://pages.mtu.edu/~suits/NoteFreqCalcs.html)
- [Music BPM Examples](https://crypt-of-the-necrodancer.fandom.com/wiki/Music)
- [Musical Form](https://en.wikipedia.org/wiki/Musical_form)
- [Analyzing KRAFTWERK's drum patterns](https://www.youtube.com/watch?v=2qhcp6iVWbw)

Music is structured.

- [A440](https://en.wikipedia.org/wiki/A440_(pitch_standard))

Beats per minute, examples are something around (120 - 140).
Syncing sound effects with the music sound nice.

A note has
- u32 id
- f32 velocity
- f32 time

Note
- octave
- Velocity
- Duration

note,octave,velocity,duration
A040
B040
C040

## MIDI
- [Midi Format 1](http://www.music.mcgill.ca/~ich/classes/mumt306/StandardMIDIfileformat.html#BMA1_)
- [Midi Format 2](https://electronicmusic.fandom.com/wiki/MIDI)
- [Note Velocity](https://electronicmusic.fandom.com/wiki/Velocity)

## Digital Audio And Synths
- [Intro to Synthesizers, A Beginner's Guide](https://www.youtube.com/watch?v=UmcOvAv-egI)
- [R-Scope mk.2 synth](https://www.youtube.com/watch?v=2XhaSXmPdKI)

## Filters
- [Digital Filters For Music Synthesis](https://karmafx.net/docs/karmafx_digitalfilters.pdf)
- [Low pass Filter](https://en.wikipedia.org/wiki/Low-pass_filter)
- [Filters with example code](https://www.musicdsp.org/en/latest/Filters/29-resonant-filter.html)

## Synth design
What is a good design for writing a synthesizer in this game?
I want to programmatically write the music. That would be cool.
This would involve 'notes' that play on given beats and last for some number of beats.

- https://open.spotify.com/track/7o6WAdouGszJEscEuuz0eP?si=3d2c702b106e4d0f
