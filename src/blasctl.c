#include <R.h>
#include <Rinternals.h>

#ifndef WIN32
#include <sys/sysinfo.h>
#endif

#ifdef _OPENMP
#include <omp.h>
#endif

#include "blasctl.h"

#if defined(WIN32)
#if !defined(LTP_PC_SMT)
typedef enum _PROCESSOR_CACHE_TYPE {
  CacheUnified,CacheInstruction,CacheData,CacheTrace
} PROCESSOR_CACHE_TYPE;
typedef struct _CACHE_DESCRIPTOR {
  BYTE Level;
  BYTE Associativity;
  WORD LineSize;
  DWORD Size;
  PROCESSOR_CACHE_TYPE Type;
} CACHE_DESCRIPTOR,*PCACHE_DESCRIPTOR;
#endif
#if !defined(CACHE_FULLY_ASSOCIATIVE)
typedef enum _LOGICAL_PROCESSOR_RELATIONSHIP {
  RelationProcessorCore,RelationNumaNode,RelationCache
} LOGICAL_PROCESSOR_RELATIONSHIP;
typedef struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION {
  ULONG_PTR ProcessorMask;
  LOGICAL_PROCESSOR_RELATIONSHIP Relationship;
  union {
    struct {
      BYTE Flags;
    } ProcessorCore;
    struct {
      DWORD NodeNumber;
    } NumaNode;
    CACHE_DESCRIPTOR Cache;
    ULONGLONG Reserved[2];
  };
} SYSTEM_LOGICAL_PROCESSOR_INFORMATION,*PSYSTEM_LOGICAL_PROCESSOR_INFORMATION;
#endif
#endif

#define MKL_ALL  0
#define MKL_BLAS 1
#define MKL_FFT  2
#define MKL_VML  3

#define GOTO_GET_NUM_PROCS         "goto_get_num_procs"
#define GOTO_SET_NUM_THREADS       "goto_set_num_threads"
#define MKL_DOMAIN_GET_MAX_THREADS "MKL_Domain_Get_Max_Threads"
#define MKL_DOMAIN_SET_NUM_THREADS "MKL_Domain_Set_Num_Threads"
#define ACML_GET_MAX_THREADS       "acmlgetmaxthreads"
#define ACML_SET_NUM_THREADS       "acmlsetnumthreads"

static int  (*goto_get_num_procs)(void) = NULL;
static void (*goto_set_num_threads)(int) = NULL;
static int  (*mkl_domain_get_max_threads)(int) = NULL;
static int  (*mkl_domain_set_num_threads)(int,int) = NULL;
static int  (*acmlgetmaxthreads)(void) = NULL;
static void (*acmlsetnumthreads)(int) = NULL;

SEXP get_num_cores(void)
{
#if defined(__APPLE__)
  static int num = 0;
  int m[2];
  size_t len;
  SEXP n;

  PROTECT(n = allocVector(INTSXP, 1));
  
  if (num == 0) {
    sysctlbyname("hw.physicalcpu", &num, sizeof(int), NULL, 0);
    if ( num < 1 ){
      m[0] = CTL_HW;
      m[1] = HW_NCPU;
      len = sizeof(int);
      sysctl(m, 2, &num, &len, NULL, 0);
    }
  }
  INTEGER(n)[0]=num;
  UNPROTECT(1);
  return(n);
  
#elif defined(WIN32)
  static int num = 0;
  SEXP n;

  PROTECT(n = allocVector(INTSXP, 1));
  
  if (num == 0) {
    BOOL (WINAPI *glpi)(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);
    glpi = (void *) GetProcAddress( GetModuleHandle("kernel32"), "GetLogicalProcessorInformation");
    if (NULL != glpi) {
      DWORD len = 0;
      glpi(NULL, &len);
      {
        SYSTEM_LOGICAL_PROCESSOR_INFORMATION buf[len/sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION)];
        int i;
        glpi(buf, &len);
        for ( i=0; i<len/sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); i++)
          if( buf[i].Relationship == RelationProcessorCore )
            num++;
      }
    }else{
      SYSTEM_INFO sysinfo;    
      GetSystemInfo(&sysinfo);
      num = sysinfo.dwNumberOfProcessors;
      
    }
  }
  INTEGER(n)[0]=num;
  UNPROTECT(1);
  return(n);

