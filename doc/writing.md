<!-- Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl> -->
<!-- writing.md - Learning writing -->
# Writing

During the development of this game I would also like to learn writing correctly about my discoveries.
I love reading technical blogs and explanations that are made by individuals with interesting ideas.

- https://github.com/Ericson2314/baccumulation/blob/main/README.adoc

## Keep things simple
Exactly like writing code, keeping the text short and simple is probably the best solution.

## Use the "I" form
Everything in this project is written by me.
This means that there is no 'we'. The writing should be personal and written from my perspective.
Everything is my opinion, it is up to the reader what to do with that.
So there is no 'we', no one else wrote that text except for me.

## Spell and grammar check
Correct spelling and grammar is difficult, but very important.
I should find good tools that can help me with this.

### Helix
I love the [Helix](https://helix-editor.com/) editor, it is my primary editor.
Except it doesn't yet have a spell checker. I wish it had one.

- [Note talking with Helix, Tp-Note and LanguageTool](https://blog.getreu.net/20220828-tp-note-new8/)
- [Discussion: Spellchecking based on Tree-Sitter queries?](https://github.com/helix-editor/helix/discussions/3637)

### CSpell and VSCode Code Spell Checker
Works pretty well.

- [CSpell](https://cspell.org/)

### Language Tool

- `languagetool doc/writing.md`

Language tool seems to be better as it also checks grammar.

### aspell / hunspell
Aspell and hunspell are very similar. They are ok with Markdown but do not work well with code.

- `aspell check doc/writing.md`
- `hunspell doc/writing.md`
- `typos src/`, for checking code

None of these are very good. Suggestions are welcome.
