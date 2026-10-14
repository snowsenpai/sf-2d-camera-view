# 2D Camera Sandbox

A C++17 / SFML 3 sandbox for implementing and comparing 2D camera systems.

## Overview

The sandbox is a small playable scene: a circle-shaped player moves around a background map that is larger than the window, and different cameras can be swapped at runtime to see how each one frames the world.

It exists as a practical environment for studying camera architecture, coordinate spaces and camera mathematics, and for testing how cameras interact with gameplay. For example, a room change repositions the player and pauses movement while the view slides to the next room.

## Features

### Fixed (room-based) camera

The world is divided into a grid of rooms, each the size of the view. With the default 768x512 window and the included 1536x1024 map, this produces a 2x2 grid. The current room is derived from the player's position. When the player's center leaves it, the camera slides to the neighboring room's center over 0.45 seconds with eased motion, gameplay is paused for the duration of the slide, and the player is repositioned just inside the new room.

The fixed camera ignores zoom. It requires the world size to be an exact multiple of the view size, which is asserted on activation.

Typical uses: top-down adventures, puzzle games, and any design where each screen is a self-contained space.

### Follow camera

The camera tracks the player with the following behaviors:

- **Dead zone:** the camera stays still while the player moves inside a rectangle around the view center (default half-size 64x48 world units), and moves only enough to keep the player inside it.
- **Smoothing:** the camera center eases toward its target with a frame-rate-independent exponential catch-up (default rate 8 per second; 0 disables smoothing).
- **World clamping:** the view never extends outside the world.
- **Zoom:** 0.5x to 3.0x with the mouse wheel.
- **Activation:** on activation the camera snaps to the player's position, clamped to the world.

Typical uses: platformers, scrolling levels, and open-ended exploration where the world is larger than the screen.

### Shared features

- **Camera switching:** cameras are registered with the manager and switched at runtime. Switching is an instant cut; the new camera initializes from the player's current position.
- **Mini-map:** a toggleable second view in the top-right corner of the game area. It covers 25% of the game area's width and height, shows three times the extent of the main view, and outlines the main view's footprint.
- **Debug visualization:** the fixed camera outlines the current room; the follow camera draws its dead zone. A separate overlay shows collision boxes.
- **Viewport resizing:** the window can be resized. The view keeps its aspect ratio with letterbox bars, and the mini-map viewport is derived from the letterboxed area so it never lands in the bars.
- **Pixel-aligned view:** the top-left corner of the view is snapped to whole pixels to avoid shimmering while scrolling.

## Technologies

| Technology | Details |
| --- | --- |
| C++ | C++17 |
| SFML | 3.1.0, Graphics module, fetched from GitHub by CMake `FetchContent` |
| CMake | 3.28 or newer |

There are no other third-party dependencies.

## Project architecture

| Component | Responsibility |
| --- | --- |
| `Camera` | Common interface: `onEnter`, `update`, `center`, plus optional `zoom`, `zoomBy`, `blocksGameplay` and a debug draw hook. |
| `FixedCamera` | Room grid, room detection, and the timed slide between rooms. |
| `FollowCamera` | Dead zone, smoothing, world clamping and zoom. |
| `CameraManager` | Owns the registered cameras, tracks the active one, owns the main and mini-map views, and applies each camera's center and zoom to the view. Also handles viewports. |
| `CameraContext`, `CameraEvents` | Input data a camera receives each frame, and the events it reports back. |
| `Game` | Gameplay integration: input, movement, collision, the per-frame camera update, and rendering. |

`Vec2`, `Entity`, `EntityManager` and `Components` form a minimal entity layer that supports the demo scene.

Each frame, `Game` builds a `CameraContext` (player position, world size, base view size, delta time) and passes it to the manager, which forwards it to the active camera. The camera updates its own state and returns `CameraEvents`; the manager copies the camera's center and zoom into the SFML view. Cameras never touch the player or the window. When the fixed camera changes rooms, it reports the crossed edge in `CameraEvents` and `Game` repositions the player. `Game` asks the manager whether the active camera blocks gameplay, and skips movement and collision while it does.

Frame order:

```cmd
input -> movement + collision (skipped while a camera blocks gameplay)
      -> camera update, then Game reacts to CameraEvents
      -> render (main view, optional debug overlay, optional mini-map)
```

### Directory structure

