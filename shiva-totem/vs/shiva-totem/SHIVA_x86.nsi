#name the installer

Icon "shiva-icon.ico"

OutFile "SHIVA_x86.exe"

InstallDir $DESKTOP\SHIVA

#default section start; every NSIS has at least one section
Section "Installer Section"



#create a popup box with OK button and text "Hello World!"
#MessageBox MB_OK "Hello World!"
SetOutPath $INSTDIR
File shiva-totem.exe
File dll_x86\*.dll
File /nonfatal /a /r Profiles\

SetOutPath $INSTDIR\Resources
File /nonfatal /r Resources\*.*

CreateDirectory $INSTDIR\Savefiles

WriteUninstaller $INSTDIR\Uninstaller.exe

#default section end
SectionEnd

Section "un.Uninstaller Section"

Delete $INSTDIR\Uninstaller.exe
RMDir /r $DESKTOP\SHIVA

SectionEnd