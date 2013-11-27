\name{RhpcBLASctl-package}
\alias{RhpcBLASctl-package}
\alias{blas_get_num_procs}
\alias{blas_set_num_threads}
\alias{omp_get_num_procs}
\alias{omp_get_max_threads}
\alias{omp_set_num_threads}
\usage{
blas_get_num_procs()
blas_set_num_threads(threads)
omp_get_num_procs()
omp_get_max_threads()
omp_set_num_threads(threads)
}
\arguments{
  \item{threads}{number of threads}
}
\docType{package}
\title{
  A package for High-Performance Computing
}
\description{
  R HPC Interfaces
}
\details{
  \url{http://prs.ism.ac.jp/~nakama/Rhpc}
}
\author{
  Junji Nakano <nakanoj@ism.ac.jp> and Ei-ji Nakama<nakama@com-one.com>
  Maintainer: Ei-ji Nakama <nakama@com-one.com>
}
\keyword{utilities}
\examples{
\dontrun{
	blas_get_num_procs()
	blas_set_num_threads(2)
}
}
