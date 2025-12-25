# Complete Symbian OS Source Code: Download & Setup Guide

**Last Updated**: December 25, 2025  
**Status**: Full source code locations and download methods documented

---

## 🎯 Quick Summary

Symbian OS source code is hosted on **GitHub SymbianSource organization**: https://github.com/SymbianSource

**Total repositories**: 216 repositories  
**Total size**: ~5–10 GB for complete source  
**Essential repositories**: ~2 GB for bootable system  

---

## 📍 GitHub SymbianSource Organization

**Organization URL**: https://github.com/SymbianSource

This organization contains **216 repositories** from the defunct Symbian Foundation. All repositories are:
- ✅ Public (no authentication required)
- ✅ Under EPL v1.0 / LGPL v2.1 licenses
- ✅ Maintained as historical archive
- ✅ Fully cloneable via Git

---

## 🔑 Critical Core Repositories

These are the **essential repositories** you MUST clone to build a bootable Symbian OS system:

### 1. **oss.FCL.sf.os.kernelhwsrv** (Highest Priority)
**URL**: https://github.com/SymbianSource/oss.FCL.sf.os.kernelhwsrv.git  
**Size**: ~500 MB  
**Content**:
- EKA2 Microkernel (complete source)
- Hardware Abstraction Layer (HAL)
- Hardware-specific drivers and extensions
- Memory management and IPC primitives
- Device driver framework (LDD/PDD)

**Must-have**: YES - This is the OS core

```bash
git clone https://github.com/SymbianSource/oss.FCL.sf.os.kernelhwsrv.git kernel
```

---

### 2. **oss.FCL.sf.os.graphics** (Graphics & UI)
**URL**: https://github.com/SymbianSource/oss.FCL.sf.os.graphics.git  
**Size**: ~300 MB  
**Content**:
- Display drivers and rendering
- Window server
- Drawing API (BitGDI, OpenVG)
- OpenGL ES support
- Touch input handling

**Must-have**: YES - Needed for UI and touch

```bash
git clone https://github.com/SymbianSource/oss.FCL.sf.os.graphics.git graphics
```

---

### 3. **oss.FCL.sf.os.deviceplatformrelease** (S60 UI Framework)
**URL**: https://github.com/SymbianSource/oss.FCL.sf.os.deviceplatformrelease.git  
**Size**: ~1 GB  
**Content**:
- S60 Framework (Series 60)
- User interface components
- Application launcher
- Settings and control panels
- Qt/QML integration
- System UI

**Must-have**: RECOMMENDED - For functional user interface

```bash
git clone https://github.com/SymbianSource/oss.FCL.sf.os.deviceplatformrelease.git s60
```

---

### 4. **oss.FCL.sf.os.devicesrv** (Device Services)
**URL**: https://github.com/SymbianSource/oss.FCL.sf.os.devicesrv.git  
**Size**: ~150 MB  
**Content**:
- Device drivers infrastructure
- Hardware control services
- Sensor management
- Power management
- LED and vibration control

**Must-have**: YES - For hardware device access

```bash
git clone https://github.com/SymbianSource/oss.FCL.sf.os.devicesrv.git devicesrv
```

---

### 5. **oss.FCL.sf.os.worldserver** (Services Framework)
**URL**: https://github.com/SymbianSource/oss.FCL.sf.os.worldserver.git  
**Size**: ~200 MB  
**Content**:
- System services
- Networking infrastructure
- IPC and messaging
- Service management

**Must-have**: OPTIONAL - For advanced features

```bash
git clone https://github.com/SymbianSource/oss.FCL.sf.os.worldserver.git worldserver
```

---

### 6. **oss.FCL.sftools.dev.build** (Build System)
**URL**: https://github.com/SymbianSource/oss.FCL.sftools.dev.build.git  
**Size**: ~200 MB  
**Content**:
- Symbian build tools (Raptor, OBake)
- Build configuration system
- Compilation scripts
- Legacy build infrastructure

**Must-have**: OPTIONAL - We're replacing with CMake

```bash
git clone https://github.com/SymbianSource/oss.FCL.sftools.dev.build.git build-tools
```

---

## 📦 Complete Repository List (216 repositories)

### Categories by Function

