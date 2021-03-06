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

#define PREFIX "ds-patch:"

using namespace Deepsolver;

class IndexReconstructionListener: public AbstractIndexConstructionListener
{
public:
  IndexReconstructionListener(bool suppress)
    : m_suppress(suppress), 
    m_providesFilteringMode(0) {}

  virtual ~IndexReconstructionListener() {}

public:
  void onReferenceCollecting(const std::string& path) 
  {
    if (m_suppress)
      return;
    std::cout << "Reading references in " << path << std::endl;
  }

  void onPackageCollecting(const std::string& path) {}

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

  void onChecksumVerifying() 
  {
    if (m_suppress)
      return;
    std::cout << "Verifying checksums" << std::endl;
  }

  void onPatchingFile(const std::string& fileName)
  {
    if (m_suppress)
      return;
    if (m_providesFilteringMode)
      std::cout << "Fixing references in  " << fileName << std::endl; else
      std::cout << "Patching file " << fileName << std::endl;
  }

  void onNoTwiceAdding(const std::string& fileName)
  {
    if (m_suppress)
      return;
    std::cerr << "WARNING! File \'" << fileName << "\' already included in index, no second adding" << std::endl;
  }

  void setProvidesFilteringMode(bool value)
  {
    m_providesFilteringMode = value;
  }

private:
  bool m_suppress;
  bool m_providesFilteringMode;
}; //class IndexReconstructionListener;

class DsPatchCliParser: public CliParser
{
public:
  /**\brief The default constructor*/
  DsPatchCliParser() {}

  /**\brief The destructor*/
  virtual ~DsPatchCliParser() {}

protected:
  /**\brief Recognizes cluster of command line arguments
   *
   * This is a custom implementation recognizing "--add" and "--del" sequences.
   *
   * \param [in] params The list of all arguments potentially included into cluster
   * \param [in/out] mode The additional mode variable with user-defined purpose
   *
   * \return The number of additional (excluding first) items in provided vector making the cluster
   */
  size_t recognizeCluster(const StringVector& params, int& mode) const
  {
  assert(!params.empty());
  if (mode != 0)//We are already after "--" key;
    return 0;
  if (params[0] != "--add" && 
      params[0] != "--del" &&
params[0] != "--add-list" &&
params[0] != "--del-list")
    return CliParser::recognizeCluster(params, mode);
  size_t ending = 1;
  while (ending < params.size() &&
	 params[ending] != "--add" &&
	 params[ending] != "--del" &&
	 params[ending] != "--add-list" &&
	 params[ending] != "--del-list" &&
	 params[ending] != "--" &&
	 findKey(params[ending]) == (KeyVector::size_type)-1)
    ending++;
  ending--;
  if (ending < 1)
    throw CliParserException(CliParserException::MissedArgument, params[0]);
  return ending;
  }

  /**\brief Parses one cluster
   *
   * The This implementation of this method parses arguments according to user-defined table and takes into account "--add" and "--del" sequences.
   *
   * \param [in] cluster The arguments of one cluster to parse
   * \param [in/out] mode The additional mode variable with user-defined purpose
   */
  void parseCluster(const StringVector& cluster, int& mode)
  {
    if (mode != 0 || 
	(cluster[0] != "--add" && 
	 cluster[0] != "--del" &&
	 cluster[0] != "--add-list" && 
	 cluster[0] != "--del-list"))
      {
	CliParser::parseCluster(cluster, mode);
	return;
      }
    assert(cluster.size() > 1);
    assert(cluster[0] == "--add" || 
	   cluster[0] == "--del" ||
	   cluster[0] == "--add-list" || 
	   cluster[0] == "--del-list");
    if (cluster[0] == "--add")
      {
	for(StringVector::size_type i = 1;i < cluster.size();i++)
	  filesToAdd.push_back(cluster[i]);
	return;
      }
    if (cluster[0] == "--add-list")
      {
	for(StringVector::size_type i = 1;i < cluster.size();i++)
	  {
	    StringVector items;
	    readListFromFile(cluster[i], items);
	    for(StringVector::size_type k = 0;k < items.size();k++)
	      filesToAdd.push_back(items[k]);
	  }
	return;
      }
    if (cluster[0] == "--del")
      {
	for(StringVector::size_type i = 1;i < cluster.size();i++)
	  filesToRemove.push_back(cluster[i]);
	return;
      }
    if (cluster[0] == "--del-list")
      {
	for(StringVector::size_type i = 1;i < cluster.size();i++)
	  {
	    StringVector items;
	    readListFromFile(cluster[i], items);
	    for(StringVector::size_type k = 0;k < items.size();k++)
	      filesToRemove.push_back(items[k]);
	  }
	return;
      }
  }

private:
  void readListFromFile(const std::string& fileName, StringVector& items) const
  {
    File f;
    f.openReadOnly(fileName);
    StringVector lines;
    f.readTextFile(lines);
    f.close();
    items.clear(); 
    for(StringVector::size_type i = 0;i < lines.size();i++)
      if (!trim(lines[i]).empty())
	items.push_back(trim(lines[i]));
  }

public:
  StringVector filesToAdd, filesToRemove;
}; //class DsPatchCliParser;

