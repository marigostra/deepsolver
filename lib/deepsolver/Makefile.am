
AM_CXXFLAGS = $(DEEPSOLVER_CXXFLAGS) $(DEEPSOLVER_INCLUDES)
LIBS += -lrpm -lcurl -lminisat
LIBdir=${libdir}
LDFLAGS=-version-info ${LT_CURRENT}:$(LT_REVISION):$(LT_AGE) -release $(LT_RELEASE)

LIB_LTLIBRARIES = libdeepsolver.la

libdeepsolver_ladir = $(includedir)

libdeepsolver_la_SOURCES = \
ConfigAdapter.cpp \
ConfigCenter.cpp \
ConfigFile.cpp \
CurlInterface.cpp \
Directory.cpp \
File.cpp \
FilesFetch.cpp \
GzipInterface.cpp \
IndexCore.cpp \
InfoCore.cpp \
InfoFileReader.cpp \
InstalledReferences.cpp \
LibMinisatSolver.cpp \
logging.cpp \
Md5.cpp \
Md5File.cpp \
OperationCore.cpp \
PackageSelection.cpp \
Pkg.cpp \
PkgScopeBase.cpp \
PkgScope.cpp \
PkgSection.cpp \
PkgSnapshot.cpp \
PkgUrlsFile.cpp \
PkgUtils.cpp \
PostponedRequire.cpp \
Process.cpp \
ProvideMap.cpp \
ProvidePriorityList.cpp \
RegExp.cpp \
RepoParams.cpp \
Repository.cpp \
RpmBackEnd.cpp \
RpmFileHeaderReader.cpp \
rpmHeader.cpp \
RpmInstalledPackagesIterator.cpp \
RpmTransaction.cpp \
SatWriter.cpp \
StringUtils.cpp \
TaskSolver.cpp \
TaskTranslator.cpp \
TaskTranslatorOutputOptimization.cpp \
TextFormatSectionReader.cpp \
TinyFileDownload.cpp \
TransactionIterator.cpp

libdeepsolver_la_HEADERS = \
AbstractFetchListener.h \
AbstractOperationContinueRequest.h \
AbstractPackageBackEnd.h \
AbstractPackageRecipient.h \
AbstractPackageScope.h \
AbstractSatSolver.h \
AbstractTaskSolver.h \
AbstractTaskTranslatorOutput.h \
AutoReleaseStrings.h \
ConfigAdapter.h \
ConfigCenter.h \
ConfigData.h \
ConfigFile.h \
config.h \
CurlInterface.h \
DateTime.h \
deepsolver.h \
Dichotomy.h \
Directory.h \
doubling.h \
exceptions.h \
File.h \
FilesFetch.h \
GzipInterface.h \
IndexCore.h \
InfoCore.h \
InfoFileReader.h \
InlineIO.h \
InstalledReferences.h \
logging.h \
Md5File.h \
Md5.h \
OperationCore.h \
PackageInfoProcessor.h \
PackageNameSorting.h \
PackageSelection.h \
Pkg.h \
PkgScopeBase.h \
PkgScope.h \
PkgSection.h \
PkgSnapshot.h \
PkgUrlsFile.h \
PkgUtils.h \
PostponedRequire.h \
Process.h \
ProvideMap.h \
ProvidePriorityList.h \
RegExp.h \
RepoParams.h \
Repository.h \
RpmBackEnd.h \
RpmFileHeaderReader.h \
rpmHeader.h \
RpmInstalledPackagesIterator.h \
RpmTransaction.h \
SatWriter.h \
StringUtils.h \
system.h \
TaskSolver.h \
TaskTranslator.h \
TaskTranslatorOutputOptimization.h \
TextFormatSectionReader.h \
TinyFileDownload.h \
TransactionIterator.h \
types.h
