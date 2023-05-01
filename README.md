# endless-sky-thrower
Endless Sky Launcher made with GTKMM

The name is a joke, since it's, you know, a _launcher_

# Features
- Multiple installations
    - Continuous builds (Fresh from the releases section)
    - Stable builds (If you don't want to install a potentially unstable release)
    - Custom builds (Just point to the executable and voilà)
    - PRs (Write the PR number and ESThrower will compile and setup it for you)
- These installations can have optional requirements:
    - Vanilla (Won't get polluted with plugins)
    - Autoupdate (Only available for Continuous instances, automatically downloads the build everytime the launcher is opened)

- Plugin installer and uninstaller
- Changelog viewer

# Compilation
Install the dependencies: 
- Arch: ```yay -S curl gtkmm3 git-extras meson libarchive cmake```
- Ubuntu/Debian: ```sudo apt install libgkmm-3.0-dev curl git-extras meson libarchive-dev cmake```

Then, just run: 
```
git clone https://github.com/DisableGraphics/endless-sky-thrower
cd endless-sky-thrower
meson setup builddir
meson compile -C builddir
```

# Wanted:
Someone who can help me port this to macOS

# Copyright
- This program is licensed under the [GNU General Public License v3.0](https://www.gnu.org/licenses/gpl-3.0.en.html). <br>
- The icon is made by Michael Zahniser and is licensed under the [CC-BY-SA-4.0](https://creativecommons.org/licenses/by-sa/4.0/legalcode) license. <br>
- [nlohmann's json library](https://github.com/nlohmann/json), the library I'm using for JSON parsing is licensed under the terms of the  [MIT License](https://mit-license.org/)
- Libarchive is licensed under the [New BSD License](https://raw.githubusercontent.com/libarchive/libarchive/master/COPYING)
