#ifdef WIN32
#include <windows.h>
static  void* DLOPEN(void)
{
  return((void*)LoadLibrary("Rblas.dll"));
}
static  void* DLSYM(void *handle, const char *symbol)
{
  return(GetProcAddress((HMODULE)handle,(LPCSTR)symbol));
}
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
  return(dlsym(handle,symbol));
}
static  int DLCLOSE(void *handle)
{
  return(dlclose(handle));
}

#endif
