#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include "ucc.h"

#define _P_WAIT 0
#define UCLDIR "./ucl/"
#define GCCDIR "c:/Users/zeli/Apps/msys64/mingw64/"
#define BINDIR GCCDIR "bin\\"

static char *CPPProg[] =
{
	BINDIR "gcc", "-D_POSIX_SOURCE", "-D__STRICT_ANSI__",
	"-Dunix", "-Di386", "-Dlinux", "-D__unix__", "-D__i386__", "-D__linux__",
	"-D__signed__=signed", "-D_UCC", "-I" GCCDIR "include", "$1", "-E", "$2", "-o", "$3", 0
};
static char *CCProg[] =
{
	UCLDIR "ucl.exe", "-ext:.s", "$1", "$2", 0
};
static char *ASProg[] =
{
	BINDIR "gcc", "-o", "$3", "$1", "$2", 0
};
static char *LDProg[] =
{
	BINDIR "gcc", "-o", "$3", "$1", "$2", UCLDIR "assert.o", "-lc", "-lm", 0
};
char *ExtNames[] = { ".c", ".i", ".s", ".o", ".a;.so", 0 };

static int Execute(char **cmd)
{
    int status = _spawnvp(_P_WAIT, cmd[0], (const char* const*)cmd);
    if (status != 0) {
        perror("spawn process error:");
    }
    return status;
}

void SetupToolChain(void)
{
}

int InvokeProgram(int oftype)
{
	List p = NULL;
	char **cmd;
	char *file;
	int status = 0;

	switch (oftype)
	{
	case PP_FILE:
		if (Option.cfiles == NULL)
			return 0;

		for (p = Option.cfiles; p != NULL; p = p->next)
		{
			PPFiles = ListAppend(PPFiles, FileName(p->str, ".i"));
		}

		Option.pfiles = ListCombine(Option.pfiles, PPFiles);
		cmd = BuildCommand(CPPProg, Option.pflags, Option.cfiles, PPFiles);
		status = Execute(cmd);

		for (p = PPFiles; p != NULL; p = p->next)
		{
			if ((file = strrchr(p->str, '\\')) || (file = strrchr(p->str, '/')))
			{
				rename(file + 1, p->str);
			}
		}
		break;

	case ASM_FILE:
		if (Option.pfiles == NULL)
			return 0;

		for (p = Option.pfiles; p != NULL; p = p->next)
		{
			ASMFiles = ListAppend(ASMFiles, FileName(p->str, ".s"));
		}

		Option.afiles = ListCombine(Option.afiles, ASMFiles);
		cmd = BuildCommand(CCProg, Option.cflags, Option.pfiles, ASMFiles);
		status = Execute(cmd);
		break;

	case OBJ_FILE:
		if (Option.afiles == NULL)
			return 0;

		for (p = Option.aflags, Option.aflags = NULL; p != NULL; p = p->next)
		{
			Option.aflags = ListCombine(Option.aflags, ParseOption(p->str + 4));
		}
		for (p = Option.afiles; p != NULL; p = p->next)
		{
			file = FileName(p->str, ".o");
			OBJFiles = ListAppend(OBJFiles, file);
			cmd = BuildCommand(ASProg, Option.aflags, ListAppend(NULL, p->str), ListAppend(NULL, file));
			status = Execute(cmd);
		}
		Option.ofiles = ListCombine(Option.ofiles, OBJFiles);
		break;

	case LIB_FILE:
		break;

	case EXE_FILE:
		if (Option.ofiles == NULL)
			return 0;

		if (Option.out == NULL)
		{
			Option.out = Option.ofiles->str;
		}
		Option.out = FileName(Option.out, ".exe");
		for (p = Option.lflags, Option.lflags = NULL; p != NULL; p = p->next)
		{
			Option.lflags = ListCombine(Option.lflags, ParseOption(p->str + 4));
		}
		cmd = BuildCommand(LDProg, Option.lflags, Option.linput, ListAppend(NULL, Option.out));
		status = Execute(cmd);
		break;
	}

	return status;
}
