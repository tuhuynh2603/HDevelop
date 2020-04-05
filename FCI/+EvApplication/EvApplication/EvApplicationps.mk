
EvApplicationps.dll: dlldata.obj EvApplication_p.obj EvApplication_i.obj
	link /dll /out:EvApplicationps.dll /def:EvApplicationps.def /entry:DllMain dlldata.obj EvApplication_p.obj EvApplication_i.obj \
		kernel32.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \
.c.obj:
	cl /c /Ox /DREGISTER_PROXY_DLL \
		$<

clean:
	@del EvApplicationps.dll
	@del EvApplicationps.lib
	@del EvApplicationps.exp
	@del dlldata.obj
	@del EvApplication_p.obj
	@del EvApplication_i.obj

EvApplicationps.dll: dlldata.obj EvApplication_p.obj EvApplication_i.obj
	link /dll /out:EvApplicationps.dll /def:EvApplicationps.def /entry:DllMain dlldata.obj EvApplication_p.obj EvApplication_i.obj \
		kernel32.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \
.c.obj:
	cl /c /Ox /DREGISTER_PROXY_DLL \
		$<

clean:
	@del EvApplicationps.dll
	@del EvApplicationps.lib
	@del EvApplicationps.exp
	@del dlldata.obj
	@del EvApplication_p.obj
	@del EvApplication_i.obj