#### **Core Operating System** (9 repos)
- `oss.FCL.sf.os.kernelhwsrv` — Kernel + HAL
- `oss.FCL.sf.os.graphics` — Graphics system
- `oss.FCL.sf.os.devicesrv` — Device services
- `oss.FCL.sf.os.userlibandfileserver` — File system
- `oss.FCL.sf.os.bluetooth` — Bluetooth stack
- `oss.FCL.sf.os.cellularsrv` — Cellular/Phone services
- `oss.FCL.sf.os.commsfw` — Communications framework
- `oss.FCL.sf.os.mw.sensorfw` — Sensor framework
- `oss.FCL.sf.os.worldserver` — System services

#### **UI & Applications** (8 repos)
- `oss.FCL.sf.os.deviceplatformrelease` — S60 UI framework
- `oss.FCL.sf.mw.qt` — Qt/QML framework
- `oss.FCL.sf.mw.appframework` — Application framework
- `oss.FCL.sf.mw.homescreenfw` — Home screen
- `oss.FCL.sf.mw.web` — Web browser (WebKit)
- `oss.FCL.sf.app.organizer` — Calendar/Organizer
- `oss.FCL.sf.app.messaging` — Messaging apps
- `oss.FCL.sf.app.videotelephony` — Video calling

#### **Build System & Tools** (15 repos)
- `oss.FCL.sftools.dev.build` — Build tools
- `oss.FCL.sftools.fbf.*` — Build framework
- `oss.FCL.sftools.dev.env` — Development environment
- `oss.FCL.sftools.dev.eclipseenv` — Eclipse integration
- `oss.FCL.sftools.dev.ux` — Development tools UI

#### **Libraries & Frameworks** (20+ repos)
- `oss.FCL.sf.mw.qt` — Qt libraries
- `oss.FCL.sf.mw.platfw` — Platform framework
- `oss.FCL.sf.mw.securitysrv` — Security services
- `oss.FCL.sf.mw.legacyplugins` — Legacy plugins
- Various protocol implementations (HTTP, SSL, etc.)

#### **Documentation & Configuration** (50+ repos)
- Package descriptions
- Platform descriptions
- Build configurations
- Testing frameworks
- Documentation source

---

## 🚀 Download Strategies

### **Option A: Essential Components Only** (Recommended - Fast Setup)
**Total size**: ~1.5 GB  
**Time**: 15–30 minutes  
**Use case**: Quick start, focus on core functionality

```bash
#!/bin/bash
# Create project directory
mkdir -p ~/symbian-os-full
cd ~/symbian-os-full

# Clone essential repositories
echo "Cloning essential Symbian OS components..."

git clone https://github.com/SymbianSource/oss.FCL.sf.os.kernelhwsrv.git kernel
git clone https://github.com/SymbianSource/oss.FCL.sf.os.graphics.git graphics
git clone https://github.com/SymbianSource/oss.FCL.sf.os.devicesrv.git devicesrv
git clone https://github.com/SymbianSource/oss.FCL.sf.os.userlibandfileserver.git userlib

echo "Essential components cloned!"
du -sh .
```

---

### **Option B: Complete UI + Services** (Balanced - 3–4 hours)
**Total size**: ~3 GB  
**Time**: 1–2 hours (depending on connection)  
**Use case**: Full-featured system with UI

```bash
#!/bin/bash
mkdir -p ~/symbian-os-full
cd ~/symbian-os-full

echo "Cloning complete Symbian OS source..."

# Core OS
git clone https://github.com/SymbianSource/oss.FCL.sf.os.kernelhwsrv.git kernel
git clone https://github.com/SymbianSource/oss.FCL.sf.os.graphics.git graphics
git clone https://github.com/SymbianSource/oss.FCL.sf.os.devicesrv.git devicesrv
git clone https://github.com/SymbianSource/oss.FCL.sf.os.userlibandfileserver.git userlib

# UI & Services
git clone https://github.com/SymbianSource/oss.FCL.sf.os.deviceplatformrelease.git s60
git clone https://github.com/SymbianSource/oss.FCL.sf.os.worldserver.git worldserver
git clone https://github.com/SymbianSource/oss.FCL.sf.mw.qt.git qt

# Communications
git clone https://github.com/SymbianSource/oss.FCL.sf.os.commsfw.git commsfw
git clone https://github.com/SymbianSource/oss.FCL.sf.os.cellularsrv.git cellularsrv

# Applications
git clone https://github.com/SymbianSource/oss.FCL.sf.mw.appframework.git appframework

echo "Complete UI + Services source cloned!"
du -sh .
```

---

### **Option C: Complete Source Tree** (Full - 5–10 hours)
**Total size**: ~5–10 GB  
**Time**: 2–4 hours (depending on connection)  
**Use case**: Complete historical archive, maximum compatibility

