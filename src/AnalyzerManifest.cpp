/*
 * Android_AXML.cpp
 *
 *  Created on: 2016年7月27日
 *      Author: LsMouse
 */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <windows.h>
#include <unistd.h>
#include <sys/stat.h>
#include "AXML.h"
/*
 * Show usage.
 */
void usage(void)
{
	fprintf(stderr, "Copyright (C) 2016 The AnalyzerManifest Project\n\n");
	fprintf(stderr, "i: input AndroidManifest.xml\n");
	fprintf(stderr, "o: ouput AndroidManifest.xml\n");
	fprintf(stderr, "e: AppName \n");
	fprintf(stderr, "f: Out Old Info \n");
	exit(0);
}
/*
************************************************************
*			getARG(int argc,char* argv[])
* 获取终端输入参数
*
************************************************************
*/
const char* cp_in_AXML = NULL;
const char* cp_out_AXML = NULL;
const char* cp_AppName = NULL;
const char* cp_InfoFile = NULL;
void getARG(int argc,char* argv[])
{
	int ch;
	opterr = 0;
	while ((ch = getopt(argc,argv,"i:o:e:f:h"))!=-1)
	{
		switch(ch)
		{
			case 'e':	// 定义配置文件 设置AppName
				cp_AppName = strdup(optarg);
				DEXLOG("cp_AppName :%s",cp_AppName);
				break;
			case 'i':	// 定义配置文件 获取XML文件
				cp_in_AXML = strdup(optarg);
				DEXLOG("cp_in_AXML :%s",cp_in_AXML);
				break;
			case 'o': 	// 定义输出文件 设置输出文件路径
				cp_out_AXML = strdup(optarg);
				DEXLOG("cp_out_AXML :%s",cp_out_AXML);
				break;
			case 'f': 	// 定义输出文件 设置输出文件路径
				cp_InfoFile = strdup(optarg);
				DEXLOG("cp_InfoFile :%s",cp_InfoFile);
				break;
			case 'h':	// help
				usage();break;
		}
	}
	if(cp_in_AXML == NULL)
	{
		usage();
	}
	struct stat mstat;
	if(stat(cp_in_AXML,&mstat) == -1)
	{
		DEXLOG("%s isn't file ",cp_in_AXML);
		usage();
	}
}
/*
*			Android Main
*设置功能。
*
*/

int main(int argc, char *argv[])
{
	// 获取 ARG
	getARG(argc,argv);
	//123
	CAXML myaxml((char*)cp_in_AXML);
	string appname = myaxml.GetApplicationEntry();
	string pkgname = myaxml.GetPkgName();
	DEXLOG("appname: %s, pkgname: %s", appname.c_str(), pkgname.c_str());
	if(cp_InfoFile != NULL)
	{
		char mBuf[1024] = {0};
		if( appname.c_str()[0] == '.')
		{
			sprintf(mBuf,"appname:%s%s\npkgname:%s\n",
					pkgname.c_str(),appname.c_str(), pkgname.c_str());
		}else{
			sprintf(mBuf,"appname:%s\npkgname:%s\n",
					appname.c_str(), pkgname.c_str());
		}

		size_t inSize = strlen(mBuf);
		FILE* mfd = fopen(cp_InfoFile,"wb");
		if(mfd != NULL)
		{
			fwrite(mBuf,inSize,1,mfd);
			fclose(mfd);
		}
	}
	if(cp_AppName != NULL)
	{
		myaxml.SetApplicationEntry((char*)cp_AppName);
	}
	if(cp_out_AXML != NULL)
	{
		myaxml.SaveAXMLFile((char*)cp_out_AXML);
	}
}



