#include <R.h>
#include <Rinternals.h>

#ifndef WIN32
#include "config.h"
#endif

#ifdef HAVE_SYS_SYSINFO_H
#include <sys/sysinfo.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_SYSCTL_H
#if !defined(__linux__)
#include <sys/sysctl.h>
#endif /* !defined(__linux__) */
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

#define    STR_GOTO_GET_NUM_PROCS         "goto_get_num_procs"
#define    STR_GOTO_SET_NUM_THREADS       "goto_set_num_threads"
#define    STR_MKL_DOMAIN_GET_MAX_THREADS "mkl_domain_get_max_threads"
#define    STR_MKL_DOMAIN_SET_NUM_THREADS "mkl_domain_set_num_threads"
#define    STR_MKL_GET_MAX_THREADS        "mkl_get_max_threads"
#define    STR_MKL_SET_NUM_THREADS        "mkl_set_num_threads"
#define    STR_ACMLGETMAXTHREADS          "acmlgetmaxthreads"
#define    STR_ACMLSETNUMTHREADS          "acmlsetnumthreads"
#define    STR_BLI_THREAD_GET_NUM_THREADS "bli_thread_get_num_threads"
#define    STR_BLI_THREAD_SET_NUM_THREADS "bli_thread_set_num_threads"

typedef int  (*GOTO_GET_NUM_PROCS)        (void);
typedef void (*GOTO_SET_NUM_THREADS)      (int);
typedef int  (*MKL_DOMAIN_GET_MAX_THREADS)(int*);
typedef int  (*MKL_DOMAIN_SET_NUM_THREADS)(int*,int*);
typedef int  (*MKL_GET_MAX_THREADS)       (void);
typedef void (*MKL_SET_NUM_THREADS)       (int*);
typedef int  (*ACMLGETMAXTHREADS)         (void);
typedef void (*ACMLSETNUMTHREADS)         (int);
typedef int  (*BLI_THREAD_GET_NUM_THREADS)(void);
typedef void (*BLI_THREAD_SET_NUM_THREADS)(int);
 
static         GOTO_GET_NUM_PROCS          goto_get_num_procs          = NULL;
static         GOTO_SET_NUM_THREADS        goto_set_num_threads        = NULL;
static         MKL_DOMAIN_GET_MAX_THREADS  mkl_domain_get_max_threads  = NULL;
static         MKL_DOMAIN_SET_NUM_THREADS  mkl_domain_set_num_threads  = NULL;
static         MKL_GET_MAX_THREADS         mkl_get_max_threads         = NULL;
static         MKL_SET_NUM_THREADS         mkl_set_num_threads         = NULL;
static         ACMLGETMAXTHREADS           acmlgetmaxthreads           = NULL;
static         ACMLSETNUMTHREADS           acmlsetnumthreads           = NULL;
static         BLI_THREAD_GET_NUM_THREADS  bli_thread_get_num_threads  = NULL;
static         BLI_THREAD_SET_NUM_THREADS  bli_thread_set_num_threads  = NULL;


#ifdef WIN32
typedef BOOL (WINAPI *GLPI)(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);
#endif

