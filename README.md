# endless-sky-thrower
Endless Sky Launcher made with GTK

The name is a joke, since it's, you know, a _launcher_

# Features
- Multiple installations
    - Continuous builds (Fresh from the releases section)
    - Stable builds (If you don't want to install a potentially unstable release)
    - Custom builds (Just point to the executable and voilà)
    - PRs (Write the PR number and ESThrower will do everything for you)
- These installations can have optional requirements:
    - Vanilla (Won't get polluted with plugins)
    - Autoupdate (Only available for Continuous instances, automatically downloads the build everytime the launcher is opened)

- Plugin installer and uninstaller

# Compilation
Install the dependencies: 
- Arch: ```yay -S curl gtkmm3 git-extras meson```
- Ubuntu/Debian: ```sudo apt install libgkmm-3.0-dev curl git-extras meson```

Then, just run: 
```
git clone https://github.com/DisableGraphics/endless-sky-thrower
cd endless-sky-thrower
meson setup builddir
meson compile -C builddir
```
# Support
This application downloads and installs endless sky instances for you. Moreover, you can add custom instances that are already downloaded on your disk (For example: a PR, a content update...).
# Wanted:
Someone who can help me port this to macOS

# Copyright
This program is licensed under the GNU General Public License v3.0. You can find the full license [here](https://www.gnu.org/licenses/gpl-3.0.en.html). <br>
The icon is made by Michael Zahniser and is licensed under CC-BY-SA-4.0. You can find the full license [here](https://creativecommons.org/licenses/by-sa/4.0/legalcode). <br>
[nlohmann's json library](https://github.com/nlohmann/json), the library I'm using for JSON parsing [MIT License](https://mit-license.org/)
