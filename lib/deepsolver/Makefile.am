
AM_CXXFLAGS = $(DEEPSOLVER_CXXFLAGS) $(DEEPSOLVER_INCLUDES)
LIBS += -lrpm -lcurl -lminisat
LIBdir=${libdir}
LDFLAGS=-version-info ${LT_CURRENT}:$(LT_REVISION):$(LT_AGE) -release $(LT_RELEASE)

LIB_LTLIBRARIES = libdeepsolver.la

libdeepsolver_ladir = $(includedir)

libdeepsolver_la_SOURCES = \
CliParser.cpp \
ConfigAdapter.cpp \
ConfigCenter.cpp \
ConfigFile.cpp \
CurlInterface.cpp \
Directory.cpp \
ExceptionMessagesEn.cpp \
exceptions.cpp \
File.cpp \
FilesFetch.cpp \
GzipInterface.cpp \
IndexCore.cpp \
InfoCore.cpp \
InfoFileReader.cpp \
logging.cpp \
Md5.cpp \
Md5File.cpp \
MinisatSolver.cpp \
OperationCore.cpp \
OsIntegrity.cpp \
PkgScopeBase.cpp \
PkgScope.cpp \
PkgScopeMetadata.cpp \
PkgSection.cpp \
PkgSnapshot.cpp \
PkgUrlsFile.cpp \
Process.cpp \
RegExp.cpp \
RepoParams.cpp \
Repository.cpp \
RpmBackEnd.cpp \
RpmFileHeaderReader.cpp \
rpmHeader.cpp \
RpmInstalledPackagesIterator.cpp \
RpmTransaction.cpp \
Sat.cpp \
Solver.cpp \
StringUtils.cpp \
TextFormatSectionReader.cpp \
TinyFileDownload.cpp \
TransactionIterator.cpp

libdeepsolver_la_HEADERS = \
AbstractContinueRequest.h \
AbstractFetchListener.h \
AbstractPkgBackEnd.h \
AbstractPkgRecipient.h \
AbstractTaskSolver.h \
AbstractTransactionListener.h \
AutoReleaseStrings.h \
CliParser.h \
ConfigAdapter.h \
ConfigCenter.h \
ConfigData.h \
ConfigFile.h \
config.h \
CurlInterface.h \
deepsolver.h \
Directory.h \
ExceptionMessagesEn.h \
exceptions.h \
File.h \
FilesFetch.h \
GzipInterface.h \
IndexCore.h \
InfoCore.h \
InfoFileReader.h \
InlineIO.h \
logging.h \
Md5File.h \
Md5.h \
MinisatCallData.h \
MinisatSolver.h \
OperationCore.h \
OsIntegrity.h \
Pkg.h \
PkgInfoProcessor.h \
PkgScopeBase.h \
PkgScope.h \
PkgScopeMetadata.h \
PkgSection.h \
PkgSnapshot.h \
PkgUrlsFile.h \
Process.h \
RegExp.h \
RepoParams.h \
Repository.h \
RpmBackEnd.h \
RpmFileHeaderReader.h \
rpmHeader.h \
RpmInstalledPackagesIterator.h \
RpmTransaction.h \
Sat.h \
SolverBase.h \
Solver.h \
StringUtils.h \
system.h \
TextFormatSectionReader.h \
TinyFileDownload.h \
TransactionIterator.h \
types.h \
utils.h
