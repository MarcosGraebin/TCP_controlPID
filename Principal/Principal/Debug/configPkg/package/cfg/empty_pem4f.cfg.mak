# invoke SourceDir generated makefile for empty.pem4f
empty.pem4f: .libraries,empty.pem4f
.libraries,empty.pem4f: package/cfg/empty_pem4f.xdl
	$(MAKE) -f C:\Users\marco\Desktop\2019.2\SISTEM~1\PROJET~1\projetos\Principal/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\marco\Desktop\2019.2\SISTEM~1\PROJET~1\projetos\Principal/src/makefile.libs clean

