# Typer

A terminal-based typing speed test written in C. Challenge yourself to type common English words as quickly and accurately as possible, with real-time feedback and detailed performance metrics.

## Features

- **Real-time Feedback**: Watch your keystrokes highlighted in green (correct) or red (incorrect) as you type
- **Multiple Lines**: Type through 3 lines of random words to test your sustained typing ability
- **Performance Metrics**: Get detailed statistics after each session:
  - Accuracy percentage
  - Words Per Minute (WPM)
  - Total time elapsed
  - Correct vs. incorrect character counts
- **Random Word Pool**: Tests draw from a curated list of 150 common English words
- **Terminal Optimized**: Full keyboard support with raw mode for immediate character capture

## Building

This project uses the [builder](https://github.com/phantazie21/builder) build system for simple, streamlined compilation.

### Prerequisites

- GCC or Clang compiler
- [builder](https://github.com/phantazie21/builder) build system

### Build Instructions

```bash
# Build the project using builder
builder config

# Or use your choice of compiler
```

The configuration is defined in the `config` file:
```
name: typer
language: c
files: main.c
```

## Usage

Run the compiled executable:

```bash
./typer
```

### How to Play

1. The program displays a line of random words
2. Type each character as it appears on the screen
3. Correct keystrokes are highlighted in **green**, mistakes in **red**
4. Press Enter or Return to move to the next line
5. Press Ctrl+C to end the session early

After completing all 3 lines (or quitting early), you'll see your results:

```
--- results ---
correct   : 245
incorrect : 12
accuracy  : 95.3%
time      : 28.42s
wpm       : 51.8
```

## How It Works

### Core Components

- **Word Pool**: 150 common English words from which test lines are randomly generated
- **Input Handling**: Raw terminal mode captures keypresses immediately without buffering
- **Real-time Rendering**: Each keystroke updates the display with color-coded feedback
- **Timing**: Uses `clock_gettime()` for precise elapsed time measurement
- **Metrics Calculation**: 
  - **Accuracy** = (correct characters / total characters) × 100%
  - **WPM** = (correct characters / 5) / (elapsed minutes)

### Terminal Features

- Enables raw mode for immediate character input without waiting for Enter
- Supports backspace/delete to correct mistakes
- Uses ANSI color codes for visual feedback
- Automatically restores terminal settings on exit

## Requirements

- POSIX-compliant system (Linux, macOS, or similar)
- Standard C library with termios support
- C compiler (gcc recommended)

## Author

Created by phantazie21

## Related

- [builder](https://github.com/phantazie21/builder) - The build system used for this project
