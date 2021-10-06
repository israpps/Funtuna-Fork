
[![Logo](https://github.com/israpps/Funtuna-Fork/blob/main/logos%20%26%20others/github%20logo.png "Funtuna logo")](https://github.com/israpps/Funtuna-Fork/releases)
***

[![Downloads](https://img.shields.io/github/downloads/israpps/Funtuna-Fork/total.svg)](https://github.com/israpps/Funtuna-Fork/releases)
![clang-format Check](https://github.com/israpps/Funtuna-Fork/workflows/clang-format%20Check/badge.svg?branch=main)

> a homebrew ps2 app launcher designed for FMCB-uncompatible models


## Dont forget to read the [LICENSE](https://github.com/israpps/Funtuna-Fork/blob/main/LICENSE.MD)


## [main download link](https://github.com/israpps/Funtuna-Fork/releases/tag/latest)

you can find the instructions to install at the [__WIKI__](https://github.com/israpps/Funtuna-Fork/wiki)

## contact links:

 + [FaceBook](https://www.facebook.com/matias.israelson.5/)

 + [PSX-Place](https://www.psx-place.com/members/el_isra.59064/)






## Differences compared to original FunTuna:

- __Hacked icon (Opentuna/fortuna) is stored on an independent Folder to avoid timestamp changes when updating software__
- Every OSDSYS element is customizable (original FunTuna had top and bottom delimiters hardcoded)
- __Fixed bugs on FreeMcBoot__
- changed FreeMcBoot settings filename to keep FunTuna settings separated from FreeMcBoot settings on usb
- FreeMcBoot and configurator can load USB drivers from `SYS-CONF` if not found on `BOOT`
- __Replaced fortuna with latest Opentuna__
- __bundled with [wLaunchELF mod](https://github.com/israpps/wLaunchELF_ISR) capable of fixing fortuna/Opentuna icon date__ (quite stable and 101kb smaller than the original one)
- Bundled with [latest OPL release](https://github.com/ps2homebrew/Open-PS2-Loader/releases/latest)
- Installer based on Opentuna installer
- FreeMcBoot and configurator are slightly smaller
- Automatically updated if uLaunchELF or OPL are updated
