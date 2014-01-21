/*
   Copyright 2011-2014 ALT Linux
   Copyright 2011-2014 Michael Pozhidaev

   This file is part of the Deepsolver.

   Deepsolver is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   Deepsolver is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
*/

#ifndef DEEPSOLVER_MESSAGES_H
#define DEEPSOLVER_MESSAGES_H

#include"deepsolver/ConfigCenter.h"
//#include"deepsolver/CurlInterface.h"
//#include"deepsolver/OperationCore.h"
#include"deepsolver/CliParser.h"

namespace Deepsolver
{
  class Messages
  {
  public:
    Messages(std::ostream& stream)
      : m_stream(stream) {}

    virtual ~Messages() {}

  public:
    //Errors;
    void onNoPkgMentionedError() const;

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
