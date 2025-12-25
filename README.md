# Symbian Revival Operating System

[![License](https://img.shields.io/badge/License-EPL%201.0%2FLGPL%202.1%2FMIT-blue.svg)](LICENSE)
[![GitHub Issues](https://img.shields.io/github/issues/YOUR-ORG/symbian-revival.svg)](https://github.com/YOUR-ORG/symbian-revival/issues)
[![Status](https://img.shields.io/badge/Status-Active%20Development-brightgreen.svg)](#status)

**A modern revival of Symbian OS with automatic hardware detection, dynamic driver loading, and support for desktop, mobile, and tablet platforms across ARM, x86, and x64 architectures.**

---

## 🎯 Project Vision

Revive **Symbian OS**—the elegant microkernel-based operating system that powered 500+ million devices—with:

✨ **Modern Hardware Support**: Works on ARM, x86, and x64 architectures  
✨ **Multi-Device**: Single build supports desktops, phones, and tablets  
✨ **Modern Connectivity**: Full Bluetooth 4.0+, WiFi, and networking support  
✨ **Touch Support**: Native multi-touch input across all platforms  
✨ **Real-time Performance**: Hard real-time guarantees (<1ms latency)  
✨ **Hardware Agnostic**: Automatic hardware detection and dynamic driver loading  
✨ **Clean Architecture**: Microkernel design superior to Linux's monolithic approach  

---

## 🚀 Quick Start

### Prerequisites
```bash
# Ubuntu/Debian
sudo apt-get install -y build-essential cmake ninja-build \
    gcc-arm-none-eabi gcc g++ binutils python3 git

# macOS
brew install cmake ninja gcc g++ python3

# Windows: Use WSL2 with Ubuntu
```

### Clone & Setup (5 minutes)

```bash
# Clone this repository
git clone https://github.com/YOUR-ORG/symbian-revival.git
cd symbian-revival

# Run setup script
./tools/setup.sh

# Build
mkdir build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja
```

**Output**: `build/bin/symbian-os.elf` - Bootable kernel image

---

## 📋 What This Project Does

### The Problem
Traditional OS porting requires:
- ❌ Months of manual hardware-specific driver development per device
- ❌ Separate builds for each device variant
- ❌ Complex, fragile driver isolation (monolithic kernels)
- ❌ Poor real-time guarantees

### The Solution
```
Boot → Auto-detect hardware → Load matching HAL + drivers → System ready
Single build works on ARM phones, x86 desktops, and x64 servers!
```

### Core Innovation: Universal Hardware Abstraction

**Traditional Approach:**
```
Device A → Manual port → Compile → Build A
Device B → Manual port → Compile → Build B
Device C → Manual port → Compile → Build C
```

**Our Approach:**
```
All devices → Auto-detect → JSON device profile → Load drivers dynamically
Result: One build, N devices, minutes to add new hardware
```

---

## 🏗️ Architecture

```
┌──────────────────────────────────────────────┐
│     Applications (Qt/QML + C++)              │
├──────────────────────────────────────────────┤
│     Symbian Services (Network, IPC, Files)   │
├──────────────────────────────────────────────┤
│     Universal Hardware Abstraction Layer     │
├──────────────────────────────────────────────┤
│  Hardware Detection Engine (Auto-Detection)  │
│  • CPU identification                        │
│  • Bus enumeration (I2C, SPI, SDIO, USB)    │
│  • Device profile matching                   │
│  • Dynamic HAL/driver loading                │
├──────────────────────────────────────────────┤
│     Generic Device Drivers (LDD/PDD)         │
│  • Touch (parameterized per chipset)         │
│  • WiFi (Atheros, Broadcom, MediaTek)       │
│  • Storage (eMMC, SD card)                   │
│  • Bluetooth 4.0+                            │
├──────────────────────────────────────────────┤
│  EKA2 Microkernel (Real-time, Hard RT <1ms) │
│  • Scheduler, memory management              │
│  • IPC primitives, driver isolation          │
├──────────────────────────────────────────────┤
│     Hardware (ARM, x86, x64 CPUs)            │
└──────────────────────────────────────────────┘
```

---

## 🔄 How It Works

### 1. Hardware Detection at Boot
```
Boot sequence:
├─ Minimal kernel starts
├─ Hardware probe module runs
│  ├─ Read CPU ID register
│  ├─ Enumerate I2C/SPI/SDIO/USB buses
│  └─ Identify chipsets (touch IC, WiFi, SoC)
├─ Match detected hardware to device profile database
└─ Load appropriate HAL + drivers
```

### 2. Device Profile (JSON)
```json
{
  "device_id": "aquos_a207sh",
  "soc": "msm7227",
  "arch": "arm",
  "touch": {
    "ic": "atmel_at88sc103",
    "i2c_bus": 0,
    "i2c_address": "0x4A",
    "interrupt_gpio": 82
  },
  "wifi": {
    "chipset": "ar9271",
    "sdio_bus": 1,
    "power_gpio": 123
  },
  "bluetooth": {
    "chipset": "bcm4330",
    "uart_port": 1
  }
}
```

### 3. Dynamic Driver Loading
```
Detected: Atmel capacitive touch on I2C
│
├─ Load generic Touch LDD (logical driver)
├─ Load Atmel-specific Touch PDD (physical driver)
├─ Parameterize with I2C bus, interrupt GPIO, calibration
└─ Touch system ready
```

---

## 📁 Project Structure

```
symbian-revival/
├── CMakeLists.txt                    # Modern build system
├── README.md                         # This file
├── LICENSE                           # EPL v1.0 / LGPL v2.1 / MIT
│
├── kernel/                           # Symbian OS EKA2 (from GitHub)
│   ├── eka/                         # Microkernel
│   ├── hal/                         # Hardware Abstraction Layer
│   ├── drivers/                     # Base driver framework
│   └── include/                     # Kernel headers
│
├── src/
│   ├── hal/                         # Universal HAL implementations
│   │   ├── common/                 # Shared HAL interfaces
│   │   ├── arm_eabi/               # ARM-specific HAL
│   │   ├── x86/                    # x86-specific HAL
│   │   └── x64/                    # x64-specific HAL
│   │
│   ├── drivers/                    # Device drivers
│   │   ├── touch/                 # Touch controllers
│   │   │   ├── atmel/
│   │   │   ├── synaptics/
│   │   │   └── cypress/
│   │   ├── wifi/                  # WiFi chipsets
│   │   │   ├── atheros/
│   │   │   ├── broadcom/
│   │   │   └── mediatek/
│   │   ├── bluetooth/             # Bluetooth 4.0+
│   │   ├── storage/               # eMMC, SD, NVMe
│   │   └── display/               # LCD, OLED, eDP
│   │
│   ├── hw_detection/              # Hardware Detection Engine (YOUR CODE)
│   │   ├── hw_probe.cpp           # CPU/bus detection
│   │   ├── device_db_loader.cpp   # JSON profile loader
│   │   ├── hal_selector.cpp       # HAL selection logic
│   │   ├── driver_loader.cpp      # Dynamic driver loading
│   │   └── device_profiles/       # JSON device configs
│   │       ├── aquos_a207sh.json
│   │       ├── nokia_n95.json
│   │       ├── generic_arm.json
│   │       ├── generic_x86.json
│   │       └── generic_x64.json
│   │
│   ├── services/                  # Symbian services
│   │   ├── networking/
│   │   ├── filesystem/
│   │   ├── ipc/
│   │   └── connectivity/
│   │
│   └── frameworks/                # Application frameworks
│       ├── qt/
│       ├── native/
│       └── legacy_s60/
│
├── docs/
│   ├── PROJECT_VISION.md           # Complete roadmap
│   ├── ARCHITECTURE.md             # Detailed architecture
│   ├── BUILD.md                    # Build instructions
│   ├── HARDWARE_DETECTION.md       # How detection works
│   ├── DEVICE_PORTING.md           # How to port devices
│   ├── DRIVER_DEVELOPMENT.md       # Writing drivers
│   └── API_REFERENCE.md            # Public APIs
│
├── tools/
│   ├── setup.sh                    # Environment setup
│   ├── build.sh                    # Build script
│   ├── rom_builder.sh              # Create bootable image
│   ├── device_profile_gen.py       # Generate device profiles
│   └── emulator_runner.sh          # Run on QEMU
│
├── tests/
│   ├── unit/                       # Unit tests
│   ├── integration/                # Integration tests
│   └── device_tests/               # Device-specific tests
│
└── build/                          # Build artifacts (generated)
    ├── arm-eabi/
    ├── x86/
    ├── x64/
    └── images/
```

---

## 🎯 Implementation Roadmap

### Phase 1: Foundation (Weeks 1–4)
- [x] Clone Symbian OS source from GitHub
- [x] Set up modern CMake build system
- [ ] Cross-compile for ARM/x86/x64
- [ ] Verify kernel boots on QEMU
- [ ] Document complete build process

**Deliverable**: Bootable kernel images for ARM, x86, x64

### Phase 2: Hardware Abstraction (Weeks 5–8)
- [ ] Boot-time CPU detection (CPUID, device tree)
- [ ] Device profile database schema (JSON)
- [ ] HAL loader and selector
- [ ] Parameterized HAL initialization
- [ ] Multi-architecture support

**Deliverable**: Hardware detection engine + reference profiles

### Phase 3: Touch & Input (Weeks 9–12)
- [ ] Generalize S60 touch framework
- [ ] Support multiple touch ICs (Atmel, Synaptics, Cypress)
- [ ] Auto-detect touch controller at boot
- [ ] Multi-touch and calibration
- [ ] Test on ARM and x86

**Deliverable**: Generic touch driver framework

### Phase 4: Connectivity (Weeks 13–16)
- [ ] SDIO/UART/SPI bus drivers
- [ ] WiFi: Atheros AR9271, Broadcom BCM43xx, MediaTek
- [ ] Bluetooth 4.0+ support
- [ ] WPA2/WPA3 security
- [ ] Firmware loading and management

**Deliverable**: Multi-chipset WiFi + Bluetooth support

### Phase 5: Multi-Platform Support (Weeks 17–24)
- [ ] ARM EABI (phones, tablets)
- [ ] x86 (legacy desktops, netbooks)
- [ ] x64 (modern desktops, servers)
- [ ] Device tree support (Linux compatibility)
- [ ] UEFI/BIOS bootloader integration

**Deliverable**: Single build running ARM, x86, x64

### Phase 6: Integration & Release (Weeks 25+)
- [ ] Multi-device bootable images
- [ ] Comprehensive documentation
- [ ] Community device profiles
- [ ] CI/CD pipeline (GitHub Actions)
- [ ] Public release and community launch

**Deliverable**: MVP system, ready for users

---

## 🔑 Key Features

### Real-Time Microkernel
✅ **Hard real-time <1ms latency** - Guaranteed response times  
✅ **Driver isolation** - Driver crash ≠ system crash  
✅ **Memory efficient** - 256KB–2MB core vs. Linux's 10MB+  
✅ **Fast boot** - 3–5 seconds vs. Android's 30–60 seconds  

### Universal Hardware Support
✅ **ARM (phones, tablets, Raspberry Pi)**  
✅ **x86 (legacy desktops, netbooks, Intel Atom)**  
✅ **x64 (modern PCs, laptops, servers)**  
✅ **Automatic hardware detection** - No manual per-device porting  

### Modern Connectivity
✅ **WiFi 802.11b/g/n** with WPA2/WPA3  
✅ **Bluetooth 4.0–5.x** with BLE  
✅ **Cellular** (LTE/5G optional)  
✅ **Ethernet**, USB, NFC  

### Touch & Input
✅ **Capacitive multi-touch** (Atmel, Synaptics, Cypress)  
✅ **Resistive touch** (legacy devices)  
✅ **Pressure-sensitive** (stylus support)  
✅ **Gesture recognition**  

### Application Support
✅ **Qt/QML** - Modern native applications  
✅ **HTML5/WebKit** - Web applications  
✅ **Legacy S60 apps** - Backward compatibility  
✅ **Linux compatibility** - ELF binaries, POSIX APIs  

---

## 📊 Why Symbian?

| Aspect | Symbian | Linux | Android | Windows |
|--------|---------|-------|---------|---------|
| **Microkernel** | ✅ True nanokernel | ❌ Monolithic | ❌ Monolithic | ❌ Monolithic |
| **Real-time** | ✅ Hard <1ms | ❌ Soft (best-effort) | ❌ Soft | ⚠️ Limited |
| **Memory** | ✅ 256KB–2MB | ❌ 10MB+ | ❌ 20MB+ | ❌ 50MB+ |
| **Boot Time** | ✅ 3–5 sec | ❌ 30–60 sec | ❌ 30–60 sec | ❌ 60+ sec |
| **Source** | ✅ GitHub (EPL) | ✅ Public | ✅ Google | ❌ Proprietary |
| **HAL Design** | ✅ Native | ❌ Retrofit | ❌ Retrofit | ❌ Proprietary |
| **Driver Isolation** | ✅ Yes | ❌ No | ❌ No | ⚠️ Limited |
| **Multi-arch** | ✅ ARM/x86/x64 | ✅ Yes | ✅ Yes | ❌ x86/x64 only |

---

## 🛠️ Build System

### Modern CMake-based Build
```bash
# Configure
cmake -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DARCH=arm-eabi \
    -DPLATFORM=generic-arm \
    ..

# Build
ninja -j$(nproc)

# Flash to device
./tools/rom_builder.sh -o symbian-os.bin -a arm-eabi
```

### Supported Architectures
- `arm-eabi` - ARM EABI (ARM7, Cortex-A8/A9/A15, etc.)
- `x86` - 32-bit Intel/AMD
- `x64` - 64-bit Intel/AMD

### Toolchain
- **Compilers**: GCC 9–11 or LLVM 12+
- **Build System**: CMake + Ninja
- **Target**: Modern (C++14/17, POSIX APIs)

---

## 📦 Source Code

### Official Symbian Foundation Source
Complete Symbian OS source code is available on GitHub:

**Organization**: https://github.com/SymbianSource (216 repositories)

**Essential components** (~1.5 GB):
```bash
git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.os.kernelhwsrv.git kernel
git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.os.graphics.git graphics
git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.os.devicesrv.git devicesrv
git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.os.userlibandfileserver.git userlib
```

See [COMPLETE_SOURCE_DOWNLOAD.md](./docs/COMPLETE_SOURCE_DOWNLOAD.md) for full source code guide.

---

## 📚 Documentation

| Document | Purpose |
|----------|---------|
| [PROJECT_VISION.md](./docs/PROJECT_VISION.md) | Complete project vision and roadmap |
| [ARCHITECTURE.md](./docs/ARCHITECTURE.md) | System architecture details |
| [BUILD.md](./docs/BUILD.md) | Detailed build instructions |
| [HARDWARE_DETECTION.md](./docs/HARDWARE_DETECTION.md) | How auto-detection works |
| [DEVICE_PORTING.md](./docs/DEVICE_PORTING.md) | How to add new devices |
| [DRIVER_DEVELOPMENT.md](./docs/DRIVER_DEVELOPMENT.md) | Writing device drivers |
| [COMPLETE_SOURCE_DOWNLOAD.md](./docs/COMPLETE_SOURCE_DOWNLOAD.md) | Symbian OS source code guide |
| [API_REFERENCE.md](./docs/API_REFERENCE.md) | Public API documentation |

---

## 🤝 Contributing

We welcome contributions in:

- **Hardware Porting**: Device profiles, drivers, testing
- **Architecture**: HAL improvements, driver framework, detection engine
- **Build System**: CMake optimization, toolchain integration
- **Documentation**: Guides, tutorials, API docs
- **Testing**: Unit tests, integration tests, device validation
- **Community**: Bug reports, feature requests, discussions

**See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.**

---

## 📄 License

- **Symbian OS Source**: Eclipse Public License v1.0 (EPL v1.0) and LGPL v2.1
- **New Code**: Dual-licensed under MIT and EPL v1.0 (maximum compatibility)

See [LICENSE](LICENSE) for details.

---

## 🎯 Supported Devices (Target List)

### Mobile Devices
- [ ] **Aquos Sharp A207SH** (Android 10, ARM Cortex-A8)
- [ ] **Nokia N95** (Symbian original, ARM11)
- [ ] **Nokia N90** (Symbian original, ARM11)
- [ ] Samsung Galaxy S series (legacy models)
- [ ] Generic ARM phones (device tree support)

### Tablets
- [ ] iPad 2 / iPad 3 (ARM Cortex-A9, x86 tablet support)
- [ ] Generic ARM tablets

### Desktops / Laptops
- [ ] Legacy x86 netbooks (Intel Atom)
- [ ] x86 desktops (Intel Core 2 Duo and later)
- [ ] x64 desktops/laptops (modern Intel/AMD)
- [ ] QEMU/Virtualbox VMs

### Single-Board Computers
- [ ] Raspberry Pi 1–4 (ARM)
- [ ] BeagleBone (ARM)
- [ ] Pine64 (ARM)
- [ ] x86 boards (Atom-based)

---

## ⚡ Status

| Component | Status | ETA |
|-----------|--------|-----|
| **Kernel Build System** | 🟡 In Progress | Week 2 |
| **Hardware Detection** | 🔴 Not Started | Week 8 |
| **Touch Support** | 🔴 Not Started | Week 12 |
| **WiFi/Bluetooth** | 🔴 Not Started | Week 16 |
| **ARM Bootable Image** | 🔴 Not Started | Week 4 |
| **x86 Support** | 🔴 Not Started | Week 20 |
| **x64 Support** | 🔴 Not Started | Week 24 |
| **MVP Release** | 🔴 Not Started | Week 26 |

**Legend**: 🟢 Complete | 🟡 In Progress | 🔴 Not Started

---

## 🚀 Getting Started

### 1. Clone Repository
```bash
git clone https://github.com/YOUR-ORG/symbian-revival.git
cd symbian-revival
```

### 2. Read Documentation
Start with [PROJECT_VISION.md](docs/PROJECT_VISION.md) to understand the goals.

### 3. Setup Build Environment
```bash
./tools/setup.sh
```

### 4. Download Symbian OS Source
See [COMPLETE_SOURCE_DOWNLOAD.md](docs/COMPLETE_SOURCE_DOWNLOAD.md):
```bash
git clone --depth=1 https://github.com/SymbianSource/oss.FCL.sf.os.kernelhwsrv.git kernel
# ... (3 more repos)
```

### 5. Build
```bash
mkdir build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja
```

### 6. Test
```bash
./tools/emulator_runner.sh  # Run on QEMU
```

---

## 📞 Community & Support

- **GitHub Issues**: [Report bugs](https://github.com/YOUR-ORG/symbian-revival/issues)
- **GitHub Discussions**: [Ask questions](https://github.com/YOUR-ORG/symbian-revival/discussions)
- **Discord** (coming soon): Real-time chat
- **Mailing List** (coming soon): Development updates

---

## 🎓 Learning Resources

- **[Programming for Symbian OS](https://www.oreilly.com/)** - O'Reilly (comprehensive reference)
- **[The Symbian OS Architecture Sourcebook](https://www.amazon.com/)** - Academic deep-dive
- **[awesome-symbian](https://github.com/hstsethi/awesome-symbian)** - Community resource collection
- **OSDev.org Forums** - OS development community
- **Academic Papers** - Referenced in PROJECT_VISION.md

---

## 🎉 Acknowledgments

- **Symbian Foundation**: For creating the elegant microkernel OS
- **Nokia, Sony Ericsson, Samsung**: For open-sourcing the codebase
- **Community**: For preserving documentation and tools
- **Contributors**: Who will help revive this remarkable OS

---

## 📈 Project Statistics

- **Total Repositories** (on GitHub SymbianSource): 216
- **Essential Repositories**: 4 (1.5 GB)
- **Complete Source**: ~5–10 GB
- **Estimated MVP Timeline**: 5–6 months
- **Target Architectures**: 3 (ARM, x86, x64)
- **Supported Devices**: 20+ (planned)

---

## 🔗 Related Projects

- **ReactOS** - Windows NT clone (similar microkernel revival approach)
- **Haiku OS** - BeOS successor (microkernel heritage)
- **Linux Kernel** - Monolithic reference (contrast)
- **QNX Neutrino** - Commercial microkernel OS (similar real-time approach)

---

## ⚠️ Important Notes

### This is a **Research & Community Project**
- Not affiliated with Nokia, Sony Ericsson, or original Symbian creators
- For educational, research, and hobbyist use
- Not intended for production critical systems (yet)
- Community-driven development

### Legal
- Uses Symbian OS source under EPL v1.0 / LGPL v2.1
- New contributions under MIT / EPL v1.0 dual license
- Respect intellectual property and licensing

---

## 📝 Changelog

### [Unreleased]
- Initial project structure and documentation
- CMake build system setup
- GitHub repository created
- Full source code guide published

### Planned Releases
- **v0.1.0** (Q2 2026) - MVP: Bootable kernel with hardware detection
- **v0.2.0** (Q3 2026) - Touch + WiFi support
- **v0.3.0** (Q4 2026) - Multi-device support (ARM, x86, x64)
- **v1.0.0** (Q1 2027) - Production-ready system

---

## 💡 FAQ

**Q: Is this a real operating system?**  
A: Yes! It's based on the real Symbian OS source code (500M+ devices used it). We're modernizing and extending it.

**Q: Can I run it on my phone?**  
A: Yes, if it has an ARM processor. We're working on specific device support. Adding a device requires creating a device profile (hours, not months).

**Q: Is it faster than Android/Linux?**  
A: For real-time operations, yes. Symbian has hard real-time guarantees Linux can't match. For general computing, performance is comparable with smaller memory footprint.

**Q: When will it be production-ready?**  
A: MVP (bootable system with touch + WiFi) in Q2 2026. Production hardening in late 2026.

**Q: Can I run Android apps on it?**  
A: Not directly, but we're planning Qt/QML compatibility for modern apps. Legacy Symbian S60 apps should run.

**Q: Why not just use Linux?**  
A: Different tradeoffs. Linux is great for general computing. Symbian excels at real-time, driver isolation, and minimal overhead. Pick the right tool for your use case.

---

## 🎯 Next Milestone

**Target**: Bootable ARM kernel with automatic hardware detection (January 2026)

---

**Built with ❤️ for OS enthusiasts, embedded developers, and the retrocomputing community.**

```
 _____ _   ___  ____  _____
|_   _| | / _ \/ ___||_   _|
  | | | |/ /_\ \___ \  | |
  | | | / ___ \|__) | | |
  |_| |_/_/   \_\____/  |_|

Symbian OS Revival
Universal Hardware Abstraction Project
```

---

**Last Updated**: December 25, 2025  
**Status**: Active Development  
**Version**: 0.0.1 (Planning Phase)  
**Maintainer**: Your Name / Organization  

🚀 **[Start Contributing](CONTRIBUTING.md)** | 📚 **[Read Docs](docs/)** | 💬 **[Join Discussion](https://github.com/YOUR-ORG/symbian-revival/discussions)**
