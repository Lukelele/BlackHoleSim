# General Relativity 3D Black Hole Ray Tracer

A real-time, physically accurate 3D black hole ray tracer built with modern OpenGL, C++17, and GLSL. Light paths are numerically calculated using null geodesic equations derived directly from Einstein's General Relativity in curved Schwarzschild and Kerr spacetimes.

---

## Features

### 1. General Relativistic Geodesic Physics
- **3D Null Geodesic Equation**:
  $$\frac{d^2 \vec{r}}{d\lambda^2} = -\frac{3 M |\vec{r} \times \vec{v}|^2}{r^5} \vec{r} + \vec{a}_{\text{Kerr}}$$
  where $\lambda$ is the affine parameter, exactly preserving the angular momentum vector $\vec{L} = \vec{r} \times \vec{v}$.
- **Numerical Integrator**: Adaptive 4th-Order Runge-Kutta (RK4) integration on the GPU, scaling step sizes dynamically near high-curvature regions.
- **Relativistic Boundaries**:
  - Event horizon: $r_H = M + \sqrt{M^2 - a^2}$
  - Ergosphere boundary: $r_E(\theta) = M + \sqrt{M^2 - a^2 \cos^2\theta}$
  - Photon sphere / orbit: $r_{ph} = 3M$ (Schwarzschild), split into prograde and retrograde radii in Kerr
  - Innermost Stable Circular Orbit (ISCO): $r_{\text{ISCO}} = 6M$ (Schwarzschild), contracting dynamically with prograde spin $a$
  - Critical capture impact parameter: $b_c = 3\sqrt{3} M \approx 5.196 M$

### 2. Accretion Disk & Relativistic Optics
- **Novikov-Thorne Thin Disk Model**: Vertical scale height flare $H(r)$ and radial temperature flux $F(r) \propto (r_{\text{in}} / r)^{2.5} (1 - \sqrt{r_{\text{in}} / r})$.
- **Relativistic Doppler Beaming**:
  $$\delta = \frac{1}{\gamma (1 - \vec{\beta} \cdot \hat{k})}, \quad I_{\text{obs}} = \delta^{3.5} I_{\text{emit}}$$
  Material orbiting towards the observer is boosted in intensity and blueshifted, creating the iconic Interstellar-style asymmetric brightness profile.
- **Gravitational Redshift**: Light climbing out of the gravitational potential well loses energy by $g_{\text{grav}} = \sqrt{1 - 2M/r}$, shifting spectrum towards red.
- **Planck Blackbody Radiation**: Color temperature mapping from 2,000 K (deep red/orange) to 40,000 K (brilliant blue-white).
- **Secondary & Tertiary Lensing**: Light from the back of the accretion disk is warped *over* the top and *under* the bottom of the black hole horizon.
- **Keplerian Differential Rotation**: Dynamic swirl with multi-octave procedural turbulence.

### 3. Celestial Background & Gravitational Lensing
- **Deep Space Skybox**: Procedural Milky Way galactic plane, nebular dust lanes, and multi-tier starfield with distinct stellar classifications.
- **Einstein Rings**: Background stars and nebula are deflected by gravitational lensing into Einstein rings and arcs around the black hole shadow.
- **Post-Processing**: ACES Filmic Tone Mapping and gamma correction ($2.2$).

### 4. GPU Architecture
- **GPU Fragment Shader Pipeline**: Optimized GLSL 410 core fullscreen shader executing massively parallel ray tracing on all GPU cores (60+ FPS on Apple Silicon M-series and modern GPUs).
- **GPU Compute Shader Pipeline**: Includes `src/Shaders/RayTracer.comp` (GLSL 430 with `imageStore`) for platforms supporting OpenGL 4.3+ compute shaders.

---

## Interactive Controls

| Control | Action |
| :--- | :--- |
| **Left Click + Drag** | Orbit camera around black hole (yaw / pitch) |
| **Right Click + Drag** | Pan camera focal point |
| **Mouse Scroll** | Zoom in / out (distance) |
| **W / S / A / D** | Fly forward / backward / left / right |
| **Q / E** | Fly down / up |
| **Tab / H** | Toggle astrophysicist GUI dashboard on/off (Cinematic Mode) |
| **1** | Camera preset: Cinematic Edge-On |
| **2** | Camera preset: Polar Overhead |
| **3** | Camera preset: 45° Perspective Angle |
| **4** | Camera preset: Extreme Photon Ring Close-up |
| **Space** | Pause / Resume accretion disk rotation |
| **R** | Reset camera view |
| **Escape** | Exit simulation |

---

## How to Compile and Run

### macOS

Using the included GLFW library:
```bash
clang++ -std=c++17 -O3 src/main.cpp src/Engine/*.cpp src/Core/*.cpp dependencies/include/imgui/imgui*.cpp dependencies/glad.c -I dependencies/include -I dependencies/include/imgui -I src dependencies/lib/libglfw.3.3.dylib -o bin/app && ./bin/app
```

Or if GLFW is installed via Homebrew (`brew install glfw`):
```bash
clang++ -std=c++17 -O3 src/main.cpp src/Engine/*.cpp src/Core/*.cpp dependencies/include/imgui/imgui*.cpp dependencies/glad.c -I dependencies/include -I dependencies/include/imgui -I src -lglfw -o bin/app && ./bin/app
```

### Linux

Install GLFW if not present (`sudo apt-get install libglfw3 libglfw3-dev`):
```bash
g++ -std=c++17 -O3 src/main.cpp src/Engine/*.cpp src/Core/*.cpp dependencies/include/imgui/imgui*.cpp dependencies/glad.c -I dependencies/include -I dependencies/include/imgui -I src -lglfw -o bin/app && ./bin/app
```

---

## Running Physics Verification Tests

To verify numerical conservation of angular momentum, critical impact parameter capture ($b_c = 3\sqrt{3} M$), and Einstein weak-field light deflection:
```bash
clang++ -std=c++17 tests/test_physics.cpp src/Core/BlackHole.cpp src/Core/Spacetime.cpp src/Core/Photon.cpp -I dependencies/include -I src -o bin/test_physics && ./bin/test_physics
```