```cmd
sf-2d-camera-view/
|-- CMakeLists.txt
|-- assets/
|   `-- fantasy_world_map1.png
`-- src/
    |-- main.cpp
    |-- Game.h / Game.cpp
    |-- Camera.h                 common camera interface
    |-- CameraTypes.h            CameraContext, CameraEvents
    |-- CameraManager.h / .cpp
    |-- FixedCamera.h / .cpp
    |-- FollowCamera.h / .cpp
    |-- Vec2.h / Vec2.cpp
    |-- Components.h
    |-- Entity.h / Entity.cpp
    `-- EntityManager.h / EntityManager.cpp
```

## Building and running

### Prerequisites

- CMake 3.28 or newer.
- A C++17 compiler. The project is developed on Windows with Visual Studio (the "Desktop development with C++" workload).
- Git, and an internet connection on the first configure: SFML 3.1.0 is downloaded from GitHub and built alongside the project, so the first build takes longer.

### Visual Studio

1. Open the repository folder (File > Open > Folder).
2. Wait for CMake to finish configuring.
3. Select `camera_view` as the startup item and run.

### Command line

```cmd
cmake -B build
cmake --build build --config Release
```

`--config` applies to multi-configuration generators such as the Visual Studio generator. With single-configuration generators (Ninja, Makefiles), pass `-DCMAKE_BUILD_TYPE=Release` at the configure step instead.

### Running

The executable `camera_view` is written to `bin/` inside the build directory, with a configuration subfolder such as `Release` under multi-configuration generators. For example, with the Visual Studio generator:

```cmd
build\bin\Release\camera_view.exe
```

The game loads `assets/fantasy_world_map1.png` relative to the working directory. A post-build step copies the `assets/` folder next to the executable (only if the folder exists when CMake configures), so launch the program from the directory that contains the executable. When launching from an IDE, set the working directory to that folder. If the texture cannot be loaded, the game prints a message to the console.

The project has been built and run only on Windows. The CMake configuration is not Windows-specific, but other platforms are untested, and building SFML from source on Linux needs additional system packages; see the SFML documentation.

## Controls

| Input | Action |
| --- | --- |
| `W` `A` `S` `D` | Move the player (one direction at a time; priority up, down, left, right) |
| `1` | Switch to the fixed camera |
| `2` | Switch to the follow camera |
| Mouse wheel | Zoom (follow camera only) |
| `X` | Toggle the mini-map |
| `Z` | Toggle the camera debug overlay |
| `C` | Toggle the collision-box overlay |
| Window resize | Letterbox the view to its aspect ratio |

Key presses register once per press; key repeat is disabled.

## Camera concepts explored

- **World space versus screen space:** all gameplay and camera values are in world coordinates (pixels, origin at top-left, y pointing down). The view maps a region of world space onto the window, and the viewport decides which part of the window it occupies.
- **View size and center:** a view is defined by its center and size in world units. Zoom changes the size (base size divided by zoom) while the aspect ratio stays fixed.
- **Dead-zone tracking:** the camera's offset from the target is compared with a half-window size per axis, and the camera moves only by the amount the target exceeds it.
- **World-boundary clamping:** the valid camera centers on each axis range from half the view size to the world size minus half the view size. If the world is smaller than the view on an axis, the camera centers on the world along that axis.
- **Smoothing:** the camera moves a fraction `1 - exp(-rate * dt)` of the remaining distance each frame, which gives the same motion regardless of frame rate.
- **Room-based transitions:** the room is derived from the player's position, the camera interpolates between room centers, and gameplay reacts through events and a query instead of the camera modifying game state.
- **Viewports and the mini-map:** viewports use normalized window coordinates. Defining the mini-map viewport as a sub-rectangle of the letterboxed main viewport lets it follow the window size without separate resize logic, and keeping the same aspect ratio avoids distortion.

## Future improvements

These are ideas, not implemented features and not a committed roadmap.

- Blend between cameras when switching, instead of an instant cut.
- Make room transition toggleable.
- Debug gui(via SFMLImGui) to show active camera and settings(hopefully realtime).
- Look-ahead framing for the follow camera.
- Define zoom behavior for the fixed camera (for example, following the player within a room).
- Make the follow camera's dead zone and smoothing adjustable at runtime; they are currently set through default configuration values.
- Per-camera mini-map settings (center and ratio); the ratio is currently fixed at 3x.
