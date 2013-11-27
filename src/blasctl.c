#include <R.h>
#include <Rinternals.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "blasctl.h"

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
  //#ifdef _OPENMP
  //  else{
  //    omp_set_num_threads(INTEGER(num)[0]);
  //  }
  //#else
  //#endif
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
  //#ifdef _OPENMP
  //  else{
  //    INTEGER(n)[0]=omp_get_max_threads();
  //  }
  //#endif
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

