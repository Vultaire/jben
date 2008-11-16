# This installer requires a version of NSIS with NSISdl support (any recent one),
# plus the ZipDLL plugin.
Name "J-Ben ${version}"
OutFile "..\archives\${version}\J-Ben_${version}_Installer.exe"
InstallDir $PROGRAMFILES\J-Ben
InstallDirRegKey HKLM \
	"Software\Microsoft\Windows\CurrentVersion\Uninstall\J-Ben" \
	"InstallLocation"

LicenseData ..\J-Ben\COPYING
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
	File "..\J-Ben\README"
	File "..\J-Ben\CHANGELOG"
	File "..\J-Ben\CREDITS"
	File "..\J-Ben\DEDICATION"
	File "..\J-Ben\COPYING"
	# The following folders will always be added/removed in full,
	# so using File /r (and RMDir /r on uninstall) is acceptable.
	File /r "..\J-Ben\bin"
	File /r "..\J-Ben\etc"
	File /r "..\J-Ben\kpengine_data"
	File /r "..\J-Ben\lib"
	File /r "..\J-Ben\license"
	File /r "..\J-Ben\share"

	# Program Files\J-Ben\dicts
	SetOutPath "$INSTDIR\dicts"
	File "..\J-Ben\dicts\README"

	# Program Files\J-Ben\sods
	SetOutPath "$INSTDIR\sods"
	File "..\J-Ben\sods\README"

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

SectionGroup "Dictionary Files"
	Section "EDICT2"
		SetOutPath "$INSTDIR\dicts"
		File "..\J-Ben\dicts\edict2.gz"
	SectionEnd
	Section "KANJIDIC"
		SetOutPath "$INSTDIR\dicts"
		File "..\J-Ben\dicts\kanjidic.gz"
	SectionEnd
	Section /o "KANJIDIC2 (Download)"
		SetOutPath "$INSTDIR\dicts"
		NSISdl::download http://www.vultaire.net/software/jben/files/dicts/kanjidic2.xml.gz kanjidic2.xml.gz
		Pop $R0
		StrCmp $R0 "success" +2
		MessageBox MB_OK "The installer was unable to download KANJIDIC2.  Installation will continue, but the program may not work properly."
	SectionEnd
SectionGroupEnd

SectionGroup "Stroke Order Diagrams (Download)"
	Section /o "KanjiCafe.com SODs"
		SetOutPath "$INSTDIR\sods"
		NSISdl::download http://www.vultaire.net/software/jben/files/sods/sod-utf8-hex.zip sod-utf8-hex.zip
		Pop $R0
		StrCmp $R0 "success" +3
		MessageBox MB_OK "The installer was unable to download the KanjiCafe.com SODs.  Installation will continue without them."
		Return
		ZipDLL::extractall "sod-utf8-hex.zip" "$INSTDIR\sods"
		Delete "sod-utf8-hex.zip"
	SectionEnd
	Section /o "KanjiCafe.com Animated SODs"
		SetOutPath "$INSTDIR\sods"
		NSISdl::download http://www.vultaire.net/software/jben/files/sods/soda-utf8-hex.zip soda-utf8-hex.zip
		Pop $R0
		StrCmp $R0 "success" +3
		MessageBox MB_OK "The installer was unable to download the KanjiCafe.com animated SODs.  Installation will continue without them."
		Return
		ZipDLL::extractall "soda-utf8-hex.zip" "$INSTDIR\sods"
		Delete "soda-utf8-hex.zip"
	SectionEnd
SectionGroupEnd

Section "Documentation"
	# Program Files\J-Ben
	SetOutPath "$INSTDIR"
	# The following folders will always be added/removed in full,
	# so using File /r (and RMDir /r on uninstall) is acceptable.
	File /r "..\J-Ben\doc"
SectionEnd

Section /o "Source code (Download)"
		SetOutPath "$INSTDIR\src"
		NSISdl::download http://www.vultaire.net/software/jben/files/${version}/jben-${version}-source.zip source.zip
		Pop $R0
		StrCmp $R0 "success" +3
		MessageBox MB_OK "The installer was unable to download the J-Ben source code package.  Installation will continue without it."
		Return
		ZipDLL::extractall "source.zip" "$INSTDIR\src"
		Delete "source.zip"
SectionEnd

