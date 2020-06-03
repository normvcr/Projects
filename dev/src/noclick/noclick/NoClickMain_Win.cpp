/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <Windows.h>

// http://alter.org.ua/en/docs/win/args/
PCHAR*
CommandLineToArgvA(
		   PCHAR CmdLine,
		   int* _argc
		   )
{
  PCHAR* argv;
  PCHAR  _argv;
  ULONG   len;
  ULONG   argc;
  CHAR   a;
  ULONG   i, j;

  BOOLEAN  in_QM;
  BOOLEAN  in_TEXT;
  BOOLEAN  in_SPACE;

  len = strlen(CmdLine);
  i = ((len+2)/2)*sizeof(PVOID) + sizeof(PVOID);

  argv = (PCHAR*)GlobalAlloc(GMEM_FIXED,
			     i + (len+2)*sizeof(CHAR));

  _argv = (PCHAR)(((PUCHAR)argv)+i);

  argc = 0;
  argv[argc] = _argv;
  in_QM = FALSE;
  in_TEXT = FALSE;
  in_SPACE = TRUE;
  i = 0;
  j = 0;

  while( a = CmdLine[i] ) {
    if(in_QM) {
      if(a == '\"') {
	in_QM = FALSE;
      } else {
	_argv[j] = a;
	j++;
      }
    } else {
      switch(a) {
      case '\"':
	in_QM = TRUE;
	in_TEXT = TRUE;
	if(in_SPACE) {
	  argv[argc] = _argv+j;
	  argc++;
	}
	in_SPACE = FALSE;
	break;
      case ' ':
      case '\t':
      case '\n':
      case '\r':
	if(in_TEXT) {
	  _argv[j] = '\0';
	  j++;
	}
	in_TEXT = FALSE;
	in_SPACE = TRUE;
	break;
      default:
	in_TEXT = TRUE;
	if(in_SPACE) {
	  argv[argc] = _argv+j;
	  argc++;
	}
	_argv[j] = a;
	j++;
	in_SPACE = FALSE;
	break;
      }
    }
    i++;
  }
  _argv[j] = '\0';
  argv[argc] = NULL;

  (*_argc) = argc;
  return argv;
}// CommandLineToArgvA

extern int mainBody( int argc, char* argv[] );

int WinMain( HINSTANCE hInstance,
		       HINSTANCE hPrevInstance, // Always NULL
		       LPSTR     lpCmdLine,
		       int       nShowCmd   // TBD ??
		       ) {

  LPSTR lpFullCmdLine = GetCommandLineA();

  int argc = 0;
  PCHAR* argv = CommandLineToArgvA( lpFullCmdLine, &argc );

  return mainBody( argc, argv );
}// WinMain