```bash
#!/bin/bash
mkdir -p ~/symbian-os-full
cd ~/symbian-os-full

echo "Downloading complete Symbian OS source tree (216 repositories)..."

# Use Python to clone all repositories from SymbianSource organization
python3 << 'PYTHONEOF'
import subprocess
import json
import urllib.request
import time

org = "SymbianSource"
per_page = 100
page = 1
cloned = 0

while True:
    url = f"https://api.github.com/orgs/{org}/repos?per_page={per_page}&page={page}"
    
    print(f"Fetching page {page}...")
    
    try:
        with urllib.request.urlopen(url) as response:
            repos = json.loads(response.read().decode())
            
            if not repos:
                break
                
            for repo in repos:
                clone_url = repo['clone_url']
                repo_name = repo['name']
                
                print(f"[{cloned+1}] Cloning {repo_name}...")
                subprocess.run(['git', 'clone', '--depth=1', clone_url, repo_name])
                cloned += 1
                time.sleep(0.5)  # Be nice to GitHub
        
        page += 1
        
    except Exception as e:
        print(f"Error: {e}")
        break

print(f"\nComplete! Cloned {cloned} repositories.")
PYTHONEOF

echo "All 216 repositories cloned!"
du -sh .
```

---

## 📋 Directory Structure After Cloning

### After Option A (Essential):
```
symbian-os-full/
├── kernel/                    # EKA2 Microkernel
├── graphics/                  # Graphics subsystem
├── devicesrv/                 # Device services
└── userlib/                   # User libraries & file server
```

### After Option B (Complete UI):
```
symbian-os-full/
├── kernel/
├── graphics/
├── devicesrv/
├── userlib/
├── s60/                       # S60 UI framework
├── worldserver/               # System services
├── qt/                        # Qt framework
├── commsfw/                   # Communications
├── cellularsrv/               # Cellular/Phone services
└── appframework/              # Application framework
```

### After Option C (Full):
```
symbian-os-full/
├── [All 216 repositories organized by category]
├── kernel/
├── graphics/
├── os.*/                      # OS components
├── mw.*/                      # Middleware
├── app.*/                     # Applications
├── sftools.*/                 # Build tools
└── [Various configuration & documentation repos]
```

---

## 🔍 Finding Repositories by Category

### Quick Lookup Table

| Function | Repository | URL |
|----------|-----------|-----|
| **Kernel** | `oss.FCL.sf.os.kernelhwsrv` | github.com/SymbianSource/oss.FCL.sf.os.kernelhwsrv |
| **Graphics** | `oss.FCL.sf.os.graphics` | github.com/SymbianSource/oss.FCL.sf.os.graphics |
| **File System** | `oss.FCL.sf.os.userlibandfileserver` | github.com/SymbianSource/oss.FCL.sf.os.userlibandfileserver |
| **UI Framework** | `oss.FCL.sf.os.deviceplatformrelease` | github.com/SymbianSource/oss.FCL.sf.os.deviceplatformrelease |
| **Bluetooth** | `oss.FCL.sf.os.bluetooth` | github.com/SymbianSource/oss.FCL.sf.os.bluetooth |
| **Cellular** | `oss.FCL.sf.os.cellularsrv` | github.com/SymbianSource/oss.FCL.sf.os.cellularsrv |
| **Communications** | `oss.FCL.sf.os.commsfw` | github.com/SymbianSource/oss.FCL.sf.os.commsfw |
| **Security** | `oss.FCL.sf.mw.securitysrv` | github.com/SymbianSource/oss.FCL.sf.mw.securitysrv |
| **Qt Framework** | `oss.FCL.sf.mw.qt` | github.com/SymbianSource/oss.FCL.sf.mw.qt |
| **Web** | `oss.FCL.sf.mw.web` | github.com/SymbianSource/oss.FCL.sf.mw.web |
| **Build Tools** | `oss.FCL.sftools.dev.build` | github.com/SymbianSource/oss.FCL.sftools.dev.build |

---

## 💾 Space Requirements

| Option | Size | Time | Recommended For |
|--------|------|------|-----------------|
| **A - Essential** | 1.5 GB | 15–30 min | Quick start, core dev |
| **B - Complete UI** | 3 GB | 45–90 min | Full-featured system |
| **C - All 216 repos** | 5–10 GB | 2–4 hours | Complete archive |

