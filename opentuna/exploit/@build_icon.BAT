
@echo off
::CONFIG TABLE
::ICON GENERATOR FILENAME:
set PJAR=OpenTunaRLE.jar

::PAYLOAD BINARIES (FILE TO BE CONVERTED) replace with %1 to activate drag and drop feature
set PBIN=%1

::OUTPUT FILENAME FOR ICON
set PICN=icon.icn
@echo off
if exist %PICN% (
echo found [%PICN%] file, press any key to delete it and continue
pause >nul
)
echo =============================================================
echo automated icon.icn maker for opentuna payload
echo creating [%PICN%]
echo input: [%PBIN%]
echo =============================================================

java -jar %PJAR% -e %PBIN% %PICN%

timeout 15
