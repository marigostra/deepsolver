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

#ifndef DEEPSOLVER_CLI_PARSER_H
#define DEEPSOLVER_CLI_PARSER_H

namespace Deepsolver
{
  /**\brief The command line parser
   *
   * This class implements flexible approach to the command line parsing
   * procedure. General difference from usual command line parsing
   * utilities is an adding preliminary step before checking what arguments
   * a user has mentioned in command line. During this step entire
   * arguments set is divided onto several clusters. Each cluster gathers a
   * subset of arguments which should be parsed together. The second step
   * is a parsing itself, which analyzes all constructed cluster one by one.
   *
   * A user should implement recognizeCluster() and parseCluster() methods
   * to get the exact behaviour he wants. CliParser class has its own
   * implementation of these methods with default behaviour. The default
   * behaviour performs clustering and parsing based on user-defined table
   * of available command line arguments. On custom implementation the internal
   * table of available arguments is used only for help screen
   * printing, unless custom variant is not explicitly involve them.
   */
  class CliParser
  {
  public:
    /**\brief An available command line argument
     *
     * The default implementation of recognizeCluster() and parseCluster()
     * methods compares really used arguments with the content of the vector
     * filled with this class instances. This vector should contain a list of
     * all available arguments and it will get various updates, reflecting
     * what arguments user has really mentioned.
     */
    struct Key
    {
      /**\brief The default constructor*/
      Key()
	: used(0) {}

      /**\brief All possible names of an argument*/
      StringVector names;

      /**\brief Additional parameter of an argument (the name used in help screen printing, an empty value means no parameter)*/
      std::string argName;

      /**\brief The value of an additional parameter recognized during parsing procedure*/
      std::string argValue;

      /**\breif The flag was this argument mentioned by a user*/
      bool used;

      /**\brief The argument description used in help screen printing*/
      std::string descr;
    }; //struct Key;

    typedef std::list<Key> KeyList;
    typedef std::vector<Key> KeyVector;

  private:
    struct Param
    {
      Param()
	: clusterNum(0) {}

      Param(const std::string v)
	: clusterNum(0), value(v) {}

      size_t clusterNum;
      std::string value;
    }; //struct Param;

    typedef std::list<Param> ParamList;
    typedef std::vector<Param> ParamVector;

  public:
    /**\brief The default constructor*/
    CliParser() {}

    /**\brief The destructor*/
    virtual ~CliParser() {}

  public:
    /**\brief Fills internal vector of arguments mentioned by user
     *
     * \param [in] argc The argc value provided to the application main()function
     * \param [in] argv The argv value provided to the application main()function
     */
    void init(int argc, char* argv[]);

    /**\brief Runs main parsing process*/
    void parse();

    /**\brief Prints the help screen
     *
     * \param [in] s The stream to print help screen to
     */
    void printHelp(std::ostream& s) const;

    /**\brief Checks if the argument was mentioned by a user
     *
     * \param [in] keyName The name of the argument to check
     * \param [out] arg The reference to the variable to receive additional parameter value
     *
     * \return Non-zero if the command line argument was mentioned by a user
     */
    bool isKeyUsed(const std::string& keyName, std::string& arg) const;

    /**\brief Checks if the argument was mentioned by a user
     *
     * \param [in] keyName The name of an argument to check
     *
     * \return Non-zero if the argument was mentioned by a user
     */
    bool isKeyUsed(const std::string& keyName) const;

    /**\brief Adds new argument to the table of the available arguments
     *
     * \param [in] name The name of the argument to add
     * \param [in] descr The argument description for a help screen
     */
    void addKey(const std::string& name, const std::string& descr);

    /**\brief Adds new argument to the table of the available arguments
     *
     * \param [in] name The name of the argument to add
     * \param [in] argName The name of additional parameter (used in a help screen)
     * \param [in] descr The argument description for a help screen
     */
    void addKey(const std::string& name, const std::string& argName, const std::string& descr);

    /**\brief Adds new argument to the table of available arguments
     *
     * \param [in] name1 The name of the argument to add
     * \param [in] name2 The synonym of the argument to add
     * \param [in] descr The argument description for a help screen
     */
    void addKeyDoubleName(const std::string& name1, const std::string& name2, const std::string& descr);

    /**\brief Adds new argument to the table of available arguments
     *
     * \param [in] name1 The name of the argument to add
     * \param [in] name2 The synonym of the argument to add
     * \param [in] argName The name of additional parameter (used in a help screen)
     * \param [in] descr The argument description for a help screen
     */
    void addKeyDoubleName(const std::string& name1, const std::string& name2, const std::string& argName, const std::string& descr);

  protected:
    /**\brief Recognizes cluster of command line arguments
     *
     * The descent class may provide custom implementation of this method.
     *
     * \param [in] params The list of all arguments potentially included into a cluster
     * \param [in/out] mode The additional mode variable with user-defined purpose
     *
     * \return The number of additional (excluding first) items in provided vector making the cluster
     */
    virtual size_t recognizeCluster(const StringVector& params, int& mode) const;

    /**\brief Parses one cluster
     *
     * \param [in] cluster The arguments of one cluster to parse
     * \param [in/out] mode The additional mode variable with user-defined purpose
     */
    virtual void parseCluster(const StringVector& cluster, int& mode);

  protected:
    /**\brief Searches the argument in the table of available arguments
     *
     * \param [in] name Name of the argument to search
     *
     * \return The index in the table of available arguments or (KeyVector::size_type)-1 if there is no such argument
     */
    KeyVector::size_type findKey(const std::string& name) const;

    /** Checks if specified argument has an additional parameter
     *
     * \param name The name of the argument to check
     *
     * \return Non-zero if the specified argument has an additional parameter
     */
    bool hasKeyArgument(const std::string& name) const;

  protected:
    /**\brief Indicates that the provided argc value equals zero
     *
     *This method must throw an exception or call the exit() function to interrupt parsing procedure.
     */
    virtual void stopNoPrgName() const;

    /**\brief Indicates the argument was mentioned without a required parameter
     *
     *This method must throw an exception or call the exit() function to interrupt parsing procedure.
     *
     * \param [in] keyname The name of the argument mentioned without a parameter.
     */
    virtual void stopMissedArgument(const std::string& keyName) const;

  private:
    std::string m_prgName;
    ParamVector m_params;

  public:
    /**\brief The table of all available arguments*/
    KeyVector keys;

    /**\brief The arguments not present in available arguments table or mentioned after "--" sequence*/
    StringVector files;
  }; //class CliParser;
} //namespace Deepsolver;

#endif //DEEPSOLVER_CLI_PARSER_H;
