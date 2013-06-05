/*
   Copyright 2011-2013 ALT Linux
   Copyright 2011-2013 Michael Pozhidaev

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

#include"deepsolver/deepsolver.h"

DEEPSOLVER_BEGIN_NAMESPACE

int Process::runSync(const std::string& bin, const StringVector& args)
{
  std::string cmdLine= bin + "";
  for (StringVector::size_type i = 0;i < args.size();i++)
    cmdLine += " \'" + args[i] + "\'";
  logMsg(LOG_DEBUG, "os:spawning external process in sync mode: %s", cmdLine.c_str());
  pid_t pid = fork();
  if (pid == (pid_t)-1)
    SYS_STOP("fork()");
  if (pid == (pid_t)0)
    {
      const int fd = ::open("/dev/null", O_RDONLY);
      if (fd < 0)
	exit(EXIT_FAILURE);
      dup2(fd, STDIN_FILENO);
      const char** a = (const char**)malloc((args.size() + 2) * sizeof(const char*));
      if (a == NULL)
	exit(EXIT_FAILURE);
      a[0] = bin.c_str();
      for(StringVector::size_type i = 0;i < args.size();i++)
	a[i + 1] = args[i].c_str();
      a[args.size() + 1] = NULL;
      execvp(bin.c_str(), (char* const *)a);
      exit(EXIT_FAILURE);
    } //Child process;
  logMsg(LOG_DEBUG, "os:process started with pid=%d (cmd: \'%s\')", pid, cmdLine.c_str());
  int status;
  TRY_SYS_CALL(waitpid(pid, &status, 0) >= 0, "waitpid()");
  if (!WIFEXITED(status))
    {
      logMsg(LOG_ERR, "waitpid(%d) has returned status %d and WIFEXITED(%d) is false", pid, status, status);
      return EXIT_FAILURE;
    }
  const int exitCode = WEXITSTATUS(status);
  logMsg(LOG_DEBUG, "os:the process with pid=%d is terminated and exit code is %d", pid, exitCode);
  return exitCode;
}

DEEPSOLVER_END_NAMESPACE
