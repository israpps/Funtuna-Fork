@echo off
IF EXIST BOOT.ELF del CFG.ELF
ps2_packer.exe BOOT.ELF CFG.ELF

PAUSE >nul
PAUSE