**Note**: Add 50–100% more space for build artifacts, binaries, and object files.

---

## ⚡ Fast Clone Tips

### Use `--depth=1` to Clone Faster
```bash
git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.os.kernelhwsrv.git
```
**Pros**: 10x faster, minimal disk usage  
**Cons**: No git history (fine for viewing source)

### Use Parallel Cloning
```bash
# Clone 4 repositories in parallel
repos=(
    "oss.FCL.sf.os.kernelhwsrv"
    "oss.FCL.sf.os.graphics"
    "oss.FCL.sf.os.devicesrv"
    "oss.FCL.sf.os.userlibandfileserver"
)

for repo in "${repos[@]}"; do
    git clone --depth=1 "https://github.com/SymbianSource/$repo.git" &
done
wait
```

### Resume Interrupted Downloads
```bash
# Git automatically resumes if you run the same command again
git clone https://github.com/SymbianSource/oss.FCL.sf.os.kernelhwsrv.git
# If interrupted, run again and it will continue
```

---

## ✅ Verify Source Code After Cloning

### Check Directory Structure
```bash
# Should show many subdirectories
ls -la kernel/
# Expected: kernel/, hal/, drivers/, include/, ...

# Check file count
find . -type f | wc -l
# Essential: ~20,000+ files

# Check total size
du -sh .
```

### Verify Git Repositories
```bash
# Verify each repository
for repo in kernel graphics devicesrv userlib s60; do
    echo "Checking $repo..."
    cd $repo
    git log --oneline | head -5
    cd ..
done
```

---

## 📖 Understanding the Source Structure

### Kernel Repository Structure
```
kernel/
├── kernel/                    # EKA2 nanokernel source
│   ├── eka/                   # EPOC kernel
│   ├── memmodel/              # Memory models
│   └── drivers/               # Kernel drivers
├── hal/                       # Hardware Abstraction Layer
│   ├── common/
│   ├── win32/
│   └── [platform-specific]/
├── epoc/                      # EPOC layer
├── e32/                       # Symbian core libraries
└── [additional components]/
```

### Graphics Repository Structure
```
graphics/
├── graphicsdeviceinterface/   # GDI (Graphics Device Interface)
├── openvg/                    # OpenVG graphics
├── opengl/                    # OpenGL ES
├── windowing/                 # Window server
├── libnvgrender/              # NV GPU rendering
└── [additional components]/
```

---

## 🔗 All 216 Repository Links

To browse all 216 repositories directly:

**Web Interface**: https://github.com/SymbianSource?tab=repositories

Use the search/filter on GitHub to find specific repositories:
- Filter by name: `oss.FCL.sf.os.*` (core OS)
- Filter by name: `oss.FCL.sf.mw.*` (middleware)
- Filter by name: `oss.FCL.sf.app.*` (applications)
- Filter by name: `oss.FCL.sftools.*` (development tools)

---

## 📝 Clone Script - Ready to Use

Save this as `clone_symbian.sh`:

```bash
#!/bin/bash

# Symbian OS Source Clone Script
# Downloads Symbian OS source code from GitHub SymbianSource

set -e

SYMBIAN_DIR="${1:-.}"
OPTION="${2:-a}"  # a=essential, b=ui, c=full

mkdir -p "$SYMBIAN_DIR"
cd "$SYMBIAN_DIR"

echo "=== Symbian OS Source Code Downloader ==="
echo "Directory: $SYMBIAN_DIR"
echo "Option: $OPTION (a=essential, b=ui, c=full)"
echo ""

case "$OPTION" in
    a)
        echo "Downloading essential components (1.5 GB)..."
        git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.os.kernelhwsrv.git kernel
        git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.os.graphics.git graphics
        git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.os.devicesrv.git devicesrv
        git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.os.userlibandfileserver.git userlib
        ;;
    b)
        echo "Downloading complete UI + services (3 GB)..."
        # All from option A plus:
        git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.os.kernelhwsrv.git kernel
        git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.os.graphics.git graphics
        git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.os.devicesrv.git devicesrv
        git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.os.userlibandfileserver.git userlib
        git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.os.deviceplatformrelease.git s60
        git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.os.worldserver.git worldserver
        git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.mw.qt.git qt
        git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.os.commsfw.git commsfw
        git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.os.cellularsrv.git cellularsrv
        ;;
    c)
        echo "Downloading all 216 repositories (5–10 GB)..."
        python3 << 'PYEOF'
import subprocess, json, urllib.request, time

org = "SymbianSource"
page = 1
cloned = 0

while True:
    url = f"https://api.github.com/orgs/{org}/repos?per_page=100&page={page}"
    try:
        with urllib.request.urlopen(url) as r:
            repos = json.loads(r.read().decode())
            if not repos: break
            for repo in repos:
                print(f"[{cloned+1}] {repo['name']}...")
                subprocess.run(['git', 'clone', '--depth=1', repo['clone_url']])
                cloned += 1
                time.sleep(0.5)
        page += 1
    except Exception as e:
        print(f"Error: {e}"); break
print(f"\nCloned {cloned} repositories!")
PYEOF
        ;;
    *)
        echo "Usage: $0 [directory] [a|b|c]"
        echo "  a = essential (1.5 GB)"
        echo "  b = complete UI (3 GB)"
        echo "  c = all 216 repos (5–10 GB)"
        exit 1
        ;;
esac

echo ""
echo "Download complete!"
du -sh .
```

