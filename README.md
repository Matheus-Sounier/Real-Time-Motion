<img width="450" height="275" alt="Image" src="https://github.com/user-attachments/assets/d5dca46c-8683-4e6d-b553-b613b214565a" />

# Real-Time Motion

A real-time motion-controlled system built with **C++**, **OpenCV**, and **CMake**.

This project allows players to control games using body movement detected through a webcam. It was originally designed around a Jump King-style gameplay mechanic, where physical jumps and movement inside detection areas are converted into keyboard inputs.

---

## Features

- Real-time motion detection using a webcam
- Customizable motion detection areas (ROIs)
- Keyboard binding for each detection area
- Persistent square configuration
- Optional jump detection system
- Jump power measurement based on airtime
- Motion-based directional controls
- Built with OpenCV and modern C++

---

## How It Works

The system uses two main concepts:

### Detection Squares

Detection squares (ROIs) can be placed anywhere on the camera feed.

When movement is detected inside a square, the assigned keyboard key is pressed automatically.

---

### Jump Detection

When enabled, a horizontal line appears on the screen.

The system monitors movement above this line.

---

## Requirements

Before building the project, install:

### 1. Visual Studio

Install Visual Studio with the **Desktop Development with C++** workload.

### 2. CMake

Download and install CMake:

https://cmake.org/download/

Make sure CMake is available in your system `PATH`.

### 3. OpenCV

Download OpenCV:

https://opencv.org/releases/

Add OpenCV to your Windows `PATH` and configure it correctly for your build environment.

---

## Installation

Clone the repository:

```bash
git clone git@github.com:Matheus-Sounier/Real-Time-Motion.git
```

Enter the project directory:

```bash
cd Real-Time-Motion
```

Create a build directory:

```bash
mkdir build
```

Generate build files:

```bash
cmake -B build/
```

Build the project:

```bash
cmake --build build/
```

---

## Running

After building, the executable will be generated inside the build folder.

Example:

```text
D:\Projects\YourProjectDirectory\build\Debug\JumpKingIRL.exe
```

Run the executable from the terminal.

---

## Initial Configuration

When the program starts, the following instructions will be displayed:

```text
USAGE:

HOW TO CREATE A SQUARE:
- Double-click with the left mouse button to add a detection square at the clicked location.

HOW TO BIND A KEY:
- Click on a square to select it.
- Press a key to bind it to the selected square.

HOW TO MOVE A SQUARE:
- Click on a square to select it.
- Use arrow keys to move the square within the window.

HOW TO REMOVE A SQUARE:
- Click on a square to select it.
- Right-click on a square to remove it.

Enable jump detection? (send 'yes' or 'no')
```

---

## Jump Detection Setup

When the application starts, you will be asked:

```text
Enable jump detection? (send 'yes' or 'no')
```

### If you choose `yes`

- Jump detection will be enabled.
- A horizontal white line will appear.
- Physical jumps crossing the line will be detected.

### If you choose `no`

- The program won't work properly.