static RepoParams params;
static DsPatchCliParser cliParser;

void initCliParser()
{
  cliParser.addKeyDoubleName("-p", "--provides", "Perform provides filtering  after patching");
  cliParser.addKeyDoubleName("-s", "--ref-sources", "LIST", "take additional requires/conflicts for provides filtering in listed directories (list should be colon-delimited)");
  cliParser.addKeyDoubleName("-ep", "--external-provides", "FILENAME", "read from FILENAME list of provides not to exclude from index, must be used in conjunction with  \'-r\'");
  cliParser.addKeyDoubleName("-h", "--help", "print this help screen and exit");
  cliParser.addKey("--log", "print log to console instead of user progress information");
  cliParser.addKey("--debug", "relax filtering level for log output");
  cliParser.addKeyDoubleName("-q", "--quiet", "suppress any output except of warning and error messages (cancels --log option)");
}

void printLogo()
{
  std::cout << "ds-patch: The utility to patch Deepsolver repository index" << std::endl;
    std::cout << "Version: " << PACKAGE_VERSION << std::endl;
  std::cout << std::endl;
}

void printHelp()
{
  printLogo();
  printf("%s", 
	 "Usage:\n"
	 "\tds-patch [OPTIONS] INDEX_DIR [--add FILE1 [FILE2 [...]]] [--del FILE1 [FILE2 [...]]] [--add-list LIST1 [LIST2 [...]]] [--del-list LIST1 [LIST2 [...]]]\n"
	 "Where:\n"
	 "\tINDEX_DIR       - directory with index to patch\n"
	 "\tFILE1, FILE2... - files to add or delete; files to add must be mentioned by their absolute path, file to delete - just by file names\n"
"\n"
	 "Valid command line options are:\n");
  cliParser.printHelp(std::cout);
  std::cout << std::endl;
  std::cout << "NOTE: New packages are added to index without any provides filtering, use ds-references utility for consequent provides filtering." << std::endl;
}

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

void parseCmdLine(int argc, char* argv[])
{
  try {
    cliParser.init(argc, argv);
    cliParser.parse();//FIXME:
  }
  catch (const CliParserException& e)
    {
      std::cerr << "command line error:" << e.getMessage() << std::endl;
      exit(EXIT_FAILURE);
    }
  catch(const AbstractException& e)
    {
      std::cerr << "ERROR:" << e.getMessage() << std::endl;
      exit(EXIT_FAILURE);
    }
  if (cliParser.isKeyUsed("--help"))
    {
      printHelp();
      exit(EXIT_SUCCESS);
    }
  if (cliParser.files.empty())
    cliParser.files.push_back(".");
  if (cliParser.files.size() > 1)
    {
      std::cerr << PREFIX << "Extra command line argument \'" << cliParser.files[1] << "\'" << std::endl;
      exit(EXIT_FAILURE);
    }
  params.indexPath = cliParser.files[0];
  if (cliParser.filesToAdd.empty() && cliParser.filesToRemove.empty())
    {
      std::cout << PREFIX << "Nothing to add and nothing to remove!" << std::endl;
      exit(EXIT_SUCCESS);
    }
  std::string arg;
  if (cliParser.isKeyUsed("--ref-sources", arg))
    splitColonDelimitedList(arg, params.providesRefsSources);
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
    params.readInfoFile(Directory::mixNameComponents(params.indexPath, REPO_INDEX_INFO_FILE));
    IndexReconstructionListener listener(cliParser.isKeyUsed("--log") || cliParser.isKeyUsed("--quiet"));
    IndexCore indexCore(listener);
    indexCore.rebuildIndex(params, cliParser.filesToAdd, cliParser.filesToRemove);

    if (cliParser.isKeyUsed("--provides"))
      {
	listener.setProvidesFilteringMode(1);
	std::string arg;
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
	indexCore.refilterProvides(params);
      }
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
