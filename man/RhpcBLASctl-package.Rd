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
  Control the number of threads on BLAS for R
}
\description{
Control the number of threads on BLAS(Aka GotoBLAS, ACML and MKL).
--
GotoBLAS(generic) binary for Windows i386 http://prs.ism.ac.jp/~nakama/Survive
GotoBLAS2/binary/windows/x86/KATMAI/Rblas.dll
GotoBLAS(generic) binary for Windows x64 http://prs.ism.ac.jp/~nakama/SurviveG
otoBLAS2/binary/windows/x64/PRESCOTT/Rblas.dll

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
