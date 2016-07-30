开发环境：
	1. eclipse 4.4.2
	2. mingw32
工具说明:
	1. 直接二进制修改AndroidManifest.xml
	2. 
shell:
	./AnalyzerManifest.exe -i AndroidManifest.xml -o AndroidManifest_patch.xml -a ijm.ProxyApp 
help:
	"Copyright (C) 2016 The AnalyzerManifest Project\n\n"
	"i: input AndroidManifest.xml\n"
	"o: ouput AndroidManifest.xml\n"
	"e: AppName \n"
	"f: Out Old Info \n"