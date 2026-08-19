# QF_Player — Music Player in Qt

![Qt](https://img.shields.io/badge/Qt-6.x-green)
![C++](https://img.shields.io/badge/C%2B%2B-17-blue)
![Windows](https://img.shields.io/badge/Windows-10%2B-blue)
![Linux](https://img.shields.io/badge/Linux-supported-green)

## Description

QF_Player is a modern music player in Qt with mpv support. The app offers:

### 🎵 Main Features
- **Audio Playback**: Supports MP3, WAV, FLAC, and other formats
- **Playlist Management**: Add, Remove, and Switch Tracks
- **Playback Mode**: Normal and Shuffle
- **Volume Control**: Smoothly adjust from 0 to 100%
- **State Saving**: Automatically saves playlists and settings

### ⌨️ System Integration
- **Windows Media Keys**: Supports keyboard and headset buttons
- **Global Hotkeys**: Work even when the window is out of focus
- **OSD Notifications**: Visual feedback when pressing media keys
- **Autoload**: Restores the last playlist on startup

## Screenshot
<img width="659" height="832" alt="image" src="https://github.com/user-attachments/assets/7fa7e5b9-0736-42c8-8894-665916434e30" />

## 🚀 Quick Start

### Requirements
- **Windows 10+** or **Linux** (Ubuntu 20.04+, Arch Linux)
- **Qt 6.x** with modules: Widgets, Multimedia, Concurrent
- **CMake 3.16+**
- **mpv** (automatic installation via scripts)

### Build

1. **Clone the repository:**
```bash
git clone https://github.com/yourname/TextRedactor.git
cd TextRedactor
```

2. **Install mpv dependencies:**

**Windows (PowerShell):** 
```powershell 
.\scripts\fetch_mpv_win.ps1 
``` 

**Linux (Ubuntu/Debian):** 
```bash 
chmod +x scripts/fetch_mpv_linux.sh 
./scripts/fetch_mpv_linux.sh 
``` 

**Linux (Arch/Manjaro):** 
```bash 
chmod +x scripts/fetch_mpv_arch.sh 
./scripts/fetch_mpv_arch.sh 
```

3. **Assemble the project:** 
```bash 
mkdir build && cd build 
cmake .. -G "Ninja Multi-Config" -DCMAKE_BUILD_TYPE=Release 
cmake --build . --config Release
```

4. **Run:**
```bash
# Windows
./TextRedactor.exe

# Linux
./TextRedactor
```

## 🎮 Controls

### Keyboard and Mouse
- **Add Tracks**: "Add" button
- **Playback**: Play/Pause button
- **Switch Tracks**: "Next"/"Previous" buttons
- **Volume**: Volume slider
- **Random Mode**: "Random" toggle

### System Media Keys (Windows)
- **Play/Pause**: `VK_MEDIA_PLAY_PAUSE` (Fn+F11, headset button)
- **Next Track**: `VK_MEDIA_NEXT_TRACK` (Fn+F12)
- **Previous track**: `VK_MEDIA_PREV_TRACK` (Fn+F10)
- **Stop**: `VK_MEDIA_STOP` (Fn+F9)

> 💡 **Tip**: Media keys work globally, even when the window is out of focus!

## 📁 Project Structure

```
TextRedactor/
├── src/
│ ├── model/ # Data Models
│ │ ├── track.h/cpp # Track Class
│ │ └── ...
│ ├── view/ # User Interface
│ │ ├── mainwindow.h/cpp # Main Window
│ │ └── mainwindow.ui # UI File
│ ├── controller/ # Business Logic
│ │ ├── playercontroller.h/cpp # Player Controller
│ │ └── durationcontroller.h/cpp # Getting track durations
│ ├── player/ # Player (mpv)
│ │ └── player.h/cpp
│ └── integration/ # System integration
│ └── mediaosd.h/cpp # OSD notifications
├── mpv/ # mpv libraries and headers
├── resources/ # Resources (images, icons)
├── scripts/ # Dependency installation scripts
├── tests/ # Unit tests
├── CMakeLists.txt
└── README.md
```

## 🧪 Testing

The project includes unit tests for all major components:

### Running Tests

```bash
# After Building the Project
cd build/tests

# Windows
./test_player.exe
./test_playercontroller.exe
./test_track.exe

# Linux
./test_player
./test_playercontroller
./test_track
```

### Test Coverage
- ✅ **Player**: Play, Pause, Volume, Position
- ✅ **PlayerController**: Playlist Management, Track Switching
- ✅ **Track**: Track Creation and Validation

## 🔧 Technical Details

### Architecture
- **MVC Pattern**: Clear Model-View-Controller Separation
- **Qt Signals/Slots**: Asynchronous Communication between Components
- **Multithreading**: Track duration retrieval in a separate thread
- **System Integration**: Global Windows hotkeys

### Performance
- **Thread Safety**: Correct work with Qt threads
- **Memory**: Automatic management via smart pointers
- **mpv Integration**: Efficient playback via libmpv

### mpv Installation Scripts

| Distribution | Script | Installation Method |
|-------------|--------|-----------------|
| **Windows** | `fetch_mpv_win.ps1` | Downloading pre-built DLLs from mpv-winbuild |
| **Ubuntu/Debian** | `fetch_mpv_linux.sh` | Installation via `apt-get` (libmpv-dev) |
| **Arch/Manjaro** | `fetch_mpv_arch.sh` | Us
