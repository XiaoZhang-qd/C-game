# CGame Racer

[![Build Status](https://github.com/YOUR_USERNAME/YOUR_REPO/actions/workflows/build.yml/badge.svg)](https://github.com/YOUR_USERNAME/YOUR_REPO/actions/workflows/build.yml)
[![Release Status](https://github.com/YOUR_USERNAME/YOUR_REPO/actions/workflows/release.yml/badge.svg)](https://github.com/YOUR_USERNAME/YOUR_REPO/actions/workflows/release.yml)

一个基于 TCP 协议的多人联网竞速/战斗小游戏，使用 **C99** 编写，支持 **几乎所有操作系统**。

玩家可以在 2000×1500 像素的 2D 世界里自由驰骋，放置炸弹攻击对手，使用加速和护盾技能争夺胜利。

---

## 目录

- [功能特性](#功能特性)
- [技术栈](#技术栈)
- [项目结构](#项目结构)
- [平台支持](#平台支持)
- [快速开始](#快速开始)
- [编译方法](#编译方法)
- [使用方法](#使用方法)
- [操作说明](#操作说明)
- [游戏机制](#游戏机制)
- [网络协议](#网络协议)
- [FAQ](#faq)
- [许可证](#许可证)

---

## 功能特性

- 🌐 **多人联网** — 基于 TCP 的客户端-服务端架构，支持最多 32 人同场竞技
- 🚗 **实时竞速** — 流畅的 2D 赛车操作，支持键盘和触屏/鼠标控制
- 💣 **炸弹系统** — 玩家可放置炸弹攻击对手，有爆炸特效和伤害判定
- ⚡ **双技能系统**
  - 技能 1（加速）— 短时间内大幅提升移动速度，带光晕特效
  - 技能 2（护盾）— 短时间内免疫炸弹伤害，带护盾光环
- ❤️ **血量/重生** — 初始 100 HP，死亡后 3 秒在随机位置满血重生
- 🗺️ **大地图** — 2000×1500 像素的开放式世界，摄像机跟随玩家
- ⚙️ **服务端权威配置** — 冷却时间、最大人数等均由服务端统一管理
- 📡 **全量同步** — 玩家状态、炸弹放置、技能特效全部实时同步
- 🎨 **GUI 菜单** — 带名字输入、服务器地址输入的可视化连接界面
- 🖥️ **跨平台** — 支持桌面、移动、Web 等 **15+ 操作系统/平台**

---

## 技术栈

| 类别 | 技术 | 说明 |
|------|------|------|
| 语言 | C99 | 无任何 C++ 依赖 |
| 构建 | CMake + Ninja/Make | 跨平台构建系统 |
| 游戏引擎 | [raylib](https://www.raylib.com/) | 已包含在 `raylib/` 目录中 |
| GUI 库 | [raygui](https://github.com/raysan5/raygui) | 已包含在 `raygui-5.0/` 目录中 |
| 网络 | 原生 POSIX / Winsock2 / Emscripten Socket | 无第三方网络库 |
| 协议 | 自定义二进制协议 | 固定长度头部 + 数据体 |

---

## 项目结构

```
c-game/
├── CMakeLists.txt              # CMake 主构建配置（含架构检测）
├── CMakePresets.json           # 多平台/多架构 CMake Presets
├── cmake/                      # 交叉编译工具链文件
│   ├── toolchain-linux-x86.cmake
│   ├── toolchain-linux-arm64.cmake
│   ├── toolchain-linux-arm32.cmake
│   ├── toolchain-linux-riscv64.cmake
│   ├── toolchain-linux-riscv32.cmake
│   ├── toolchain-linux-ppc64.cmake
│   ├── toolchain-linux-ppc64le.cmake
│   ├── toolchain-linux-ppc32.cmake
│   ├── toolchain-linux-s390x.cmake
│   ├── toolchain-linux-mips64.cmake
│   ├── toolchain-linux-mips64le.cmake
│   ├── toolchain-linux-mips32.cmake
│   ├── toolchain-linux-mips32le.cmake
│   ├── toolchain-linux-sparc64.cmake
│   ├── toolchain-windows-x64.cmake
│   ├── toolchain-windows-x86.cmake
│   ├── toolchain-windows-arm64.cmake
│   └── toolchain-macos-universal.cmake
├── build.bat                   # Windows 一键编译（CMD，支持 [arch] 参数）
├── build.ps1                   # 跨平台编译（PowerShell，支持 [arch] 参数）
├── build.sh                    # Linux/macOS 编译（Bash，支持 [arch] 参数）
├── build-wasm.sh / .bat / .ps1 # WebAssembly 编译
├── build-android.sh / .bat / .ps1 # Android NDK 编译
├── build-ios.sh                # iOS 编译（自动识别 macOS/Theos）
├── build-ios-theos.sh          # iOS 交叉编译 + IPA 打包（非 macOS）
├── build-harmonyos.sh          # HarmonyOS 编译
├── build-chromeos.sh           # ChromeOS 编译
├── build-all.sh / .ps1         # 全平台构建调度器
├── makefile                    # 基础 Makefile（macOS 原生）
├── packet.h                    # 旧协议头文件（保留）
│
├── common/                     # 公共模块
│   ├── config.h                # 全局常量
│   ├── platform.h              # 平台 + 架构检测（15+ 平台, 10+ 架构）
│   ├── protocol.h              # 网络协议定义
│   ├── socket.h                # 跨平台 Socket 接口
│   └── socket.c                # Socket 实现
│
├── server/                     # 服务端
│   ├── main.c                  # 入口 & 命令行参数
│   ├── server.h                # 数据结构声明
│   └── server.c                # 主循环、包转发
│
├── client/                     # 客户端
│   ├── main.c                  # 入口 & 主循环
│   ├── game.h / game.c         # 游戏逻辑、渲染
│   ├── gui.h / gui.c           # 菜单 GUI
│   ├── network.h / network.c   # 网络收发
│   └── raygui_impl.c           # raygui 依赖
│
├── raylib/                     # raylib 游戏引擎（已包含）
│   └── src/libraylib.a         # 静态库（已预编译）
│
└── raygui-5.0/                 # raygui GUI 库（已包含）
    └── src/raygui.h
```

---

## 平台支持

### 完整平台列表

| 平台 | 支持的 CPU 架构 | 状态 | 构建方式 | 备注 |
|------|----------------|------|----------|------|
| **Windows** 10/11 | x86, x64, ARM64 | ✅ | `build.bat [arch]` / `build.ps1 [arch]` | MSVC / MinGW / Clang |
| **Linux** 所有发行版 | x86, x64, ARM32, ARM64, RISC-V 32/64, PPC 32/64, s390x, MIPS 32/64, SPARC64 | ✅ | `build.sh [arch]` / `build.ps1 [arch]` | 原生或交叉编译 |
| **macOS** 10.13+ | x64, ARM64 (Apple Silicon), Universal 2 | ✅ | `build.sh [arch]` / `build.ps1 [arch]` | 支持 Intel/Apple Silicon/通用 |
| **FreeBSD** 12+ | x64 | ✅ | `cmake --preset freebsd-x64` | POSIX 兼容 |
| **OpenBSD** 7+ | x64 | ✅ | `cmake --preset openbsd-x64` | POSIX 兼容 |
| **NetBSD** 9+ | x64 | ✅ | `cmake --preset netbsd-x64` | POSIX 兼容 |
| **DragonFly BSD** | x64 | ✅ | `cmake --preset dragonfly-x64` | POSIX 兼容 |
| **Android** 7.0+ (API 24+) | ARM64, ARMv7, x86, x86_64 | ✅ | `build-android.sh` / `.ps1` | NDK 编译 |
| **iOS** 13+ | ARM64 (设备), ARM64 (模拟器) | ✅ | `build-ios.sh` / `build-ios-theos.sh` | Xcode 或 Theos SDKs |
| **HarmonyOS** NEXT | ARM64 | ✅ | `build-harmonyos.sh` | OpenHarmony Native Toolkit |
| **ChromeOS** | x64, ARM64 | ✅ | `build-chromeos.sh` / WASM | Crostini 或浏览器 |
| **Web** (浏览器) | WASM | ✅ | `build-wasm.sh` / `.bat` / `.ps1` | Chrome/Firefox/Safari/Edge |
| **Linux DRM** (嵌入式) | ARM64, ARM32, x64 | ✅ | `cmake -DCGAME_USE_DRM=ON` | Raspberry Pi 等 |

### CPU 架构支持矩阵

| 架构 | 32/64 位 | 端序 | Linux | Windows | macOS | BSD | Android | iOS | 工具链 / Preset |
|------|---------|------|-------|---------|-------|-----|---------|-----|-----------------|
| x86 (i386) | 32 | LE | ✅ | ✅ | — | ✅ | ✅ | — | `linux-x86.cmake` / `windows-x86.cmake` / `android-x86.cmake` |
| x64 (AMD64) | 64 | LE | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ (sim) | `linux-x64.cmake` / `windows-x64.cmake` / `macos-x64.cmake` / `android-x86_64.cmake` |
| ARM32 (armv7l) | 32 | LE | ✅ | ✅ | — | — | ✅ | — | `linux-arm32.cmake` / `windows-arm64.cmake` / `android-armv7.cmake` |
| ARM64 (aarch64) | 64 | LE | ✅ | ✅ | ✅ | — | ✅ | ✅ | `linux-arm64.cmake` / `windows-arm64.cmake` / `macos-arm64.cmake` / `android-arm64.cmake` / `ios-arm64.cmake` |
| ARM64-simulator | 64 | LE | — | — | — | — | — | ✅ | `ios-arm64-simulator.cmake` |
| x86_64-simulator | 64 | LE | — | — | — | — | — | ✅ | `ios-arm64-simulator.cmake` (x86_64 目标) |
| RISC-V 64 | 64 | LE | ✅ | — | — | — | — | — | `linux-riscv64.cmake` |
| MIPS 64 | 64 | BE/LE | ✅ | — | — | — | — | — | `linux-mips64.cmake` |
| WASM (Emscripten) | 32 | LE | ✅ | ✅ | ✅ | — | — | — | `wasm.cmake` / `web-emscripten` preset |

### 架构组

| 分组 | 包含架构 | 特征 |
|------|---------|------|
| x86 家族 | x86, x64, x86_64-simulator | CISC, 可变长度指令, 主要桌面/服务器 |
| ARM 家族 | ARM32, ARM64, ARM64-simulator | RISC, 固定长度指令, 移动端/嵌入式 |
| RISC-V 家族 | RISC-V 64 | 开源 ISA, 科研/嵌入式 |
| MIPS 家族 | MIPS 64 | 经典 RISC, 嵌入式/网络设备 |
| WASM | WASM32 | 虚拟 ISA, 沙箱执行 |

### CMake Presets 快速编译

所有平台的编译配置已通过 `CMakePresets.json` 预设好：

```bash
# ===== 桌面平台（自动检测原生架构） =====
cmake --preset windows-x64          # Windows x64
cmake --preset windows-x86          # Windows x86 (32-bit)
cmake --preset windows-arm64        # Windows ARM64
cmake --preset linux-x64            # Linux x64
cmake --preset linux-x86             # Linux x86 (32-bit)
cmake --preset macos-x64            # macOS x64 (Intel)
cmake --preset macos-arm64          # macOS ARM64 (Apple Silicon)
cmake --preset freebsd-x64          # FreeBSD x64
cmake --preset openbsd-x64          # OpenBSD x64
cmake --preset netbsd-x64           # NetBSD x64
cmake --preset dragonfly-x64        # DragonFly x64

# ===== Linux 交叉编译 =====
cmake --preset linux-arm64          # Linux → ARM64 (aarch64)
cmake --preset linux-arm32          # Linux → ARM32 (armv7l)
cmake --preset linux-riscv64        # Linux → RISC-V 64
cmake --preset linux-mips64          # Linux → MIPS 64
cmake --preset linux-drm             # Linux DRM/KMS (Raspberry Pi)
cmake --preset linux-chromeos       # Linux → ChromeOS (Crostini)

# ===== ChromeOS =====
cmake --preset chromeos-x64          # ChromeOS x64

# ===== 移动/嵌入式平台 =====
cmake --preset android-arm64        # Android ARM64
cmake --preset android-armv7        # Android ARMv7
cmake --preset android-x86          # Android x86
cmake --preset android-x86_64      # Android x86_64
cmake --preset ios-arm64           # iOS 真机 (Xcode)
cmake --preset ios-arm64-theos     # iOS 真机 (Theos 交叉编译)
cmake --preset ios-arm64-simulator  # iOS 模拟器 (Xcode)
cmake --preset ios-arm64-sim-theos # iOS 模拟器 (Theos)
cmake --preset harmonyos-arm64      # HarmonyOS

# ===== WebAssembly =====
cmake --preset web-emscripten       # WebAssembly / WASM

# ===== 编译 =====
cmake --build --preset <preset-name>
```

### 平台对应关系

```
┌─────────────────────────────────────────────────────────────────────┐
│                        用户的设备                                   │
├──────────────┬──────────────────────────────────────────────────────┤
│ Windows PC   │ → build.bat / build.ps1 / CMake Preset              │
│ Linux PC     │ → build.sh / build.ps1 / CMake Preset               │
│ Mac          │ → build.sh / build.ps1 / CMake Preset               │
│ BSD 系统     │ → build.sh / CMake Preset                           │
│ Android 手机 │ → build-android.sh / build-android.ps1              │
│ iPhone/iPad  │ → build-ios.sh (macOS) 或 build-ios-theos.sh (跨平台) │
│ HarmonyOS 设备│ → build-harmonyos.sh                                │
│ Chromebook   │ → build-chromeos.sh (原生) 或 build-wasm.sh (浏览器) │
│ 任意设备浏览器│ → build-wasm.sh / build-wasm.bat → 打开 HTML 文件  │
│ Raspberry Pi │ → cmake -DCGAME_USE_DRM=ON                           │
└──────────────┴──────────────────────────────────────────────────────┘
```

---

## 快速开始

### 桌面平台（Windows / Linux / macOS / BSD）

**Windows:**
```cmd
build.bat
build.bat x86        rem 32-bit x86
build.bat arm64      rem ARM64 (交叉编译)
```

**Linux / macOS / BSD:**
```bash
chmod +x build.sh
./build.sh              # 原生架构
./build.sh arm64        # 交叉编译 ARM64
./build.sh riscv64      # 交叉编译 RISC-V 64
./build.sh ppc64        # 交叉编译 PowerPC 64
./build.sh s390x        # 交叉编译 IBM Z
# 支持: x64, x86, arm64, arm32, riscv64, riscv32, ppc64, ppc64le, ppc32, s390x, mips64, mips64le, mips32, mips32le, sparc64
```

**PowerShell（全平台）：**
```powershell
pwsh -File build.ps1              # 原生架构
pwsh -File build.ps1 arm64        # ARM64 交叉编译
pwsh -File build.ps1 riscv64      # RISC-V 64 交叉编译
```

### WebAssembly（任意设备浏览器）

```bash
# 需要先安装 Emscripten (https://emscripten.org)
./build-wasm.sh

# 然后本地启动服务器
python3 -m http.server 8000 -d build-web
# 打开 http://localhost:8000/c-game-client-web.html
```

### Android

```bash
# 需要 Android NDK (https://developer.android.com/ndk)
export ANDROID_NDK_HOME=/path/to/ndk
./build-android.sh arm64-v8a 24
# 将生成的 .so 文件打包到 APK 中
```

### iOS

**方式一：macOS + Xcode（原生）**
```bash
./build-ios.sh arm64 13.0
# 在 Xcode 中打开 build-ios/ 项目并部署到真机/模拟器
```

**方式二：跨平台编译（Linux/Windows → IPA）**

使用 [Theos SDKs](https://github.com/theos/sdks) 提供的补丁 SDK，在任意操作系统上交叉编译 iOS 应用并直接打包为 `.ipa`：

```bash
# 1. 克隆 Theos 补丁 SDKs
git clone https://github.com/theos/sdks.git ~/theos/sdks

# 2. 交叉编译并自动打包 IPA
./build-ios-theos.sh arm64 13.0 com.cgame.racer "CGame Racer"

# 生成的 IPA: build-ios-theos/c-game-client-arm64-ios13.ipa
```

> Theos SDKs 包含了从 Xcode 7.3 / iOS 9.3 起被移除的私有框架 `.tbd` 文件，使得在非 macOS 平台上也能完整编译 iOS 应用。生成的 `.ipa` 可通过 TrollStore、AltStore 等工具侧载安装。

### HarmonyOS

```bash
# 需要 HarmonyOS SDK (https://developer.harmonyos.com)
export HOS_SDK_HOME=/path/to/harmonyos-sdk
./build-harmonyos.sh arm64-v8a 12
```

### ChromeOS

```bash
# 原生方式（通过 Crostini/Linux）
./build-chromeos.sh native

# Web 方式（通过 Chrome 浏览器）
./build-chromeos.sh web

# 或同时构建两种
./build-chromeos.sh all
```

### 一键构建所有已安装平台

```bash
./build-all.sh       # Bash
pwsh -File build-all.ps1  # PowerShell
```

编译完成后执行：

```bash
# 启动服务端
./build/c-game-server

# 启动客户端（另开一个终端）
./build/c-game-client
```

---

## 编译方法

### 前置依赖

| 依赖 | 最低版本 | 说明 |
|------|----------|------|
| CMake | 3.15+ | 构建系统 |
| C 编译器 | C99 兼容 | GCC / Clang / MSVC 任选 |
| Ninja 或 Make | — | 构建工具（脚本自动检测） |
| raylib | — | 已包含在仓库中 |

### 各平台依赖安装

**Windows：**
```cmd
winget install CMake.CMake
winget install Ninja-build.Ninja
winget install MinGW-w64
rem 或安装 Visual Studio 2022 (MSVC)
```

**Ubuntu / Debian：**
```bash
sudo apt update
sudo apt install cmake ninja-build gcc build-essential
```

**Fedora：**
```bash
sudo dnf install cmake ninja-build gcc
```

**Arch Linux：**
```bash
sudo pacman -S cmake ninja gcc
```

**FreeBSD：**
```bash
sudo pkg install cmake ninja gcc
```

**macOS：**
```bash
xcode-select --install
brew install cmake ninja
```

**Android (NDK)：**
```bash
# 下载: https://developer.android.com/ndk/downloads
export ANDROID_NDK_HOME=/path/to/android-ndk
```

**iOS (macOS 原生)：**
```bash
xcode-select --install
brew install cmake
```

**iOS (Theos SDKs 跨平台)：**
```bash
# Linux / Windows / macOS 均可
git clone https://github.com/theos/sdks.git ~/theos/sdks
export THEOS=~/theos
# 或设置到 PATH
```

**HarmonyOS：**
```bash
# 下载: https://developer.harmonyos.com
export HOS_SDK_HOME=/path/to/harmonyos-sdk
```

**WebAssembly (Emscripten)：**
```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh    # Linux/macOS
# emsdk_env.bat         # Windows
```

### 手动编译

```bash
mkdir -p build && cd build
cmake -G Ninja ..
cmake --build .
```

编译产物：

| 文件 | 说明 |
|------|------|
| `c-game-server` (`.exe`) | 服务端 |
| `c-game-client` (`.exe`) | 客户端 |
| `c-game-client-web.html` | WebAssembly 客户端 |

---

## 使用方法

### 启动服务端

```bash
# 默认配置
./c-game-server

# 自定义配置
./c-game-server --port 25565 --max 16 --bomb 60 --skill1 300 --skill2 300

# 绑定特定 IP
./c-game-server --ip 127.0.0.1 --port 25565
```

**服务端命令行参数：**

| 参数 | 说明 | 默认值 |
|------|------|--------|
| `-ip` / `--ip` | 监听 IP 地址 | `0.0.0.0` |
| `-port` / `--port` | 监听端口 | `25565` |
| `-max` / `--max` | 最大玩家数 | `8` |
| `-bomb` / `--bomb` | 炸弹冷却（帧） | `30` |
| `-skill1` / `--skill1` | 技能 1 冷却（帧） | `300` |
| `-skill2` / `--skill2` | 技能 2 冷却（帧） | `300` |
| `-help` / `--help` / `-h` | 显示帮助 | — |

> ⚠️ **重要**：所有冷却参数**只能在服务端**修改，客户端无法更改。

### 启动客户端

```bash
./c-game-client
```

在菜单中输入昵称和服务器地址，点击 **CONNECT**。

---

## 操作说明

### 键盘

| 操作 | 按键 |
|------|------|
| 上移 | `W` / `↑` |
| 下移 | `S` / `↓` |
| 左移 | `A` / `←` |
| 右移 | `D` / `→` |
| 放置炸弹 | `Space` |
| 技能 1（加速） | `J` / `1` / `Numpad 1` |
| 技能 2（护盾） | `K` / `2` / `Numpad 2` |

### 触屏/鼠标

| 操作 | 控件 |
|------|------|
| 方向移动 | 左下虚拟方向键 ↑↓←→ |
| 放置炸弹 | 右下 BOMB 按钮 |
| 技能 1 | 右下按钮 `1` |
| 技能 2 | 右下按钮 `2` |
| 切换按钮 | 右上 `>>` / `<<` |

### 冷却显示

按钮上实时显示剩余冷却时间（秒）。

---

## 游戏机制

### 技能

| 技能 | 效果 | 持续 | 默认冷却 |
|------|------|------|----------|
| 加速 | 移动速度翻倍 | 2 秒 | 5 秒 |
| 护盾 | 免疫炸弹伤害 | 3 秒 | 5 秒 |

### 炸弹

- 放置后 **1.5 秒** 爆炸
- 爆炸半径 **60 像素**，造成 **40 点** 伤害
- 护盾保护不受伤害
- 最多同时 **16 颗** 炸弹
- 其他玩家的炸弹实时同步显示

### 死亡与重生

- 初始血量 **100 HP**
- 血量归零 → "YOU DIED"
- **3 秒** 后随机位置满血重生
- 重生后获得 **2 秒护盾**

---

## 网络协议

### 数据包类型

| ID | 名称 | 方向 | 说明 |
|----|------|------|------|
| 1 | `PKT_LOGIN` | C→S | 登录请求 |
| 2 | `PKT_PLAYER_UPDATE` | C→S→C | 玩家状态广播 |
| 3 | `PKT_PLAYER_LIST` | S→C | 玩家列表（预留） |
| 4 | `PKT_LOGIN_RESPONSE` | S→C | 登录结果 |
| 5 | `PKT_BOMB_EXPLOSION` | C→S→C | 炸弹爆炸通知 |
| 6 | `PKT_SERVER_CONFIG` | S→C | 服务端配置下发 |
| 7 | `PKT_BOMB_PLACED` | C→S→C | 炸弹放置通知 |

### 数据流

```
客户端 A                          服务端                           客户端 B
  │── PKT_LOGIN ──────────────>│                                │
  │<── PKT_LOGIN_RESPONSE ────│                                │
  │<── PKT_SERVER_CONFIG ─────│                                │
  │                              │                              │
  │── PKT_BOMB_PLACED ───────>│── PKT_BOMB_PLACED ──────────>│
  │                              │                              │
  │── PKT_BOMB_EXPLOSION ────>│── PKT_BOMB_EXPLOSION ──────>│
  │                              │                              │
  │<── PKT_PLAYER_UPDATE(B) ───│                              │
  │                              │── PKT_PLAYER_UPDATE(A) ────>│
```

---

## FAQ

**Q: 支持多少种操作系统？**
A: 代码层面支持 **15+ 平台**，覆盖桌面（Windows/Linux/macOS/BSD）、移动（Android/iOS/HarmonyOS）、Web（WASM）、嵌入式（DRM）等所有主流系统。

**Q: 编译时报错找不到 `libraylib.a`？**
A: 确认 `raylib/src/libraylib.a` 存在。`build.sh` 会自动尝试编译 raylib。

**Q: 客户端能修改冷却时间吗？**
A: **不能。** 所有冷却参数由服务端控制，客户端无法修改。这是为了游戏公平性。

**Q: 如何在 iPhone/iPad 上玩？**
A: 两种方式：
1. **macOS + Xcode**：使用 `build-ios.sh` 在 Mac 上编译，通过 Xcode 部署到真机
2. **跨平台编译（所有系统）**：使用 [Theos SDKs](https://github.com/theos/sdks) + `build-ios-theos.sh`，在 Linux/Windows/macOS 上交叉编译并自动打包为 `.ipa`，通过 TrollStore/AltStore 侧载安装

**Q: 如何在 Chromebook 上玩？**
A: 两种方式：
1. **Crostini**（Linux 容器）：使用 `build-chromeos.sh native` 编译原生版本
2. **浏览器**：使用 `build-wasm.sh` 编译 WASM 版本，在 Chrome 中打开

**Q: 如何在 HarmonyOS 设备上玩？**
A: 使用 `build-harmonyos.sh` 编译原生版本，打包为 HAP 安装。或使用 WASM 版本在浏览器中运行。

**Q: 多人游戏时炸弹看不到？**
A: 确认所有玩家连接到同一个服务端。炸弹放置需要服务端转发。

**Q: 画面卡顿怎么办？**
A: 检查网络延迟、服务器性能、显卡驱动。WASM 版本建议使用 Chrome/Edge 最新版。

---

## 许可证

本项目仅供学习和娱乐使用。使用、复制、修改时请注明来源。