SEXP get_num_cores(void)
{
#if  (defined(__APPLE__) || defined( __FreeBSD__ ) || defined ( __NetBSD__ ) )
  static int num = 0;
  int m[2];
  size_t len;
  size_t size=sizeof(int);
  SEXP n;

  PROTECT(n = allocVector(INTSXP, 1));
  
  if (num == 0) {
#ifdef  HAVE_SYSCTLBYNAME
    sysctlbyname("hw.physicalcpu", &num, &size, NULL, 0);
#endif
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
    //BOOL (WINAPI *glpi)(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);
    GLPI glpi;
    glpi = (GLPI) GetProcAddress( GetModuleHandle("kernel32"), "GetLogicalProcessorInformation");
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
  char *rcbuf=NULL;

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
      rcbuf=fgets(pbuf,sizeof(pbuf),pfd);
      rcbuf=fgets(cbuf,sizeof(cbuf),cfd);
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
    if(seek>1) num=seek;
    
    free((void*)cpu_table);
  }
  INTEGER(n)[0]=num; 
  UNPROTECT(1);
  return (n);

#elif defined(HAVE_GET_NPROCS)
  static int num = 0;
  SEXP n;

  PROTECT(n = allocVector(INTSXP, 1));

  if (!num){
    num = get_nprocs();
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
#if (defined(__APPLE__) || defined( __FreeBSD__ ) || defined ( __NetBSD__ ) )
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

#elif defined(HAVE_GET_NPROCS)
  static int num = 0;
  SEXP n;

  PROTECT(n = allocVector(INTSXP, 1));

  if (!num){
    num = get_nprocs();
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
  int   threads = (XLENGTH(num)>0) ? INTEGER(num)[0] : 1;

  if(dlh==NULL){
    error("Failed to acquire BLAS handle.");
    return R_NilValue;
  }

  if (threads < 1) threads=1; /* minimum 1 */
  
  if(      NULL != ( EXPDLSYM(goto_set_num_threads,       GOTO_SET_NUM_THREADS,       dlh, STR_GOTO_SET_NUM_THREADS))){
    goto_set_num_threads(threads);
  }
  else if( NULL != ( EXPDLSYM(mkl_domain_set_num_threads, MKL_DOMAIN_SET_NUM_THREADS, dlh, STR_MKL_DOMAIN_SET_NUM_THREADS))){
    int   type = MKL_BLAS;
    mkl_domain_set_num_threads(&threads,&type);
  }
  else if(NULL != ( EXPDLSYM(mkl_set_num_threads,         MKL_SET_NUM_THREADS,        dlh, STR_MKL_SET_NUM_THREADS))){
    mkl_set_num_threads(&threads);
  }
  else if( NULL != ( EXPDLSYM(acmlsetnumthreads,          ACMLSETNUMTHREADS,          dlh, STR_ACMLSETNUMTHREADS))){
    acmlsetnumthreads(threads);
  }
  else if( NULL != ( EXPDLSYM(bli_thread_set_num_threads, BLI_THREAD_SET_NUM_THREADS, dlh, STR_BLI_THREAD_SET_NUM_THREADS))){
    bli_thread_set_num_threads(threads);
  }

  DLCLOSE(dlh);
  return R_NilValue;
}

SEXP blas_get_num_procs(void)
{
  SEXP n;
  void *dlh = DLOPEN();

  if(dlh==NULL){
    error("Failed to acquire BLAS handle.");
    return R_UnboundValue;
  }

  PROTECT(n = allocVector(INTSXP, 1));
  INTEGER(n)[0]=1;

  if(      NULL != ( EXPDLSYM(goto_get_num_procs,         GOTO_GET_NUM_PROCS,         dlh, STR_GOTO_GET_NUM_PROCS))){
    INTEGER(n)[0]=goto_get_num_procs();
  }
  else if( NULL != ( EXPDLSYM(mkl_domain_get_max_threads, MKL_DOMAIN_GET_MAX_THREADS, dlh, STR_MKL_DOMAIN_GET_MAX_THREADS))){
    int type = MKL_BLAS;
    INTEGER(n)[0]=mkl_domain_get_max_threads(&type);
  }
  else if( NULL != ( EXPDLSYM(mkl_get_max_threads,        MKL_GET_MAX_THREADS,        dlh, STR_MKL_GET_MAX_THREADS))){
    INTEGER(n)[0]=mkl_get_max_threads();
  }
  else if( NULL != ( EXPDLSYM(acmlgetmaxthreads,          ACMLGETMAXTHREADS,          dlh, STR_ACMLGETMAXTHREADS))){
    INTEGER(n)[0]=acmlgetmaxthreads();
  }
  else if( NULL != ( EXPDLSYM(bli_thread_get_num_threads, BLI_THREAD_GET_NUM_THREADS, dlh, STR_BLI_THREAD_GET_NUM_THREADS))){
    INTEGER(n)[0]=bli_thread_get_num_threads();
  }

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
  SEXP n;
  PROTECT(n = allocVector(INTSXP, 1));
#ifdef _OPENMP
  INTEGER(n)[0]=omp_get_num_procs();
#else
  INTEGER(n)[0]=NA_INTEGER;
#endif
  UNPROTECT(1);
  return (n);
}

SEXP Rhpc_omp_get_max_threads(void)
{
  SEXP n;
  PROTECT(n = allocVector(INTSXP, 1));
#ifdef _OPENMP
  INTEGER(n)[0]=omp_get_max_threads();
#else
  INTEGER(n)[0]=NA_INTEGER;
#endif
  UNPROTECT(1);
  return (n);
}

