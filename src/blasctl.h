#define OPTION_WIN_RBLAS_NAME "RhpcBLASctl.win.Rblas.name"
#define OPTION_WIN_RBLAS_TYPE "RhpcBLASctl.win.Rblas.type"

static char *getRblasName(void)
{
  static char Rblas[1024];
  SEXP ret = GetOption1(install(OPTION_WIN_RBLAS_NAME));
  if(ret==R_NilValue||XLENGTH(ret)<1||TYPEOF(ret)!=STRSXP) return("Rblas.dll");
  strncpy(Rblas, CHAR(STRING_ELT(ret,0)), sizeof(Rblas));
  return(Rblas);
}

#ifdef WIN32
#include <windows.h>
static  void* DLOPEN(void)
{
  return((void*)LoadLibrary(getRblasName()));
}
static  FARPROC DLSYM(void *handle, const char *symbol)
{
  FARPROC proc = GetProcAddress((HMODULE)handle,(LPCSTR)symbol);
  if(proc != NULL)
    Rprintf("detected function %s\n", symbol);
  return(proc);
}
#define EXPDLSYM(_res,_cast,_handle,_symbol) (_res = (_cast)DLSYM(_handle,_symbol)) 
static  int DLCLOSE(void *handle)
{
  return(FreeLibrary((HMODULE)handle));
}
#else
#include <dlfcn.h>
static  void* DLOPEN(void)
{
  return(dlopen(NULL, RTLD_NOW|RTLD_GLOBAL));
}
static  void* DLSYM(void *handle, const char *symbol)
{
  void* proc;
  *(void**)(&proc)= dlsym(handle,symbol);
  return(proc);
}
#define EXPDLSYM(_res,_cast,_handle,_symbol) (*(void**)(&_res) =DLSYM(_handle,_symbol)) 
static  int DLCLOSE(void *handle)
{
  return(dlclose(handle));
}

#endif
