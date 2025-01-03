# NX Homebrew Lister
WIP nintendo switch app to list all installed homebrew software.

Until first stable release, you have to build the project to get .nro files. 

## Todo

* [x] List all apps in /switch/*
* [ ] Display app info such as name, author, version, filename from .nro file
* [ ] On start up page to select between displaying apps / overlays / sysmodules
* [ ] List all overlays in /switch/.overlays/*
* [ ] Display overlay info from .ovl file.
* [ ] List all sysmodules from /atmosphere/content/42* or 00*

## Building
devkitA64 must be already installed with all tools and libraries necessary for switch development.

For a detailed guide on setting up development environment, see [switchbrew](https://switchbrew.org/wiki/Setting_up_Development_Environment).

### Build

```
git clone https://github.com/Xc987/NX-Homebrew-Lister
cd NX-Homebrew-Lister
make
```

### Deploy

```
nxlink -a [SWITCH IP] NX-Homebrew-Lister.nro
```

### Clean

```
make clean
```