#elif defined(__linux__)
#define P_CPU_PATH "/sys/devices/system/cpu/cpu%d/topology/physical_package_id"
#define C_CPU_PATH "/sys/devices/system/cpu/cpu%d/topology/core_id"
  static int num = 0;
  static int *cpu_table;
  int i;
  FILE *pfd, *cfd;
  char pbuf[128];
  char cbuf[128];
  int  key;
  int  seek=0;
  SEXP n;

  PROTECT(n = allocVector(INTSXP, 1));

  if (!num){
    num = get_nprocs();
    cpu_table=malloc(num*sizeof(int));
    memset((void*)cpu_table, 0xff, num*sizeof(int));
    for (seek=0,i=0 ; i<num ; i++){
      sprintf(pbuf,P_CPU_PATH,i);
      sprintf(cbuf,C_CPU_PATH,i);
      if(NULL==(pfd=fopen(pbuf,"r")))break;
      if(NULL==(cfd=fopen(cbuf,"r")))break;
      fgets(pbuf,sizeof(pbuf),pfd);
      fgets(cbuf,sizeof(cbuf),cfd);
      key = (atoi(pbuf)<<8) + atoi(cbuf);
      
      for(seek=0; seek<num && cpu_table[seek]!=-1; seek++){
        if(cpu_table[seek]==key){
          break;
        }
      }
      
      if (cpu_table[seek]==-1){
        cpu_table[seek]=key;
      }
      fclose(cfd);
      fclose(pfd);
    }
    
    for (seek=0 ; seek<num && cpu_table[seek]!=-1; seek++);
    num=seek;
    
    free((void*)cpu_table);
  }
  INTEGER(n)[0]=num; 
  UNPROTECT(1);
  return (n);


#elif ( defined( __FreeBSD__ ) || defined ( __NetBSD__ ) )
  static int num = 0;
  int m[2];
  size_t len;

  PROTECT(n = allocVector(INTSXP, 1));

  if (num == 0) {
    m[0] = CTL_HW;
    m[1] = HW_NCPU;
    len = sizeof(int);
    sysctl(m, 2, &num, &len, NULL, 0);
  }

  INTEGER(n)[0]=num;
  UNPROTECT(1);
  return (n);
#else
  SEXP n;
  PROTECT(n = allocVector(INTSXP, 1));

  INTEGER(n)[0]=1; 
  UNPROTECT(1);
  return(n);
#endif
}

SEXP get_num_procs(void)
{
#if defined(__APPLE__)
  static int num = 0;
  int m[2];
  size_t len;
  SEXP n;

  PROTECT(n = allocVector(INTSXP, 1));
  
  if (num == 0) {
    m[0] = CTL_HW;
    m[1] = HW_NCPU;
    len = sizeof(int);
    sysctl(m, 2, &num, &len, NULL, 0);
  }
  INTEGER(n)[0]=num;
  UNPROTECT(1);
  return(n);
  
#elif defined(WIN32)
  static int num = 0;
  SEXP n;

  PROTECT(n = allocVector(INTSXP, 1));
  
  if (num == 0) {
    SYSTEM_INFO sysinfo;    
    GetSystemInfo(&sysinfo);
    num = sysinfo.dwNumberOfProcessors;
  }
  INTEGER(n)[0]=num;
  UNPROTECT(1);
  return(n);

#elif defined(__linux__)
  static int num = 0;
  SEXP n;

  PROTECT(n = allocVector(INTSXP, 1));

  if (!num){
    num = get_nprocs();
  }
  INTEGER(n)[0]=num; 
  UNPROTECT(1);
  return (n);

#elif ( defined( __FreeBSD__ ) || defined ( __NetBSD__ ) )
  static int num = 0;
  int m[2];
  size_t len;

  PROTECT(n = allocVector(INTSXP, 1));

  if (num == 0) {
    m[0] = CTL_HW;
    m[1] = HW_NCPU;
    len = sizeof(int);
    sysctl(m, 2, &num, &len, NULL, 0);
  }

  INTEGER(n)[0]=num;
  UNPROTECT(1);
  return (n);
#else
  SEXP n;
  PROTECT(n = allocVector(INTSXP, 1));

  INTEGER(n)[0]=1; 
  UNPROTECT(1);
  return(n);
#endif
}


