\name{RhpcBLASctl-package}
\alias{RhpcBLASctl-package}
\alias{get_num_cores}
\alias{get_num_procs}
\alias{blas_get_num_procs}
\alias{blas_set_num_threads}
\alias{omp_get_num_procs}
\alias{omp_get_max_threads}
\alias{omp_set_num_threads}
\usage{
get_num_cores()
get_num_procs()
blas_get_num_procs()
blas_set_num_threads(threads)
omp_get_num_procs()
omp_get_max_threads()
omp_set_num_threads(threads)
}
\arguments{
  \item{threads}{Number of threads}
}
\docType{package}
\title{
  Control the number of threads on BLAS
}
\description{
Control the number of threads on 'BLAS' (Aka 'GotoBLAS', 'ACML' and 'MKL').

If you build R directly on Windows and the BLAS file name is not
'Rblas.dll', you can give it as 'options ("RhpcBLASctl.win.Rblas.name" = "boofoo.dll")'.

openmp-related functions only work at build-time instructions.
Omp_get * now returns NA_Integer if R was not built with openmp instructions.
NOTE: if R itself is built without OpenMP instructions and BLAS thread
is used with OpenMP, this package will not work...XD

}
\value{
  \item{get_num_cores}{Number of CPU physical processor.
    NOTE:Returns 1L if unknown}
  \item{get_num_procs}{Number of CPU logical processor.
    NOTE:Returns 1L if unknown}
  \item{blas_get_num_procs}{Number of available cores for the optimized
    BLAS.
    NOTE:Returns 1L if unknown}
  \item{omp_get_num_procs}{Result from OpenMP's omp_get_num_procs
    function.
    NOTE Returns NA_integer_ if unknown}
  \item{omp_get_max_threads}{Result from OpenMP's omp_get_max_threads
    function.
    NOTE Returns NA_integer_ if unknown}
}
\details{
  \url{https://prs.ism.ac.jp/~nakama/Rhpc}
}
\author{
  Junji NAKANO <nakanoj@ism.ac.jp> and Ei-ji Nakama<nakama@com-one.com>
  Maintainer: Ei-ji Nakama <nakama@com-one.com>
}
\keyword{utilities}
\examples{
get_num_cores()
get_num_procs()
blas_get_num_procs()
blas_set_num_threads(2)
omp_get_num_procs()
omp_get_max_threads()
omp_set_num_threads(2)        
}
