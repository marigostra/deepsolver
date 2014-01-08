/*
 * This file now provides only support for English language messages but
 * generally multilingual support implies.
 */

#include"deepsolver/deepsolver.h"
#include"Messages.h"

DEEPSOLVER_BEGIN_NAMESPACE

std::string messagesProgramName;


//Command line errors;

void Messages::onMissedProgramName() const
{
  m_stream << messagesProgramName << ":The command line doesn\'t contain program name" << std::endl;
}

void Messages::onMissedCommandLineArgument(const std::string& arg) const
{
  m_stream << messagesProgramName << ":The command line argument \'" << arg << "\' requires additional parameter" << std::endl;
}

// User input errors;

void Messages::onNoPackagesMentionedError() const
{
  m_stream << messagesProgramName << ":No packages mentioned" << std::endl;
}

// ds-update;

void Messages::dsUpdateLogo() const
{
  m_stream << "ds-update: the utility to fetch repository headers" << std::endl;
  m_stream << "Version: " << PACKAGE_VERSION << std::endl;
  m_stream << std::endl;
}

void Messages::dsUpdateInitCliParser(CliParser& cliParser) const
{
  cliParser.addKeyDoubleName("-h", "--help", "print this help screen and exit");
  cliParser.addKey("--log", "print log to console instead of user progress information");
  cliParser.addKey("--debug", "relax filtering level for log output");
}

void Messages::dsUpdateHelp(const CliParser& cliParser) const
{
  dsUpdateLogo();
  m_stream << "Usage: ds-update [OPTIONS]" << std::endl;
  m_stream << std::endl;
  m_stream << "Valid command line options are:" << std::endl;
  cliParser.printHelp(m_stream);
}

void Messages::introduceRepoSet(const ConfigCenter& conf) const
{
  for(ConfRepoVector::size_type i = 0;i < conf.root().repo.size();i++)
    {
      const ConfRepo& repo = conf.root().repo[i];
      std::cout << "Repo: " << repo.name << " (" << repo.url << ")" << std::endl;
      std::cout << "Arch:";
      for(StringVector::size_type k = 0;k < repo.arch.size();k++)
	std::cout << " " << repo.arch[k];
      std::cout << std::endl;
      std::cout << "Components:";
      for(StringVector::size_type k = 0;k < repo.components.size();k++)
	std::cout << " " << repo.components[k];
      std::cout << std::endl;
      std::cout << std::endl;
    }
}

// ds-install;

void Messages::dsInstallLogo() const
{
  m_stream << "ds-install: the Deepsolver utility for package installation" << std::endl;
  m_stream << "Version: " << PACKAGE_VERSION << std::endl;
  m_stream << std::endl;
}

void Messages::dsInstallInitCliParser(CliParser& cliParser) const
{
  cliParser.addKeyDoubleName("-n", "--dry-run", "print a solution and do nothing");
  cliParser.addKeyDoubleName("-u", "--urls", "print URLs of packages for installation and do nothing");
  cliParser.addKeyDoubleName("-f", "--files", "fetch packages and print file names");
  cliParser.addKeyDoubleName("-s", "--sat", "print SAT equation and do not touch any packages");
  cliParser.addKeyDoubleName("-h", "--help", "print this help screen and exit");
  cliParser.addKey("--log", "print log to console instead of user progress information");
  cliParser.addKey("--debug", "relax filtering level for log output");
}

void Messages::dsInstallHelp(const CliParser& cliParser) const
{
  dsInstallLogo();
  m_stream << "Usage: ds-install [OPTIONS] [PKG1 [(<|<=|=|>=|>) VERSION] [...]]" << std::endl;
  m_stream << std::endl;
  m_stream << "Valid command line options are:" << std::endl;
  cliParser.printHelp(m_stream);
}

// ds-remove;

void Messages::dsRemoveLogo() const
{
  m_stream << "ds-remove: the Deepsolver utility for package removing" << std::endl;
  m_stream << "Version: " << PACKAGE_VERSION << std::endl;
  m_stream << std::endl;
}