**Usage**:
```bash
chmod +x clone_symbian.sh

# Download essential (fastest)
./clone_symbian.sh ~/symbian-os a

# Download with UI
./clone_symbian.sh ~/symbian-os b

# Download everything
./clone_symbian.sh ~/symbian-os c
```

---

## 🎯 Recommended Starting Point

### For Most Users: Option A (Essential)
```bash
mkdir -p ~/symbian-revival
cd ~/symbian-revival

git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.os.kernelhwsrv.git kernel
git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.os.graphics.git graphics
git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.os.devicesrv.git devicesrv
git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.os.userlibandfileserver.git userlib

# Verify
du -sh .
ls -la
```

**Time**: 15–30 minutes  
**Size**: ~1.5 GB  
**Result**: Ready to build bootable kernel with hardware abstraction

---

## 🚨 If Download is Slow

### Option 1: Try Again Later
GitHub has rate limits. Wait 1 hour and retry.

### Option 2: Use `--depth=1` Flag
Already recommended above—saves 70% bandwidth.

### Option 3: Download as ZIP (GitHub Web)
```
https://github.com/SymbianSource/oss.FCL.sf.os.kernelhwsrv/archive/master.zip
```
(One repository at a time, slower overall but sometimes works when git is slow)

### Option 4: Resume Interrupted Clone
```bash
# If interrupted, just run the same command again
git clone https://github.com/SymbianSource/oss.FCL.sf.os.kernelhwsrv.git kernel
# Git will resume from where it left off
```

---

## ✅ Verification Checklist

After cloning, verify:

- [ ] All target directories exist (`kernel/`, `graphics/`, etc.)
- [ ] Each directory contains `.git/` folder (valid git repo)
- [ ] Total size matches expectation (Option A: ~1.5GB, etc.)
- [ ] Can run `git log` in each directory
- [ ] File count is reasonable (~20,000+ files for Option A)

---

## 📚 Next Steps After Downloading Source

1. **Review the source**: `ls -la kernel/` to understand structure
2. **Read documentation**: Check kernel/doc/ and graphics/doc/ for guides
3. **Follow GETTING_STARTED.md**: Set up modern build system
4. **Configure CMakeLists.txt**: Point to cloned repositories
5. **Begin building**: Start with kernel compilation

---

## 📞 Troubleshooting

### "Clone timeout" or "Connection reset"
```bash
# Retry with depth=1 for faster clone
git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.os.kernelhwsrv.git
```

### "Permission denied (publickey)"
```bash
# Use HTTPS instead of SSH
git clone https://github.com/SymbianSource/oss.FCL.sf.os.kernelhwsrv.git
# SSH is not needed (no authentication required)
```

### "Certificate verification failed"
```bash
# Temporary workaround (not recommended for production)
git config --global http.sslVerify false
```

### Out of disk space
```bash
# Check available space
df -h

# Use --depth=1 to reduce size
git clone --depth=1 [url]

# Or download only what you need (Option A)
```

---

## 🎉 You're Ready!

Once you've downloaded the source using Option A:

1. You have **complete Symbian OS kernel source**
2. You have **graphics and device services**
3. You have **~1.5 GB of high-quality code to study**
4. You can **begin implementing your hardware detection layer**

**Next**: Follow GETTING_STARTED.md to set up the build system.

---

**Complete Guide Created**: December 25, 2025  
**Total Repositories**: 216 available (4 essential recommended)  
**Estimated Time**: 15–30 minutes (Option A)  
**Status**: Ready to download and build!
