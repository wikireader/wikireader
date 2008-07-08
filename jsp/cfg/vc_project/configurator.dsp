# Microsoft Developer Studio Project File - Name="configurator" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=configurator - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "configurator.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "configurator.mak" CFG="configurator - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "configurator - Win32 Release" ("Win32 (x86) Console Application" 用)
!MESSAGE "configurator - Win32 Debug" ("Win32 (x86) Console Application" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "configurator - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G6 /W2 /GR /GX /O2 /I ".." /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"../cfg.exe"

!ELSEIF  "$(CFG)" == "configurator - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I ".." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../cfg.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "configurator - Win32 Release"
# Name "configurator - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\base\collection.cpp
# End Source File
# Begin Source File

SOURCE=..\base\component.cpp
# End Source File
# Begin Source File

SOURCE=..\base\coverage.cpp
# End Source File
# Begin Source File

SOURCE=..\base\directorymap.cpp
# End Source File
# Begin Source File

SOURCE=..\base\event.cpp
# End Source File
# Begin Source File

SOURCE=..\base\except.cpp
# End Source File
# Begin Source File

SOURCE=..\base\garbage.cpp
# End Source File
# Begin Source File

SOURCE=..\base\manager.cpp
# End Source File
# Begin Source File

SOURCE=..\base\message.cpp
# End Source File
# Begin Source File

SOURCE=..\base\mpstrstream.cpp
# End Source File
# Begin Source File

SOURCE=..\base\option.cpp
# End Source File
# Begin Source File

SOURCE=..\base\parser.cpp
# End Source File
# Begin Source File

SOURCE=..\base\singleton.cpp
# End Source File
# Begin Source File

SOURCE=..\base\testsuite.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\base\collection.h
# End Source File
# Begin Source File

SOURCE=..\base\component.h
# End Source File
# Begin Source File

SOURCE=..\base\coverage_defs.h
# End Source File
# Begin Source File

SOURCE=..\base\coverage_undefs.h
# End Source File
# Begin Source File

SOURCE=..\base\defs.h
# End Source File
# Begin Source File

SOURCE=..\base\directorymap.h
# End Source File
# Begin Source File

SOURCE=..\base\event.h
# End Source File
# Begin Source File

SOURCE=..\base\except.h
# End Source File
# Begin Source File

SOURCE=..\base\garbage.h
# End Source File
# Begin Source File

SOURCE=..\base\message.h
# End Source File
# Begin Source File

SOURCE=..\base\mpstrstream.h
# End Source File
# Begin Source File

SOURCE=..\base\option.h
# End Source File
# Begin Source File

SOURCE=..\base\parser.h
# End Source File
# Begin Source File

SOURCE=..\base\singleton.h
# End Source File
# Begin Source File

SOURCE=..\base\testsuite.h
# End Source File
# End Group
# Begin Group "Target dependent"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\jsp\jsp_checkscript.cpp
# End Source File
# Begin Source File

SOURCE=..\jsp\jsp_classes.h
# End Source File
# Begin Source File

SOURCE=..\jsp\jsp_common.cpp
# End Source File
# Begin Source File

SOURCE=..\jsp\jsp_common.h
# End Source File
# Begin Source File

SOURCE=..\jsp\jsp_defs.h
# End Source File
# Begin Source File

SOURCE=..\jsp\jsp_parser.cpp
# End Source File
# Begin Source File

SOURCE=..\jsp\jsp_staticapi.cpp
# End Source File
# End Group
# End Target
# End Project
