Name "J-Ben ${version}"
OutFile "..\archives\${version}\J-Ben_${version}_Installer.exe"
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
	File "..\J-Ben\README.txt"
	File "..\J-Ben\CHANGELOG.txt"
	# The following folders will always be added/removed in full,
	# so using File /r (and RMDir /r on uninstall) is acceptable.
	File /r "..\J-Ben\bin"
	File /r "..\J-Ben\etc"
	File /r "..\J-Ben\kpengine_data"
	File /r "..\J-Ben\lib"
	File /r "..\J-Ben\license"
	File /r "..\J-Ben\share"

	# Program Files\J-Ben\dicts
	# Although the user could add their own dictionaries one-by-one,
	# I think we can safely use /r here on installation and removal.
	File /r "..\J-Ben\dicts"

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
	SetOutPath "$INSTDIR\bin"
	CreateDirectory "$SMPROGRAMS\J-Ben"
	CreateShortcut "$SMPROGRAMS\J-Ben\J-Ben.lnk" "$INSTDIR\bin\jben.exe"
	CreateShortcut "$SMPROGRAMS\J-Ben\Uninstall J-Ben.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

Section "Create Desktop Icon"
	SetOutPath "$INSTDIR\bin"
	CreateShortcut "$DESKTOP\J-Ben.lnk" "$INSTDIR\bin\jben.exe"
SectionEnd

;Section "J-Ben Documentation"
;SectionEnd

;Section "J-Ben Source Code"
;SectionEnd

Section "un.J-Ben Core"
	# Program Files\J-Ben
	SetOutPath "$INSTDIR"
	Delete "README.txt"
	Delete "CHANGELOG.txt"
	RMDir /r "$INSTDIR\bin"
	RMDir /r "$INSTDIR\etc"
	RMDir /r "$INSTDIR\kpengine_data"
	RMDir /r "$INSTDIR\lib"
	RMDir /r "$INSTDIR\license"
	RMDir /r "$INSTDIR\share"

	# Program Files\J-Ben\dicts
	# Although the user could add their own dictionaries one-by-one,
	# I think we can safely use /r here on installation and removal.
	RMDir /r "$INSTDIR\dicts"

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
