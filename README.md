# QmClient

QmClient is a graphical tool to browse and edit the configuration of Pirelli/ADB routers.

It talks to the cm daemon of the CPE over SSH,
so there is no need to recompile cm or to relay it manually.

<p align="center"><img src="https://github.com/user-attachments/assets/0c486235-6e3d-4e5e-8ac0-7dad8143ee66" alt="QmClient main window" width="677"></p>

## Build

Requirements: CMake 3.19 or newer, Qt 6 (Widgets, Network) and libssh2.

- Debian/Ubuntu: `apt install cmake qt6-base-dev libssh2-1-dev`
- Fedora: `dnf install cmake qt6-qtbase-devel libssh2-devel`

```
cmake -B build
cmake --build build
```

A Debian package can be built with `dpkg-buildpackage -b`.

## License

Copyright (C) 2015 Matteo Croce

QmClient is released under the GNU General Public License version 3 or later, see [COPYING](COPYING).
