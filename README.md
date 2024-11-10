# 2D Alien Farm Game 🚀👽🌱

A unique farming simulation game built with C++ and SDL2, featuring an alien-themed world where you can create and manage an otherworldly ecosystem with dynamic tile systems, animated plants, and autonomous creatures.

## ✨ Features

### Tile System
- Multiple tile types with unique properties:
  - Water (creates moisture zones)
  - Dirt (normal/wet variants)
  - Multiple colored grass variants (Purple, Green, Yellow, Blue, White)
- Dynamic shadow generation based on tile height differences
- Automatic moisture propagation from water sources
- Checkered pattern rendering for visual depth

### Plant System
- 5 unique plant types with different characteristics
- Growth stages (small to full-size)
- Animated movement (floating/swaying effect)
- Environment-specific placement rules:
  - Some plants require wet soil
  - Others thrive on grass variants
- Dynamic shadows with transparency
- Collision detection with other game elements

### Animal System
- 3 different animal species with unique sizes
- Autonomous movement patterns:
  - Random position targeting
  - Random rotation
  - Collision avoidance
- Growth stages from young to adult
- Smart pathfinding to avoid:
  - Other animals
  - Plants
  - Water tiles
- Realistic shadows and movement animations

## 🎮 Controls

### Tile Placement
- Numeric Keys (1-7): Select tile types
  - 1: Water
  - 2: Dirt
  - 3-7: Different colored grass variants
- Left Mouse Button: Place selected tile
- Right Mouse Button: Remove plants/animals

### Plant Management
- Q-T: Select different plant types
  - Q: Plant Type 1 (Wet dirt)
  - W: Plant Type 2 (Wet dirt)
  - E: Plant Type 3 (Grass)
  - R: Plant Type 4 (Grass)
  - T: Plant Type 5 (Grass)
- Left Mouse Button: Place selected plant

### Animal Management
- A-D: Select different animal types
  - A: Animal Type 1 (Small)
  - S: Animal Type 2 (Medium)
  - D: Animal Type 3 (Large)
- Left Mouse Button: Place selected animal

### General Controls
- ESC: Exit game

## 🛠️ Technical Requirements

### Dependencies
- C++ Compiler (C++11 or higher)
- SDL2 Library
- CMake (recommended for building)

### Building

#### Windows with Visual Studio
1. Install SDL2 development libraries
2. Set up SDL2 include and library paths in project settings
3. Build the solution

#### Linux
```bash
# Install SDL2
sudo apt-get install libsdl2-dev

# Build the project
mkdir build && cd build
cmake ..
make
```

## 🎨 Asset Requirements

The game requires BMP image files for:
- Tile shadows (8 variants for edge cases)
- Plants (normal + shadow, small + full size variants)
- Animals (normal + shadow, small + full size variants)

All images should be:
- BMP format
- Transparency supported
- Located in the appropriate Data/Images subdirectory

## 🔧 Technical Implementation Notes

### Tile System
- Uses a grid-based system with customizable tile size (default 64px)
- Implements moisture propagation up to 2 tiles away from water
- Supports dynamic shadow casting based on tile height differences

### Physics & Collision
- Circle-based collision detection for animals
- Rectangle-based collision for plants
- Grid-based tile collision checks

### Graphics
- Hardware-accelerated rendering with SDL2
- Support for transparency and alpha blending
- Optimized texture management with texture pooling
- Dynamic shadow rendering with adjustable opacity

### Performance Optimizations
- Texture caching through TextureLoader
- Efficient collision detection using spatial partitioning
- Smart update system for active entities
