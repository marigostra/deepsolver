
AM_CXXFLAGS = $(DEEPSOLVER_CXXFLAGS) $(DEEPSOLVER_INCLUDES)
LIBS += -lrpm -lcurl -lminisat
LIBdir=${libdir}
LDFLAGS=-version-info ${LT_CURRENT}:$(LT_REVISION):$(LT_AGE) -release $(LT_RELEASE)

LIB_LTLIBRARIES = libdeepsolver.la

libdeepsolver_ladir = $(includedir)

libdeepsolver_la_SOURCES = \
ConfigCenter.cpp \
ConfigFile.cpp \
IndexCore.cpp \
FilesFetch.cpp \
InfoCore.cpp \
io/PackageScopeContentBuilder.cpp \
io/PackageScopeContentLoader.cpp \
logging.cpp \
OperationCore.cpp \
os/Directory.cpp \
os/File.cpp \
Pkg.cpp \
PkgUrlsFile.cpp \
PkgUtils.cpp \
repo/InfoFileReader.cpp \
repo/PkgSection.cpp \
repo/RepoParams.cpp \
Repository.cpp \
repo/TextFormatSectionReader.cpp \
rpm/RpmBackEnd.cpp \
rpm/RpmFileHeaderReader.cpp \
rpm/rpmHeader.cpp \
rpm/RpmInstalledPackagesIterator.cpp \
transact/InstalledReferences.cpp \
transact/LibMinisatSolver.cpp \
transact/PackageScopeContent.cpp \
transact/PackageScope.cpp \
transact/PackageSelection.cpp \
transact/PostponedRequire.cpp \
transact/ProvideMap.cpp \
transact/ProvidePriorityList.cpp \
transact/SatWriter.cpp \
transact/TaskSolver.cpp \
transact/TaskTranslator.cpp \
transact/TaskTranslatorOutputOptimization.cpp \
TransactionIterator.cpp \
utils/CurlInterface.cpp \
utils/GzipInterface.cpp \
utils/Md5.cpp \
utils/Md5File.cpp \
utils/RegExp.cpp \
utils/StringUtils.cpp \
utils/TinyFileDownload.cpp

libdeepsolver_la_HEADERS = \
AbstractPackageBackEnd.h \
AbstractOperationContinueRequest.h \
AbstractFetchListener.h \
AbstractPackageRecipient.h \
ConfigCenter.h \
ConfigData.h \
ConfigFile.h \
DeepsolverExceptions.h \
deepsolver.h \
DeepsolverTypes.h \
DefaultValues.h \
IndexCore.h \
FilesFetch.h \
InfoCore.h \
io/PackageScopeContentBuilder.h \
io/PackageScopeContentLoader.h \
logging.h \
OperationCore.h \
os/Directory.h \
os/File.h \
os/system.h \
PackageInfoProcessor.h \
Pkg.h \
PkgUrlsFile.h \
PkgUtils.h \
repo/InfoFileReader.h \
repo/PkgSection.h \
repo/RepoParams.h \
Repository.h \
repo/TextFormatSectionReader.h \
rpm/RpmBackEnd.h \
rpm/RpmFileHeaderReader.h \
rpm/rpmHeader.h \
rpm/RpmInstalledPackagesIterator.h \
transact/AbstractPackageScope.h \
transact/AbstractSatSolver.h \
transact/AbstractTaskSolver.h \
transact/AbstractTaskTranslatorOutput.h \
transact/InstalledReferences.h \
transact/PackageNameSorting.h \
transact/PackageScopeContent.h \
transact/PackageScope.h \
transact/PackageSelection.h \
transact/PostponedRequire.h \
transact/ProvideMap.h \
transact/ProvidePriorityList.h \
transact/SatWriter.h \
transact/TaskSolver.h \
transact/TaskTranslator.h \
transact/TaskTranslatorOutputOptimization.h \
TransactionIterator.h \
utils/CurlInterface.h \
utils/DateTime.h \
utils/doubling.h \
utils/GzipInterface.h \
utils/Md5File.h \
utils/Md5.h \
utils/RegExp.h \
utils/StringUtils.h \
utils/TinyFileDownload.h