SectionGroup "Shortcuts"
	Section "Create Start Menu Shortcuts"
		SetOutPath "$INSTDIR\bin"
		CreateDirectory "$SMPROGRAMS\J-Ben"
		CreateShortcut "$SMPROGRAMS\J-Ben\J-Ben.lnk" "$INSTDIR\bin\jben.exe"
		CreateShortcut "$SMPROGRAMS\J-Ben\Uninstall J-Ben.lnk" "$INSTDIR\uninstall.exe"
	SectionEnd

	Section "Create Desktop Shortcut"
		SetOutPath "$INSTDIR\bin"
		CreateShortcut "$DESKTOP\J-Ben.lnk" "$INSTDIR\bin\jben.exe"
	SectionEnd
SectionGroupEnd

Section "un.J-Ben Core"
	# Program Files\J-Ben
	SetOutPath "$INSTDIR"
	Delete "README"
	Delete "CHANGELOG"
	Delete "CREDITS"
	Delete "DEDICATION"
	Delete "COPYING"
	RMDir /r "$INSTDIR\bin"
	RMDir /r "$INSTDIR\etc"
	RMDir /r "$INSTDIR\kpengine_data"
	RMDir /r "$INSTDIR\lib"
	RMDir /r "$INSTDIR\license"
	RMDir /r "$INSTDIR\share"

	Delete "dicts\README"
	RMDir "$INSTDIR\dicts"
	Delete "sods\README"
	RMDir "$INSTDIR\sods"

	Delete "uninstall.exe"
	SetOutPath "$INSTDIR\.."
	RMDir "$INSTDIR"

	DeleteRegKey HKLM \
		"Software\Microsoft\Windows\CurrentVersion\Uninstall\J-Ben"
SectionEnd

SectionGroup "un.Dictionaries"
	Section "un.EDICT2"
		SetOutPath "$INSTDIR"
		Delete "dicts\edict2.gz"
		RMDir "$INSTDIR\dicts"
		SetOutPath "$INSTDIR\.."
		RMDir "$INSTDIR"
	SectionEnd
	Section "un.KANJIDIC"
		SetOutPath "$INSTDIR"
		Delete "dicts\kanjidic.gz"
		RMDir "$INSTDIR\dicts"
		SetOutPath "$INSTDIR\.."
		RMDir "$INSTDIR"
	SectionEnd
	Section "un.KANJIDIC2"
		SetOutPath "$INSTDIR"
		Delete "dicts\kanjidic2.xmlgz"
		RMDir "$INSTDIR\dicts"
		SetOutPath "$INSTDIR\.."
		RMDir "$INSTDIR"
	SectionEnd
SectionGroupEnd

SectionGroup "un.Stroke Order Diagrams"
	Section "un.KanjiCafe.com SODs"
		SetOutPath "$INSTDIR\sods"
		RMDir /r "$INSTDIR\sods\sod-utf8-hex"
		SetOutPath "$INSTDIR"
		RMDir "$INSTDIR\sods"
		SetOutPath "$INSTDIR\.."
		RMDir "$INSTDIR"
	SectionEnd
	Section "un.KanjiCafe.com Animated SODs"
		SetOutPath "$INSTDIR\sods"
		RMDir /r "$INSTDIR\sods\soda-utf8-hex"
		SetOutPath "$INSTDIR"
		RMDir "$INSTDIR\sods"
		SetOutPath "$INSTDIR\.."
		RMDir "$INSTDIR"
	SectionEnd
SectionGroupEnd

Section "un.J-Ben Documentation"
	# Program Files\J-Ben
	SetOutPath "$INSTDIR"
	# The following folders will always be added/removed in full,
	# so using File /r (and RMDir /r on uninstall) is acceptable.
	RMDir /r "$INSTDIR\doc"

	SetOutPath "$INSTDIR\.."
	RMDir "$INSTDIR"
SectionEnd

Section "un.Start Menu and Desktop Shortcuts"
	Delete "$SMPROGRAMS\J-Ben\J-Ben.lnk"
	Delete "$SMPROGRAMS\J-Ben\Uninstall J-Ben.lnk"
	RMDir "$SMPROGRAMS\J-Ben"
	Delete "$DESKTOP\J-Ben.lnk"
SectionEnd

Section "un.Source code"
	SetOutPath "$INSTDIR"
	RMDir /r "$INSTDIR\src"
	SetOutPath "$INSTDIR\.."
	RMDir "$INSTDIR"
SectionEnd
