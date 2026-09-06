GLFW is an Open Source, multi-platform library for OpenGL, OpenGL ES and Vulkan development on the desktop. It provides a simple API for creating windows, contexts and surfaces, receiving input and events.

GLFW is written in C and supports Windows, macOS, Wayland and X11. https://www.glfw.org/



## 🎯 Purpose of the Engine

This engine is not meant to compete with production engines like Unity or Unreal.  
Instead, it serves as a **learning-focused codebase**, designed to reveal how:

- Rendering pipelines are built from zero.
- Physics and collision systems function internally.
- Scene graphs, components, UI systems and scripting expand engine architecture.
- Audio integrated
- Real 2D and 3D gameplay logic is implemented.

Every subsystem is introduced gradually with detailed breakdowns and simple C++ implementations.

---

## 💻 Supported Platforms

The engine is designed to be **cross-platform** and currently targets:

- ✅ **Windows**
- ✅ **macOS**
- ✅ **Linux**

---

## 🛠️ Technologies & Standards

| Technology        | Details                               |
|------------------|---------------------------------------|
| **Language**      | C++ (C++17 standard)                  |
| **Build System**  | CMake                                 |
| **Graphics**      | OpenGL                                |
| **Window Management**      | GLFW                                |

---

## 📂 Project Structure

| Folder             | Description |
|------------------|------------|
| `source/`         | Project source code |
| `thirdparty/`     | Third-party libraries |
| `lessons/`        | Lesson materials |
| `CMakeLists.txt`  | Build configuration |

---

## ⚙️ Build Instructions

You can build the engine using one of two approaches:

### 1. Generate Project via Script
- `./gen_proj_vs2022.bat` for Visual Studio 2022
- `./gen_proj_xcode.sh` for Xcode
Then open the generated project from `build` folder in your preferred IDE.

### 2. Open Repository with IDE in CMake Mode (Recommended)
Simply open the repository root in **CLion / Visual Studio Code / Visual Studio (CMake mode)** and let it configure automatically.
