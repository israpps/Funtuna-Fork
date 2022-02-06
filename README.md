
[![Logo](https://github.com/israpps/Funtuna-Fork/blob/main/logos%20%26%20others/github%20logo.png "Funtuna logo")](https://github.com/israpps/Funtuna-Fork/releases)
***

[![Downloads](https://img.shields.io/github/downloads/israpps/Funtuna-Fork/total.svg)](https://github.com/israpps/Funtuna-Fork/releases)
![clang-format Check](https://github.com/israpps/Funtuna-Fork/workflows/clang-format%20Check/badge.svg?branch=main)

> a homebrew ps2 app launcher designed for ps2 units incompatible with normal FreeMcBoot installs or memory cards without magicgae 

## Dont forget to read the [LICENSE](https://github.com/israpps/Funtuna-Fork/blob/main/LICENSE.MD)

## [main download link](https://github.com/israpps/Funtuna-Fork/releases/tag/latest)

### Special thanks to anyone that collaborated trough testing:

- GMV
- Gustavo Parra
- JuankyTutoriales

you can find the instructions to install at the [__WIKI__](https://github.com/israpps/Funtuna-Fork/wiki)

## contact links:

 + [FaceBook](https://www.facebook.com/matias.israelson.5/)

 + [PSX-Place](https://www.psx-place.com/members/el_isra.59064/)

## known issues:

- Opentuna will have issues if the memory card contains icons with japanese text

## Differences compared to original FunTuna:

- __Hacked icon (Opentuna/fortuna) is stored on an independent Folder to avoid timestamp changes when updating software__
- Every OSDSYS element is customizable (original FunTuna had top and bottom delimiters hardcoded)
- __Fixed bugs on FreeMcBoot__
- Changed FreeMcBoot settings filename to keep FunTuna settings separated from FreeMcBoot settings on usb
- FreeMcBoot and configurator can load USB drivers from `SYS-CONF` if not found on `BOOT`
- __Replaced fortuna with latest Opentuna__
- __Bundled with [wLaunchELF mod](https://github.com/israpps/wLaunchELF_ISR) capable of fixing fortuna/Opentuna icon date__ (quite stable and 101kb smaller than the original one)
- Bundled with [latest OPL release](https://github.com/ps2homebrew/Open-PS2-Loader/releases/latest)
- Installer based on Opentuna installer
- FreeMcBoot and configurator are slightly smaller
- Automatically updated if uLaunchELF is updated
- FreeMcBoot Ported to latest SDK by [Alex Parrado](https://github.com/parrado)
- FreeMcBoot Configurator Ported to latest SDK by [El_isra](https://github.com/israpps)
- Installer will load a preconfigured OPL folder if opl settings can´t be found, saving you the trouble of configuring common settings
- Installer will clean memory card _(Delete folders used by FunTuna fork, or folders that would interfere with Opentuna behaviour)_ before installing
