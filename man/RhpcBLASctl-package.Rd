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
--
'GotoBLAS(generic)' binary for Windows i386 http://prs.ism.ac.jp/~nakama/Survive
GotoBLAS2/binary/windows/x86/KATMAI/Rblas.dll
'GotoBLAS(generic)' binary for Windows x64 http://prs.ism.ac.jp/~nakama/SurviveG
otoBLAS2/binary/windows/x64/PRESCOTT/Rblas.dll

get_num_cores is number of physical cores returned, when feasible.
get_num_procs is number of logical processor returned.
}
\details{
  \url{http://prs.ism.ac.jp/~nakama/Rhpc}
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
