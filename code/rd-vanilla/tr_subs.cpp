//Anything above this #include will be ignored by the compiler
#include "../server/exe_headers.h"

// tr_subs.cpp - common function replacements for modular renderer
#include "tr_local.h"

#ifdef _MSC_VER
extern int Q_vsnprintf(char *str, size_t size, const char *format, va_list ap);
#endif

void QDECL Com_Printf( const char *msg, ... )
{
	va_list         argptr;
	char            text[1024];

	va_start(argptr, msg);
	Q_vsnprintf(text, sizeof(text), msg, argptr);
	va_end(argptr);

	ri.Printf(PRINT_ALL, "%s", text);
}

void QDECL Com_Error( int level, const char *error, ... )
{
	va_list         argptr;
	char            text[1024];

	va_start(argptr, error);
	Q_vsnprintf(text, sizeof(text), error, argptr);
	va_end(argptr);

	ri.Error(level, "%s", text);
}

/*
================
Com_DPrintf

DLL glue
================
*/
void Com_DPrintf(const char *format, ...)
{
	va_list         argptr;
	char            text[1024];

	va_start(argptr, format);
	Q_vsnprintf(text, sizeof(text), format, argptr);
	va_end(argptr);

	ri.Printf(PRINT_DEVELOPER, "%s", text);
}

/*
================
VID_Printf

DLL glue
================
*/
#define	MAXPRINTMSG	4096
void VID_Printf (int print_level, const char *fmt, ...)
{
	va_list		argptr;
	char		msg[MAXPRINTMSG];
	
	va_start (argptr,fmt);
	vsprintf (msg,fmt,argptr);
	va_end (argptr);

	if ( print_level == PRINT_ALL ) {
		Com_Printf ("%s", msg);
	} else if ( print_level == PRINT_WARNING ) {
		Com_Printf (S_COLOR_YELLOW "%s", msg);		// yellow
	} else if ( print_level == PRINT_DEVELOPER ) {
		Com_DPrintf (S_COLOR_RED"%s", msg);
	}
}

// HUNK

//int Hunk_MemoryRemaining( void ) {
//	return ri.Hunk_MemoryRemaining();
//}

// ZONE
void *Z_Malloc( int iSize, memtag_t eTag, qboolean bZeroit, int iAlign ) {
	return ri.Z_Malloc( iSize, eTag, bZeroit, iAlign );
}

int Z_Free( void *ptr ) {
	return ri.Z_Free( ptr );
}

int Z_MemSize( memtag_t eTag ) {
	return ri.Z_MemSize( eTag );
}

void Z_MorphMallocTag( void *pvBuffer, memtag_t eDesiredTag ) {
	ri.Z_MorphMallocTag( pvBuffer, eDesiredTag );
}

// Parsing

#include "../game/GenericParser2.h"

bool Com_ParseTextFile(const char *file, class CGenericParser2 &parser, bool cleanFirst)
{
	fileHandle_t	f;
	int				length = 0;
	char			*buf = 0, *bufParse = 0;

	length = ri.FS_FOpenFileByMode( file, &f, FS_READ );
	if (!f || !length)		
	{
		return false;
	}

	buf = new char [length + 1];
	ri.FS_Read( buf, length, f );
	buf[length] = 0;

	bufParse = buf;
	parser.Parse(&bufParse, cleanFirst);
	delete buf;

	ri.FS_FCloseFile( f );

	return true;
}

void Com_ParseTextFileDestroy(class CGenericParser2 &parser)
{
	parser.Clean();
}

CGenericParser2 *Com_ParseTextFile(const char *file, bool cleanFirst, bool writeable)
{
	fileHandle_t	f;
	int				length = 0;
	char			*buf = 0, *bufParse = 0;
	CGenericParser2 *parse;

	length = ri.FS_FOpenFileByMode( file, &f, FS_READ );
	if (!f || !length)		
	{
		return 0;
	}

	buf = new char [length + 1];
	ri.FS_Read( buf, length, f );
	ri.FS_FCloseFile( f );
	buf[length] = 0;

	bufParse = buf;

	parse = new CGenericParser2;
	if (!parse->Parse(&bufParse, cleanFirst, writeable))
	{
		delete parse;
		parse = 0;
	}

	delete buf;

	return parse;
}
