
watcherps.dll: dlldata.obj watcher_p.obj watcher_i.obj
	link /dll /out:watcherps.dll /def:watcherps.def /entry:DllMain dlldata.obj watcher_p.obj watcher_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del watcherps.dll
	@del watcherps.lib
	@del watcherps.exp
	@del dlldata.obj
	@del watcher_p.obj
	@del watcher_i.obj
