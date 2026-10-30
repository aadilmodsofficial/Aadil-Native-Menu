# Aadil Native Menu

**A native Android Mod Menu template by Aadil Mods.**

Aadil Native Menu is a floating mod menu template designed for Android game modding. It combines Java, JNI and C++ to provide a native menu interface with support for IL2CPP and other native Android games.

Developed and maintained by **Aadil Mods**.

## Introduction

Aadil Native Menu provides a foundation for developing and integrating custom mod menus into Android games.

It is designed to work with **Aadil Virtual**, but it can also be used independently in standalone mod menu projects or when injecting a native library directly into a game's process.

### Features

- Floating mod menu interface
- Java and JNI integration
- Native C++ functionality
- IL2CPP and native game support
- ARM64 architecture support
- Memory patching using KittyMemory
- Function hooking support
- Hexadecimal offset support
- String obfuscation
- Embedded DEX support through C++ headers
- Customizable menu elements and controls
- Support for standalone APK and Aadil Virtual environments
- Ability to load the native menu without loading the embedded DEX

## Compatibility

The project currently targets:

- **Android:** 7.0 and above
- **Architecture:** ARM64 (64-bit)
- **Game engines:** IL2CPP and other native Android games

### ARM32 Support

Although this project currently supports ARM64 only, you can add ARM32 support by making a few changes:

1. Add the ARM32 Keystone static library.
2. Add an ARM32-compatible hooking library.
3. Include `armeabi-v7a` in `Application.mk`.
4. Make sure all native dependencies support ARM32.

### Supporting Older Android Versions

The current `Application.mk` uses `android-24` as the minimum API level.

You can lower this value to target older Android versions. However, make sure your code, NDK configuration and all prebuilt native libraries support the selected API level.

Compatibility may vary depending on the game, Android version, architecture and native libraries.

## Getting Started

You can customize the menu, add your own features and integrate it into supported Android games.

The project source code is organized into folders for native libraries, hooking, memory patching and menu implementation. Explore the project files to understand how everything is connected.

For build instructions and integration details, refer to the sections below.

## Different Ways to Use This Project

### 1. Standalone Mod Menu APK

You can build the project as a standalone mod menu APK and load the menu directly from the APK.

Set the following variable in `Main.cpp`:

```cpp
bool LOAD_FROM_APK = true;
```

### 2. Aadil Virtual Integration

This project is compatible with **Aadil Virtual**. You can build the native library and launch the menu directly when running a supported game inside Aadil Virtual.

Set the following variable in `Main.cpp`:

```cpp
bool LOAD_FROM_APK = false;
```

Make sure this setting is correct before building. Using the wrong setting for your launch environment can cause the game to crash.

Learn more about Aadil Virtual development:

https://app.aadilmods.com/developer

### 3. Native Library Injection

This project can also be useful when injecting a native library directly into a game's process.

If you only want to load the native mod menu without loading the embedded DEX, you can do that as well.

You need to hook the appropriate JNI function in the game that provides access to its `Context` and `Activity`. Once you obtain these objects, you can use the game's own context and activity to initialize and display the menu.

This approach allows you to load the native menu directly inside the game without depending on the embedded DEX.

**Note:** The JNI function and initialization method may vary depending on the game and its implementation.

## How to Build

### Windows (Android Studio)

1. Install and open Android Studio.
2. Clone this repository using Git:

   ```bash
   git clone https://github.com/aadilmodsofficial/Aadil-Native-Menu.git
   ```

   Alternatively, download the project ZIP from the repository's Releases section and extract it.

3. Open the extracted or cloned project in Android Studio.
4. Allow Android Studio to sync the project.
5. Since this project was originally developed using Android AIDE, you may need to upgrade Gradle, the Android Gradle Plugin and other project configurations to make it compatible with your Android Studio environment.
6. Build the project.

### Android (AIDE)

You can also build the project directly on your Android device.

1. Install **AIDE Cmods (Pro or Plus)**.
2. Download the project ZIP from the Releases section.
3. Extract the ZIP file.
4. Open the extracted project in AIDE.
5. Rebuild the project to generate a clean build.

**Note:** Build compatibility may depend on your AIDE version and installed NDK/toolchain.

## Aadil Virtual

[Aadil Virtual](https://app.aadilmods.com) is another Android project developed by Aadil Mods.

It provides a virtual environment for running Android applications and games without requiring root access.

Aadil Native Menu and Aadil Virtual are separate projects within the Aadil Mods ecosystem, but this menu template is designed to work with Aadil Virtual.

For developer information and integration instructions, visit:

https://app.aadilmods.com/developer

## Need Help?

If you encounter issues during development or integration, check the documentation and review the relevant implementation before reporting a problem.

For updates, announcements and support, join our community:

- **Telegram:** https://t.me/aadilmodsofficial
- **Discord:** https://discord.gg/4X8Hhgwc5A
- **Website:** https://aadilmods.com
- **GitHub:** https://github.com/aadilmodsofficial

## Credits

This project includes or builds upon work from the following developers and projects:

- **Octowolve / Escanor** - Substrate Mod Menu and Hooking examples
- **VanHoevenTR** - Android Mod Menu
- **MrIkso** - Floating Mod Menu
- **MJx0 (Ruit)** - KittyMemory
- **Rprop** - And64InlineHook
- **LGL Team** - Original Mod Menu template and contributions
- **All other developers** who contributed code, ideas and improvements.

Full credit goes to the original authors of the respective libraries and implementations.

## License

This project is licensed under the **GNU General Public License v3.0 (GPL-3.0)**.

See the `LICENSE` file for details.

## Disclaimer

This project is intended for educational and development purposes only.

Aadil Mods does not encourage cheating, exploiting or gaining unfair advantages in online multiplayer games. Developers are responsible for how they use and distribute their modifications.

Do not purchase unofficial copies or source codes from unauthorized sellers. This project is intended to remain freely available under its applicable license.

---

**Aadil Mods**

Android Development | Native Modding | Reverse Engineering

Website: https://aadilmods.com
