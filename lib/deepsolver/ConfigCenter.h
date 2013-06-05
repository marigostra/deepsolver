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

#ifndef DEEPSOLVER_CONFIG_CENTER_H
#define DEEPSOLVER_CONFIG_CENTER_H

#include"deepsolver/ConfigAdapter.h"
#include"deepsolver/ConfigData.h"

namespace Deepsolver
{
  /**\brief The central configuration processing class
   *
   * This class designed as central place to store all configuration
   * information necessary for all operations except repository index
   * building. Every client application should create the instance of this
   * class, process with it configuration files it need and provide the
   * reference to OperationCore or InfoCore classes depending on required
   * task.
   *
   * Configuration center class strongly associated with ConfigFile class
   * since it is used for configuration file syntax processing. During
   * configuration file reading two types of exceptions can be thrown:
   * ConfigFileException and ConfigException. The first type is used for
   * syntax error indication only, the second one is designed for general
   * configuration data validation problems.
   *
   * Do not forget to call commit() method after configuration files
   * reading. Some data may be left unprepared without commit() method
   * invocation. After configuration files processing configuration data is
   * placed in various structures gathered in ConfRoot type and can be
   * accessed directly.
   *
   * \sa ConfRoot ConfigFile ConfigException ConfigFileException
   */
  class ConfigCenter
    : public ConfigAdapter,
      private AbstractConfigFileHandler
  {
  public:
    /**\brief The default constructor*/
    ConfigCenter() 
    {
      initValues();
    }

    /**\brief The destructor*/
    virtual ~ConfigCenter() {}

  public:
    /**\brief reads single configuration file
     *
     * This method reads one configuration file parses it and saves processed
     * values into internal structures for further access. You can call it
     * multiple times for different files but do not forget make final
     * invocation of commit() method to validate received data.
     *
     * This method throws two types of exceptions: ConfigFileException to
     * indicate syntax problems and ConfigException for general errors.
     *
     * \param [in] fileName Name of the file to read configuration data from
     *
     * \sa ConfigException ConfigFileException
     */
    void loadFromFile(const std::string& fileName);
    void loadFromDir(const std::string& path);

    /**\brief Verifies read configuration data
     *
     * This method makes final configuration data preparing and performs
     * various checks to be sure the data is valid. In case of errors
     * ConfigException or ConfigFileException can be thrown. This method call
     * is strongly required for proper further execution.
     *
     * \sa ConfigException ConfigFileException
     */
    void commit();

    void printConfigData(std::ostream& s) const;

    /**\brief Returns the reference to parsed configuration data
     *
     * This method used for access to parsed values after configuration data reading during operations processing.
     *
     * \return The reference to parsed data
     *
     * \sa ConfRoot
     */
    const ConfRoot& root() const
    {
      return m_root;
    }

  private:
  private:
    void initValues();
    void initRepoValues();
    void initProvideValues();

    void addStringParam3(const std::string& path1,
			 const std::string& path2,
			 const std::string& path3,
			 std::string& value);

    void addNonEmptyStringParam3(const std::string& path1,
				 const std::string& path2,
				 const std::string& path3,
				 std::string& value);

    void addStringListParam3(const std::string& path1,
			     const std::string& path2,
			     const std::string& path3,
			     StringVector& value);

    void addNonEmptyStringListParam3(const std::string& path1,
				     const std::string& path2,
				     const std::string& path3,
				     StringVector& value);

  private://AbstractConfigFileHandler;
    void onConfigFileValue(const StringVector& path, 
			   const std::string& sectArg,
			   const std::string& value,
			   bool adding,
			   const ConfigFilePosInfo& pos);

  private:
    ConfRoot m_root;
  }; //class ConfigCenter;
} //namespace Deepsolver;

#endif //DEEPSOLVER_CONFIG_CENTER_H;
