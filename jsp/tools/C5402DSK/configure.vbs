Dim fs
Dim InFile, OutFile
Dim str
Dim cmd
Dim Ws
Dim ccs

Const ForReading = 1, ForWriting = 2, ForAppending = 3

msgbox "TOPPERS/JSP をビルドするのに必要なファイルを作成します．"

Set fs = CreateObject("Scripting.FileSystemObject")
fs.copyfile "..\..\sample\sample1.c", ".\"
fs.copyfile "..\..\sample\sample1.h", ".\"

Set InFile = fs.OpenTextFile("..\..\sample\sample1.cfg", ForReading, False)

Set OutFile = fs.OpenTextFile(".\sample1.cfg", ForWriting, True)

While not InFile.AtEndOfStream
	str = InFile.ReadLine
	str = Replace(str,"@(SRCDIR)",  "../../")
	OutFile.WriteLine str
Wend

InFile.Close
OutFile.Close

set Ws = WScript.CreateObject("WScript.Shell")
path = "c:\ti\c5400\cgtools\"
cmd = """" + path + "bin\ar500.exe"" -x """ + path + "lib\rts.src"" ldiv.asm udiv.asm lmpy.asm"
Ws.run cmd, 0, true

cmd = "cl /E /D_MACRO_ONLY /EP /I ""../../kernel"" /I ""../../include"" /I ""../../config/tms320c54x"" /I ""../../config/tms320c54x/c5402dsk"" sample1.cfg > sample1_i.cfg"
Ws.run "cmd /c" & cmd, 0, true

cmd = "..\..\cfg\cfg.exe -s sample1_i.cfg  -c -v -lj -obj --ZERO ""x y[1]"""
Ws.run cmd, 0, true

msgbox "正常に終了しました"