void Messages::dsRemoveInitCliParser(CliParser& cliParser) const
{
  cliParser.addKeyDoubleName("-n", "--dry-run", "print a solution and do nothing");
  cliParser.addKeyDoubleName("-u", "--urls", "print URLs of packages for installation and do nothing");
  cliParser.addKeyDoubleName("-f", "--files", "fetch packages and print file names");
  cliParser.addKeyDoubleName("-s", "--sat", "print SAT equation and do not touch any packages");
  cliParser.addKeyDoubleName("-h", "--help", "print this help screen and exit");
  cliParser.addKey("--log", "print log to console instead of user progress information");
  cliParser.addKey("--debug", "relax filtering level for log output");
}

void Messages::dsRemoveHelp(const CliParser& cliParser) const
{
  dsRemoveLogo();
  m_stream << "Usage: ds-remove [OPTIONS] [PKG1 [PKG2 [...]]]" << std::endl;
  m_stream << std::endl;
  m_stream << "Valid command line options are:" << std::endl;
  cliParser.printHelp(m_stream);
}

// ds-require;

void Messages::dsRequireLogo() const
{
  m_stream << "ds-require: the Deepsolver utility for require processing" << std::endl;
  m_stream << "Version: " << PACKAGE_VERSION << std::endl;
  m_stream << std::endl;
}

void Messages::dsRequireInitCliParser(CliParser& cliParser) const
{
  cliParser.addKeyDoubleName("-h", "--help", "print this help screen and exit");
  cliParser.addKey("--log", "print log to console instead of user progress information");
  cliParser.addKey("--debug", "relax filtering level for log output");
}

void Messages::dsRequireHelp(const CliParser& cliParser) const
{
  dsRequireLogo();
  m_stream << "Usage: ds-require [OPTIONS] PKG [(<|<=|=|>=|>) VERSION]" << std::endl;
  m_stream << std::endl;
  m_stream << "Valid command line options are:" << std::endl;
  cliParser.printHelp(m_stream);
}

void Messages::dsRequireOnInvalidInput()
{
  dsRequireLogo();
  m_stream << "You should provide a proper require entry. For example valid input may be:" << std::endl;
    m_stream << "ds-require foo" << std::endl;
    m_stream << "ds-require foo = 1.0" << std::endl;
    m_stream << "ds-require foo >= 1.0" << std::endl;
    m_stream << "..." << std::endl;
}

// ds-snapshot;

void Messages::dsSnapshotLogo() const
{
  m_stream << "ds-snapshot: the Deepsolver utility to view package scope" << std::endl;
  m_stream << "Version: " << PACKAGE_VERSION << std::endl;
  m_stream << std::endl;
}

void Messages::dsSnapshotInitCliParser(CliParser& cliParser) const
{
  cliParser.addKeyDoubleName("-i", "--installed", "insert installed packages to scope");
  cliParser.addKey("--ids", "print internal package IDs");
  cliParser.addKeyDoubleName("-h", "--help", "print this help screen and exit");
  cliParser.addKey("--log", "print log to console instead of user progress information");
  cliParser.addKey("--debug", "relax filtering level for log output");
}

void Messages::dsSnapshotHelp(const CliParser& cliParser) const
{
  dsSnapshotLogo();
  m_stream << "Usage: ds-snapshot [--installed] [--help] [--log [--debug]] " << std::endl;
  m_stream << std::endl;
  m_stream << "Valid command line options are:" << std::endl;
  cliParser.printHelp(m_stream);
}

bool Messages::confirmContinuing()
{
  m_stream << "Do you really agree to continue? (y/N): ";
  std::string str;
  std::getline(std::cin, str);
  if (str == "y" || str == "Y")
    return 1;
  if (str == "yes" || str == "Yes" || str == "YES")
    return 1;
  return 0;
}

DEEPSOLVER_END_NAMESPACE
