Name "J-Ben ${VERSION}"
OutFile "..\archives\${VERSION}\J-Ben_${VERSION}_Installer.exe"
InstallDir $PROGRAMFILES\J-Ben
InstallDirRegKey HKLM \
	"Software\Microsoft\Windows\CurrentVersion\Uninstall\J-Ben" \
	"InstallLocation"

LicenseData ..\J-Ben\license\gpl-2.0.txt
LicenseForceSelection checkbox

Page license
Page components
Page directory
Page instfiles
UninstPage uninstConfirm
UninstPage components
UninstPage instfiles

# I am deliberately being very specific on which files are installed
# and uninstalled.  This allows us to easily copy/paste from the
# installer section to the uninstaller, ensuring we ONLY remove the files
# we installed.  It's ugly, I know, but the script suggested to automate
# this is ugly enough by itself.
Section "!J-Ben Core"
	# Program Files\J-Ben
	SetOutPath "$INSTDIR"
	File "..\J-Ben\jben.exe"
	File "..\J-Ben\jben_kpengine.exe"
	File "..\J-Ben\libcharset1.dll"
	File "..\J-Ben\libiconv2.dll"
	File "..\J-Ben\libstlport.5.1.dll"
	File "..\J-Ben\mingwm10.dll"
	File "..\J-Ben\wxbase28u_gcc_vultaire.net.dll"
	File "..\J-Ben\wxmsw28u_core_gcc_vultaire.net.dll"
	File "..\J-Ben\wxmsw28u_html_gcc_vultaire.net.dll"
	File "..\J-Ben\README.txt"
	File "..\J-Ben\CHANGELOG.txt"
	# The following 2 folders will always be added/removed in full,
	# so using File /r (and RMDir /r on uninstall) is acceptable.
	File /r "..\J-Ben\kpengine_data"
	File /r "..\J-Ben\license"

	# Program Files\J-Ben\dicts
	# By default, we install EDICT2, KANJIDIC, KRADFILE and RADKFILE.
	SetOutPath "$INSTDIR\dicts"
	File "..\J-Ben\dicts\README.txt"
	File "..\J-Ben\dicts\edict2"
	File "..\J-Ben\dicts\kanjidic"
	File "..\J-Ben\dicts\kradfile"
	File "..\J-Ben\dicts\radkfile"

	# Program Files\J-Ben\sods
	# No SODs are loaded by default; this is an optional component.
	# The only file to be copied is a readme.
	SetOutPath "$INSTDIR\sods"
	File "..\J-Ben\sods\README.txt"

	# Write registry settings and make uninstaller
	WriteRegStr HKLM \
		"Software\Microsoft\Windows\CurrentVersion\Uninstall\J-Ben" \
		"DisplayName" "J-Ben"
	WriteRegStr HKLM \
		"Software\Microsoft\Windows\CurrentVersion\Uninstall\J-Ben" \
		"UninstallString" "$INSTDIR\uninstall.exe"
	WriteRegStr HKLM \
		"Software\Microsoft\Windows\CurrentVersion\Uninstall\J-Ben" \
		"InstallLocation" "$INSTDIR"
	WriteRegStr HKLM \
		"Software\Microsoft\Windows\CurrentVersion\Uninstall\J-Ben" \
		"DisplayIcon" "$INSTDIR\jben.exe"
	WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd

Section "Create Start Menu Icons"
	SetOutPath "$INSTDIR"
	CreateDirectory "$SMPROGRAMS\J-Ben"
	CreateShortcut "$SMPROGRAMS\J-Ben\J-Ben.lnk" "$INSTDIR\jben.exe"
	CreateShortcut "$SMPROGRAMS\J-Ben\Uninstall J-Ben.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

Section /o "Create Desktop Icon"
	SetOutPath "$INSTDIR"
	CreateShortcut "$DESKTOP\J-Ben.lnk" "$INSTDIR\jben.exe"
SectionEnd

;Section "J-Ben Documentation"
;SectionEnd

;Section "J-Ben Source Code"
;SectionEnd

Section "un.J-Ben Core"
	# Program Files\J-Ben
	SetOutPath "$INSTDIR"
	Delete "jben.exe"
	Delete "jben_kpengine.exe"
	Delete "libcharset1.dll"
	Delete "libiconv2.dll"
	Delete "libstlport.5.1.dll"
	Delete "mingwm10.dll"
	Delete "wxbase28u_gcc_vultaire.net.dll"
	Delete "wxmsw28u_core_gcc_vultaire.net.dll"
	Delete "wxmsw28u_html_gcc_vultaire.net.dll"
	Delete "README.txt"
	Delete "CHANGELOG.txt"
	RMDir /r "$INSTDIR\kpengine_data"
	RMDir /r "$INSTDIR\license"

	# Program Files\J-Ben\dicts
	# By default, we install EDICT2, KANJIDIC, KRADFILE and RADKFILE.
	Delete "dicts\README.txt"
	Delete "dicts\edict2"
	Delete "dicts\kanjidic"
	Delete "dicts\kradfile"
	Delete "dicts\radkfile"
	RMDir "$INSTDIR\dicts"

	# Program Files\J-Ben\sods
	# No SODs are loaded by default; this is an optional component.
	# The only file to be copied is a readme.
	Delete "sods\README.txt"
	RMDir "$INSTDIR\sods"

	Delete "uninstall.exe"
	SetOutPath "$INSTDIR\.."
	RMDir "$INSTDIR"

	DeleteRegKey HKLM \
		"Software\Microsoft\Windows\CurrentVersion\Uninstall\J-Ben"
SectionEnd

Section "un.Start Menu and Desktop Shortcuts"
	Delete "$SMPROGRAMS\J-Ben\J-Ben.lnk"
	Delete "$SMPROGRAMS\J-Ben\Uninstall J-Ben.lnk"
	RMDir "$SMPROGRAMS\J-Ben"
	Delete "$DESKTOP\J-Ben.lnk"
SectionEnd
