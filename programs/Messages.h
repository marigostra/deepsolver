
#ifndef DEEPSOLVER_MESSAGES_H
#define DEEPSOLVER_MESSAGES_H

#include"deepsolver/ConfigCenter.h"
#include"deepsolver/CurlInterface.h"
#include"deepsolver/OperationCore.h"
#include"CliParser.h"

namespace Deepsolver
{
  extern std::string messagesProgramName;

  class Messages
  {
  public:
    Messages(std::ostream& stream)
      : m_stream(stream) {}

    virtual ~Messages() {}

  public:
    //Command line errors;
    void onMissedProgramName() const;
    void onMissedCommandLineArgument(const std::string& arg) const;

    //User input errors;
    void onNoPackagesMentionedError() const;

    //ds-update;
    void dsUpdateLogo() const;
    void dsUpdateInitCliParser(CliParser& cliParser) const;
    void dsUpdateHelp(const CliParser& cliParser) const;
    void introduceRepoSet(const ConfigCenter& conf) const;

    //ds-install;
    void dsInstallLogo() const;
    void dsInstallInitCliParser(CliParser& cliParser) const;
    void dsInstallHelp(const CliParser& cliParser) const;

    //ds-remove;
    void dsRemoveLogo() const;
    void dsRemoveInitCliParser(CliParser& cliParser) const;
    void dsRemoveHelp(const CliParser& cliParser) const;

    //ds-require;
    void dsRequireLogo() const;
    void dsRequireInitCliParser(CliParser& cliParser) const;
    void dsRequireHelp(const CliParser& cliParser) const;
    void dsRequireOnInvalidInput();

    //ds-snapshot;
    void dsSnapshotLogo() const;
    void dsSnapshotInitCliParser(CliParser& cliParser) const;
    void dsSnapshotHelp(const CliParser& cliParser) const;

    //Dialogs;
    bool confirmContinuing();

  private:
    std::ostream& m_stream;
  }; //class Messages;
} //namespace Deepsolver;

#endif //DEEPSOLVER_MESSAGES_H;
