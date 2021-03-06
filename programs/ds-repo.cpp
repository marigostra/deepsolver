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

#include"deepsolver/deepsolver.h"
#include"deepsolver/IndexCore.h"
#include"deepsolver/CliParser.h"

#define PREFIX "ds-repo:"

using namespace Deepsolver;

static RepoParams params;
static CliParser cliParser;

class IndexConstructionListener: public AbstractIndexConstructionListener
{
public:
  IndexConstructionListener(bool suppress) 
    : m_suppress(suppress) {}

  virtual ~IndexConstructionListener() {}

public:
  void onReferenceCollecting(const std::string& path)
  {
    if (m_suppress)
      return;
    std::cout << "Collecting requires/conflicts in " << path << std::endl;
  }

  void onPackageCollecting(const std::string& path)
  {
    if (m_suppress)
      return;
    std::cout << "Reading packages in " << path << std::endl;
  }

  void onProvidesCleaning()
  {
    if (m_suppress)
      return;
    std::cout << "Performing provides filtering" << std::endl;
  }

  void onChecksumWriting()
  {
    if (m_suppress)
      return;
    std::cout << "Writing checksum file" << std::endl;
  }

  void onChecksumVerifying() {}
  void onPatchingFile(const std::string& fileName) {}
  void onNoTwiceAdding(const std::string& fileName) {}

private:
  const bool m_suppress;
}; //class IndexConstructionListener;

void splitColonDelimitedList(const std::string& str, StringVector& res)
{
  std::string s;
  for(std::string::size_type i = 0;i < str.length();i++)
    {
      if (str[i] == ':')
	{
	  if (s.empty())
	    continue;
	  res.push_back(s);
	  s.erase();
	  continue;
	}
      s += str[i];
    } //for();
  if (!s.empty())
    res.push_back(s);
}

bool splitUserParam(const std::string& str, std::string& name, std::string& value)
{
  name.erase();
  value.erase();
  bool wasEquals = 0;
  for(std::string::size_type i = 0;i < str.length();i++)
    {
      if (!wasEquals && str[i] == '=')
	{
	  wasEquals = 1;
	  continue;
	}
      if (!wasEquals)
	name += str[i]; else
	value += str[i];
    }
  return wasEquals;
}

char selectCompressionType(const std::string& value)
{
  if (value == "none")
    return RepoParams::CompressionTypeNone;
  if (value == "gzip")
    return RepoParams::CompressionTypeGzip;
  return -1;
}

char selectFormatType(const std::string& value)
{
  if (value == "binary")
    return RepoParams::FormatTypeBinary;
  if (value == "text")
    return RepoParams::FormatTypeText;
  return -1;
}

bool processUserParam(const std::string& s)
{
  std::string name, value;
  if (!splitUserParam(s, name, value))
    return 0;
  for(std::string::size_type i = 0;i < name.length();i++)
    if (s[i] == '#' || s[i] == '\\' || BLANK_CHAR(s[i]))
      return 0;
  if (name == INFO_FILE_FORMAT_TYPE ||
      name == INFO_FILE_COMPRESSION_TYPE ||
      name == INFO_FILE_VERSION ||
      name == INFO_FILE_MD5SUM ||
      name == INFO_FILE_FILTER_PROVIDES_BY_DIRS ||
      name == INFO_FILE_FILTER_PROVIDES_BY_REFS ||
      name == INFO_FILE_EXCLUDE_REQUIRES ||
      name == INFO_FILE_CHANGELOG_SOURCES ||
      name == INFO_FILE_CHANGELOG_BINARY)
    return 0;
  params.userParams.insert(StringToStringMap::value_type(name, value));
  return 1;
}

void initCliParser()
{
  cliParser.addKeyDoubleName("-c", "--compression", "TYPE", "set compression method");
  cliParser.addKeyDoubleName("-lb", "--changelog-binary", "include changelog for binary packages");
  cliParser.addKeyDoubleName("-ls", "--changelog-source", "include changelog for source packages");
  cliParser.addKeyDoubleName("-u", "--user", "NAME=VALUE[:...]", "add custom user parameters to information file");
  cliParser.addKeyDoubleName("-r", "--references", "write only provides with known corresponding requires/conflicts");
  cliParser.addKeyDoubleName("-s", "--ref-sources", "LIST", "take additional requires/conflicts for provides filtering in listed directories (list should be colon-delimited)");
  cliParser.addKeyDoubleName("-d", "--dirs", "LIST", "write only file provides  from listed directories (list should be colon-delimited)");
  cliParser.addKeyDoubleName("-ep", "--external-provides", "FILENAME", "read from FILENAME list of provides not to exclude from index, must be used in conjunction with  \'-r\'");
  cliParser.addKeyDoubleName("-nr", "--no-requires", "FILENAME", "skip requires listed by regexp in FILENAME");
  cliParser.addKeyDoubleName("-h", "--help", "print this help screen and exit");
  cliParser.addKey("--log", "print log to console instead of user progress information");
  cliParser.addKey("--debug", "relax filtering level for log output");
  cliParser.addKeyDoubleName("-q", "--quiet", "suppress any output except of warning and error messages (cancels --log option)");
}

