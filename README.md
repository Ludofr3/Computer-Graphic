# 3D Physics Simulation with Archimedes' Buoyancy

## 📋 Description

This project was developed **as part of a physics simulation course** and implements an interactive 3D physics simulation focusing on **Archimedes' buoyant force** and fluid-object interactions. The simulation allows users to visualize the behavior of various geometric objects (cubes, ellipsoids, pyramids) in a fluid environment with realistic physical calculations.

## ✨ Current Features

### 🎯 Physics Simulation

* **Archimedes’ Buoyant Force**: Accurate buoyant force based on submerged volume
* **Drag Forces**: Resistance proportional to the square of the velocity
* **Gravity**: Realistic gravitational force application
* **Damping**: Simulation of energy loss (linear and angular)

### 🔷 Supported Geometric Objects

* **Cubes**: With exact moment of inertia tensor calculation
* **Ellipsoids/Spheres**: Configurable geometry (independent radii)
* **Pyramids**: Rectangular base with appropriate physics

### 🌊 Fluid Simulation

* Submersion detection via volumetric sampling
* Dynamic center of buoyancy computation
* Support for different fluid densities

## 🏗️ Architecture

### Main Classes

#### `Mover` (Abstract Class)

Base class for all simulated physical objects.

#### `EllipsoidMover`

```cpp
// Create an ellipsoid
EllipsoidMover(position, radiusX, radiusY, radiusZ, density)
// Or a sphere
EllipsoidMover(position, radius, density)
```

#### `PyramidMover`

```cpp
// Rectangular-based pyramid
PyramidMover(position, halfWidth, halfDepth, height, density)
```

#### `Fluid`

Handles fluid-object interactions:

* `getBuoyancy()` : Computes Archimedes’ force using sampling
* `getArchimedesForce()` : Simplified buoyant force
* `getDragForce()` : Drag forces

## 🚀 Installation & Compilation

### Prerequisites

* **vcpkg**: C++ package manager (required)
* **FLTK**: UI and OpenGL rendering
* **OpenGL**: 3D rendering
* **Cyclone Physics Engine**: Physics engine
* **C++ Compiler** supporting C++11 or newer

### Installing Dependencies

#### 1. Install vcpkg

```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh    # Linux/macOS
# or
./bootstrap-vcpkg.bat   # Windows
```

#### 2. Install libraries via vcpkg

```bash
./vcpkg install fltk
./vcpkg install opengl
./vcpkg install glut
```

#### 3. Integrate with your IDE

```bash
./vcpkg integrate install
```

### Compilation

```bash
# With vcpkg configured
g++ -o simulation main.cpp MyGlWindow.cpp Fluid.cpp \
    -lfltk -lfltk_gl -lGL -lGLU -lglut

# Or use your IDE with vcpkg integrated (Visual Studio, CLion, etc.)
```

## 🎮 Usage

### User Interface

* **Run/Pause**: Button to start/stop the simulation
* **Step**: Advance frame by frame
* **FrameRate**: Simulation speed control (15/30/60 FPS)
* **Test Slider**: Adjustable test parameter

### View Controls

* Interactive 3D navigation
* Camera zoom and rotation
* Shadow mode toggle

## 🔬 Physics Calculations

### Archimedes’ Buoyancy

The buoyant force is calculated as:

```
F_buoyancy = ρ_fluid × V_submerged × g
```

The submerged volume is determined by 3D sampling (default 8×8×8 points).

### Inertia Tensors

#### Ellipsoid

```
Iₓ = (1/5)m(r²ᵧ + r²𝓏)
Iᵧ = (1/5)m(r²ₓ + r²𝓏)  
I𝓏 = (1/5)m(r²ₓ + r²ᵧ)
```

#### Pyramid

```
Iₓ = (1/20)m(b² + h²)
Iᵧ = (1/10)m(a² + b²)
I𝓏 = (1/20)m(a² + h²)
```

## 🎨 Object Configuration

### Example Configuration

```cpp
// Cubes with different densities
new CubeMover(Vector3(0,15,0), Vector3(1,1,2), 1000.0f)

// Various ellipsoids  
new EllipsoidMover(Vector3(10,13,0), 1.0f, 2.0f, 3.0f, 500.0f)

// Pyramids
new PyramidMover(Vector3(10,20,10), 1.0f, 2.0f, 3.0f, 750.0f)
```

### Color Coding

* **🟡 Yellow**: Light objects (density < 1000 kg/m³)
* **🔵 Blue**: Heavy objects (density ≥ 1000 kg/m³)
* **🔴 Red**: Resting objects (sleeping)
* **⚫ Gray**: Shadow mode

## 📊 Project Status

### ✅ Implemented Features

* [x] Basic simulation with Archimedes' buoyancy
* [x] Multiple geometric objects (cube, ellipsoid, pyramid)
* [x] FLTK user interface
* [x] OpenGL rendering with shadows
* [x] Accurate physical calculations (mass, inertia, forces)

### 🚧 In Development

* [ ] Object collisions
* [ ] Spring-damper interactions
* [ ] Particle simulation

## 🐛 Known Issues

* Limited performance with a large number of objects
* Buoyancy computation via sampling (approximate)
* Basic user interface

## 📄 License

\[Specify the project license]

## 📚 References

* **Cyclone Physics Engine**: Physics engine used
* **FLTK Documentation**: UI toolkit
* **Archimedes’ Principle**: Theoretical foundation of the simulation

---

*3D Physics Simulation Project – Focus on Fluid Interactions*
