# Wagic (Android Fork)

This repository is a modernized fork of Wagic, the Homebrew, focused exclusively on Android development.

---

## Overview

The original Wagic project supports multiple legacy platforms and build systems. This fork is intended to steamline the build pipeline for Android development and migrate the build system to Android Studio/Gradle.

---

## Goals

- Deprecate non-Android platforms (PSP, iOS, desktop, legacy ports)
- Replace legacy build systems with a modern Gradle-based pipeline
- Simplify project structure for easier development and debugging
- Remove outdated dependencies and reduce technical debt
- Gradually migrate away from SDL toward a more modern rendering approach

---

## Vision

This fork aims to transform Wagic into a clean, maintainable, and extensible Android codebase.

The long-term goal:
- Easier feature development
- Improved performance and stability
- Integration into automated build pipelines

---

### Build Requirements

- Android Studio (latest recommended)
- Android SDK
- Android NDK (required for native components)

---

### Build Instructions

1. Open the project in Android Studio  
2. Allow Gradle to sync  
3. Select a device or emulator  
4. Run the `app` module