void printLogo()
{
  std::cout << "ds-repo: The utility to create Deepsolver repository index" << std::endl;
    std::cout << "Version: " << PACKAGE_VERSION << std::endl;
  std::cout << std::endl;
}

void printHelp()
{
  printLogo();
  printf("%s", 
	 "Usage:\n"
	 "\tds-repo [OPTIONS] INDEX_DIR [PACKAGES_DIR1 [PACKAGES_DIR2 [...]]]\n"
	 "Valid command line options are:\n");
  cliParser.printHelp(std::cout);
  printf("\nIf PACKAGES_DIR1 is not specified current directory is used to search packages.\n");
}

void parseCmdLine(int argc, char* argv[])
{
  try {
    cliParser.init(argc, argv);
    cliParser.parse();
  }
  catch (const CliParserException& e)
    {
      std::cerr << "command line error:" << e.getMessage() << std::endl;
      exit(EXIT_FAILURE);
    }
  std::string arg;
  if (cliParser.isKeyUsed("--compression", arg))
    {
      params.compressionType = selectCompressionType(arg);
      if (params.compressionType == -1)
	{
	  std::cerr << PREFIX << "unknown compression type \'" << arg << "\'" << std::endl;
	  exit(EXIT_FAILURE);
	}
    }
  params.changeLogBinary = cliParser.isKeyUsed("--changelog-binary");
  params.changeLogSources = cliParser.isKeyUsed("--changelog-source");
  if (cliParser.isKeyUsed("--user", arg))
    {
      StringVector userParams;
      splitColonDelimitedList(arg, userParams);
      for(StringVector::size_type i = 0;i < userParams.size();i++)
	if (!processUserParam(userParams[i]))
	  {
	    std::cerr << PREFIX << "invalid user parameter specification \'" << arg << "\'" << std::endl;
	    exit(EXIT_FAILURE);
	  }
    }
  params.filterProvidesByRefs = cliParser.isKeyUsed("--references");
  if (cliParser.isKeyUsed("--ref-sources", arg))
    splitColonDelimitedList(arg, params.providesRefsSources);
  if (cliParser.isKeyUsed("--help"))
    {
      printHelp();
      exit(EXIT_SUCCESS);
    }
  if (cliParser.isKeyUsed("--dirs", arg))
    splitColonDelimitedList(arg, params.filterProvidesByDirs);
  if (cliParser.files.empty() || cliParser.files[0].empty())
    {
      std::cerr << PREFIX << "index directory was not mentioned" << std::endl;
      exit(EXIT_FAILURE);
    }
  params.indexPath = cliParser.files[0];
  for(StringVector::size_type i = 1;i < cliParser.files.size();i++)
    params.pkgSources.push_back(cliParser.files[i]);
  if (params.pkgSources.empty())
    params.pkgSources.push_back(".");
  if (!params.filterProvidesByRefs)
    {
      params.providesRefsSources.clear();
      params.providesRefs.clear();
    }
}

int main(int argc, char* argv[])
{
  setlocale(LC_ALL, "");
  initCliParser();
  parseCmdLine(argc, argv);
  initLogging(cliParser.isKeyUsed("--debug")?LOG_DEBUG:LOG_INFO, cliParser.isKeyUsed("--log") && !cliParser.isKeyUsed("--quiet"));
  try {
    if (!cliParser.isKeyUsed("--log") && !cliParser.isKeyUsed("--quiet"))
      printLogo();
    std::string arg;
    if (cliParser.isKeyUsed("--no-requires", arg))
      {
	File f;
	f.openReadOnly(arg);
	StringVector lines;
	f.readTextFile(lines);
	for(StringVector::size_type i = 0;i < lines.size();i++)
	  {
	    const std::string line = trim(lines[i]);
	    if (line.empty())
	      continue;
	    params.excludeRequiresRegExp.push_back(line);
	  }
      }
    if (cliParser.isKeyUsed("--external-provides", arg))
      {
	File f;
	f.openReadOnly(arg);
	StringVector lines;
	f.readTextFile(lines);
	for(StringVector::size_type i = 0;i < lines.size();i++)
	  {
	    const std::string line = trim(lines[i]);
	    if (line.empty())
	      continue;
	    params.providesRefs.push_back(line);
	  }
      }
    IndexConstructionListener listener(cliParser.isKeyUsed("--log") || cliParser.isKeyUsed("--quiet"));
    IndexCore indexCore(listener);
    indexCore.buildIndex(params);
  }
  catch(const AbstractException& e)
    {
      logMsg(LOG_CRIT, "%s error:%s", e.getType().c_str(), e.getMessage().c_str());
      if (!cliParser.isKeyUsed("--log"))
	std::cerr << "ERROR:" << e.getMessage() << std::endl;
      return EXIT_FAILURE;
    }
  catch(std::bad_alloc)
    {
      logMsg(LOG_CRIT, "No enough memory");
      if (!cliParser.isKeyUsed("--log"))
	std::cerr << "ERROR:No enough memory" << std::endl;
	  return EXIT_FAILURE;
    }




  return EXIT_SUCCESS;
}