SEXP blas_set_num_threads(SEXP num)
{
  void *dlh = DLOPEN();

  if(dlh==NULL){
    return R_NilValue;
  }

  if(       NULL != ( goto_set_num_threads =
		      (void(*)(int))   DLSYM(dlh, GOTO_SET_NUM_THREADS))){
    goto_set_num_threads(INTEGER(num)[0]);
  }
  else if( NULL != ( mkl_domain_set_num_threads =
		      (int(*)(int,int))DLSYM(dlh, MKL_DOMAIN_SET_NUM_THREADS))){
    mkl_domain_set_num_threads(INTEGER(num)[0],MKL_BLAS);
  }
  else if( NULL != ( acmlsetnumthreads =
		      (void(*)(int))   DLSYM(dlh, ACML_SET_NUM_THREADS))){
    acmlsetnumthreads(INTEGER(num)[0]);
  }
  /*
  #ifdef _OPENMP
    else{
      omp_set_num_threads(INTEGER(num)[0]);
    }
  #else
  #endif
  */
  DLCLOSE(dlh);
  return R_NilValue;
}

SEXP blas_get_num_procs(void)
{
  SEXP n;
  void *dlh = DLOPEN();

  if(dlh==NULL){
    return R_NilValue;
  }

  PROTECT(n = allocVector(INTSXP, 1));
  INTEGER(n)[0]=1;

  if(       NULL != ( goto_get_num_procs           =
		      (int(*)(void))   DLSYM(dlh, GOTO_GET_NUM_PROCS))){
    INTEGER(n)[0]=goto_get_num_procs();
  }
  else if( NULL != ( mkl_domain_get_max_threads   =
		     (int(*)(int))   DLSYM(dlh, MKL_DOMAIN_GET_MAX_THREADS))){
    INTEGER(n)[0]=mkl_domain_get_max_threads(MKL_BLAS);
  }
  else if( NULL != (acmlgetmaxthreads            =
		    (int(*)(void))   DLSYM(dlh, ACML_GET_MAX_THREADS))){
    INTEGER(n)[0]=acmlgetmaxthreads();
  }
  /*
  #ifdef _OPENMP
    else{
      INTEGER(n)[0]=omp_get_max_threads();
    }
  #endif
  */
  DLCLOSE(dlh);
  UNPROTECT(1);
  return (n);
}

SEXP Rhpc_omp_set_num_threads(SEXP num)
{
  int n = INTEGER(num)[0];
#ifdef _OPENMP
  omp_set_num_threads(n);
#else
#endif
  return(R_NilValue);
}

SEXP Rhpc_omp_get_num_procs(void)
{
#ifdef _OPENMP
  SEXP n;
  PROTECT(n = allocVector(INTSXP, 1));

  INTEGER(n)[0]=omp_get_num_procs();
  UNPROTECT(1);
  return (n);
#else
  return(R_NilValue);
#endif
}

SEXP Rhpc_omp_get_max_threads(void)
{
#ifdef _OPENMP
  SEXP n;
  PROTECT(n = allocVector(INTSXP, 1));

  INTEGER(n)[0]=omp_get_max_threads();
  UNPROTECT(1);
  return (n);
#else
  return(R_NilValue);
#endif
}

