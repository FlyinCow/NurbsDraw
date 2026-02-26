# NurbsDraw — AI Agent Instructions

## Overview

Interactive NURBS/Bezier curve editor built with C++23, ImGui (docking branch), and SDL3. The math layer provides geometric primitives (Vector, BezierCurve, BCurve/B-spline); the GUI layer renders and edits them via ImGui.

## Build

```bash
cmake --preset windows-msvc      # configure (Debug, Ninja, MSVC toolchain)
cmake --build --preset windows   # build → build/windows-msvc/NurbsDraw.exe
```

Sources are collected via `GLOB_RECURSE` on `src/**/*.cpp`. **After adding new `.cpp` files, CMake must be reconfigured** for them to be picked up.

## C++ Standard & Features

- **C++23** — actively uses deducing `this` (`auto &&member(this auto &&self)`), `std::format` / `std::formatter`, `constexpr`, `noexcept`.
- Prefer modern C++ idioms; avoid raw `new`/`delete`.

## Code Style

- 4-space indentation, CRLF line endings, opening brace on same line (K&R).
- `#pragma once` for all headers — no `#ifndef` guards.
- Include order: own header → project headers (`"math/Vector.h"`) → third-party (`"imgui.h"`, `"SDL3/SDL.h"`) → standard library (`<vector>`).
- All includes use paths relative to `src/` (e.g. `"gui/widget/Canvas.h"`).

## Naming

| Element | Convention | Example |
|---------|-----------|---------|
| Classes/Structs | UpperCamelCase | `BezierCurve`, `WindowManager` |
| Macros | UPPER_CAMEL_CASE | (if needed) |
| All functions | snake_case | `draw_curve_canvas`, `sample_bezier_uniform` |
| All variables | snake_case | `vertices`, `canvas_p0`, `draw_list` |
| Parameters | snake_case | `ctrl_points`, `window_name` |

## Architecture

```
src/
  math/       Geometry primitives (Vector, BezierCurve, BCurve, BernsteinBase)
  gui/
    widget/   Atomic ImGui widgets as free functions (Canvas, CoordList, ControlButton)
    window/   Window-level functions that compose widgets (DrawBezier)
    adaptor/  Adapter layer (placeholder)
  app/        Application layer (placeholder)
main.cpp      SDL3/ImGui init + main loop, delegates to gui/window functions
```

- **GUI widgets** are free functions, not classes. Persistent state lives in `static` locals inside the window function or is passed by reference.
- **Math classes** are value types with operator overloads (pass-by-value + `operator+=` pattern).
- `WindowManager` is a Meyer's singleton (`WindowManager::Get()`).

## Key Patterns

- Canvas widget uses a world-coordinate system with `world_to_screen` / `screen_to_world` lambdas, supporting pan (middle-drag) and zoom (scroll wheel).
- When a widget needs to mutate caller state, pass by **reference** (`bool &drawing`), not by value.
- `BezierCurve::eval(t)` uses Bernstein basis; `sample_bezier_uniform` lives alongside `BezierCurve` in `math/`.

## Dependencies (fetched via CMake FetchContent)

- **ImGui** — `docking` branch, built as `imgui_lib` static library with SDL3 backend.
- **SDL3** — `main` branch, static linking (`SDL_STATIC ON`).
