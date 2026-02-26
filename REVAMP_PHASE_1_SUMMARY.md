# ChordNova Revamp - Phase 1 Summary

## Overview

Phase 1 introduces a complete rewrite of the ChordNova core as **chordnovarw**, built with modern C++20 under the `chordnovarw::` namespace. The legacy GUI application in `main/` remains functional and unchanged in behavior, with added documentation and const-correctness improvements.

**Stats**: 299 files changed, ~38,800 lines added, ~5,400 lines modified.

---

## Architecture

### Namespace & Class Hierarchy

All new code lives under `chordnovarw::` with submodules: `model`, `algorithm`, `service`, `io`, `utility`.

Core data types:
- **Pitch** — MIDI number wrapper, decomposable into PitchClass (0–11) and Octave
- **PitchIterable** — abstract base for collections of pitches
- **OrderedChord** — voiced chord (ordered pitches), extends PitchIterable
- **PitchSet** — unordered unique pitch collection, extends PitchIterable

### Module Breakdown

#### `src/model/` — Data Structures
- `Pitch`, `PitchClass`, `PitchSet`, `OrderedChord` — musical primitives
- `OrderedChordStatistics` — single-chord metrics (tension, thickness, root, geometric center)
- `BigramChordStatistics` — transition metrics (chroma, voice-leading distance, similarity, Q-indicator, common notes)
- `ProgressionConfig` — aggregates 11 constraint structs decomposed from the legacy ~80 fields:
  - VoiceLeadingConstraints, RangeConstraints, HarmonicConstraints, AlignmentConfig, ExclusionConfig, PedalConfig, UniquenessConfig, ScaleConfig, SimilarityConfig, RootMovementConfig, BassConfig, ChordLibraryConfig, SortConfig
- `CircleOfFifths`, `Octave`, `Semitone`, `Chroma`, `COFUnit` — theory helpers

#### `src/algorithm/` — Core Algorithms
- **Validation** — 15-stage pipeline with short-circuit evaluation:
  1. Monotonicity, 2. Range, 3. Alignment, 4. Exclusion, 5. Pedal inclusion, 6. Cardinality, 7. Single-chord stats, 8. Scale membership, 9. Bass & library, 10. Uniqueness, 11. Voice-leading constraints, 12. Similarity lookback, 13. Circle-of-Fifths span, 14. Q-indicator, 15. Voice movement uniqueness
- **Progression** — single-step chord generation engine
- **Sorting** — multi-criteria sorting by 13+ keys (tension, chroma, sv, similarity, etc.) with right-to-left priority and ascending/descending modifiers
- **Analysis** — bigram analysis for consecutive chords
- **Substitution** — search across all 4095 pitch-class subsets

#### `src/service/` — Higher-Level Operations
- **VoiceLeading** — optimal voice-leading vector computation with expansion mode
- **Expansion** — chord expansion to target voice count via binomial combinations
- **ChordLibrary** — singleton cache for chord statistics

#### `src/io/` — Input/Output
- **NoteParser** — parse "C4 E4 G4" or MIDI numbers into OrderedChord
- **Database** — load chord database and alignment files
- **Formatter** — format results with statistics for display
- **MIDI** — Standard MIDI File (Format 0) writer with VLQ encoding

#### `src/utility/` — Utilities
- **Combinatorics** — binomial coefficients, lazily-cached expansion index lookup (replaces legacy 15MB static array)
- **MixedRadix** — iterator over mutation vectors in [-vl_max, vl_max] with dead zone support
- **MIDI Encoding** — VLQ encoding/decoding

---

## Test Suite

Comprehensive Google Test suite with **17 test files** covering all modules:

| Module | Files | Description |
|--------|-------|-------------|
| model | 3 | OrderedChord, Pitch, PitchClass, Config validation |
| algorithm | 3 | All 15 validators, progression generation, multi-key sorting |
| service | 4 | Expansion, voice-leading, analysis, substitution |
| io | 3 | Note parser, MIDI writer, database loading |
| utility | 3 | Combinatorics, mixed-radix iteration, VLQ encoding |

---

## Build System

- **CMake** (C++20) with FetchContent for Google Test and Apache log4cxx
- Modular shared library targets: `chord`, `utility`, `service`, `algorithm`, `io`, `logger`
- **Makefile** shortcuts: `make build`, `make test`, `make doc`

---

## Documentation

- **Doxygen** configuration with generated HTML in `doc/html/`
- **mainpage.dox** covering both legacy algorithm overview and revamp goals
- **User guide** (`ChordNova.3.0.User.Guide.Simple.CN.md`) in Chinese
- Legacy code (`main/`) annotated with Doxygen-compatible comments

---

## Legacy Code Changes

- Added comprehensive Doxygen documentation to `main/chord.h`, `main/chord.cpp`, `main/functions.cpp`
- Const-correctness improvements across legacy headers
- Consolidated constants: `main/constant.h` removed, replaced by `src/include/constant.h` with backward-compatible alias
- No behavioral changes to the legacy GUI application
