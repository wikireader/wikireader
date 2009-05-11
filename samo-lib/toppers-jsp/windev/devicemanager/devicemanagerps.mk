
devicemanagerps.dll: dlldata.obj devicemanager_p.obj devicemanager_i.obj
	link /dll /out:devicemanagerps.dll /def:devicemanagerps.def /entry:DllMain dlldata.obj devicemanager_p.obj devicemanager_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del devicemanagerps.dll
	@del devicemanagerps.lib
	@del devicemanagerps.exp
	@del dlldata.obj
	@del devicemanager_p.obj
	@del devicemanager_i.obj
