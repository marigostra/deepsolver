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

#ifndef DEEPSOLVER_EXCEPTIONS_H
#define DEEPSOLVER_EXCEPTIONS_H

#define TRY_SYS_CALL(expr, msg) if (!(expr)) throw SystemException(msg)
#define SYS_STOP(msg) throw SystemException(msg)

namespace Deepsolver
{
  class AbstractException;
  class SystemException;
  class TaskException;
  class OperationCoreException;
  class IndexCoreException;
  class ConfigFileException;
  class ConfigException;
  class InfoFileSyntaxException;
  class InfoFileValueException;
  class GzipException;
  class CurlException;
  class RegExpException;
  class Md5FileException;
  class PkgBackEndException;
  class NotImplementedException;

  /**\brief The class implementing the Visitor design pattern for project exceptions
   *
   * This class helps to create particular handler for each type of project
   * exceptions. For example, it can be used for generating messages,
   * describing occurred errors. ExceptionVisitor, according to its name,
   * implements widely-known design pattern "visitor". Use accept() method
   * of AbstractException class to invoke appropriate visit() variant. It
   * is not necessary to implement all visit() methods, since there are
   * empty bodies for them. You may safely override only their subset.
   *
   * \sa AbstractException
   */
  class ExceptionVisitor
  {
  public:
    /**\brief The default constructor*/
    ExceptionVisitor() {}

    /**\brief The destructor*/
    virtual ~ExceptionVisitor() {}

  public:
    /**\brief The action for system call errors
     *
     * \param [in] e The reference to the exception object
     */
    virtual void visit(const SystemException& e) {}

    /**\brief The action for user task processing errors
     *
     * \param [in] e The reference to the exception object
     */
    virtual void visit(const TaskException& e) {}

    /**\brief The action for operation core errors
     *
     * \param [in] e The reference to the exception object
     */
    virtual void visit(const OperationCoreException& e) {}

    /**\brief The action for index core errors
     *
     * \param [in] e The reference to the exception object
     */
    virtual void visit(const IndexCoreException& e) {}

    /**\brief The action for configuration file syntax errors
     *
     * \param [in] e The reference to the exception object
     */
    virtual void visit(const ConfigFileException& e) {}

    /**\brief The action for configuration errors
     *
     * \param [in] e The reference to the exception object
     */
    virtual void visit(const ConfigException& e) {}

    /**\brief The action for the repository infor file syntax errors
     *
     * \param [in] e The reference to the exception object
     */
    virtual void visit(const InfoFileSyntaxException& e) {}

    /**\brief The action for the repository info file value errors
     *
     * \param [in] e The reference to the exception object
     */
    virtual void visit(const InfoFileValueException& e) {}

    /**\brief The action for GZip compression/decompression errors
     *
     * \param [in] e The reference to the exception object
     */
    virtual void visit(const GzipException& e) {}

    /**\brief The action for downloading errors
     *
     * \param [in] e The reference to the exception object
     */
    virtual void visit(const CurlException& e) {}

    /**\brief The action for regular expression processing errors
     *
     * \param [in] e The reference to the exception object
     */
    virtual void visit(const RegExpException& e) {}

    /**\brief The action for MD5-file syntax errors
     *
     * \param [in] e The reference to the exception object
     */
    virtual void visit(const Md5FileException& e) {}

    /**\brief The action for package back-end errors
     *
     * \param [in] e The reference to the exception object
     */
    virtual void visit(const PkgBackEndException& e) {}

    /**\brief The action for invocation of non-implemented features
     *
     * \param [in] e The reference to the exception object
     */
    virtual void visit(const NotImplementedException& e) {}
  }; //class ExceptionVisitor;

  /**\brief The main exception class of the project
   *
   * Every exception class, used for error indication in Deepsolver
   * project, must be a descendant (not exactly direct) of this
   * AbstractException class. It is created to simplify and unify error handling.
   * The main information provided by this class is the
   * an error type designation and a single line description.
   *
   * \sa ExceptionVisitor
   */
  class AbstractException
  {
  public:
    /**\brief The default constructor*/
    AbstractException() {}

    /**\brief the destructor*/
    virtual ~AbstractException() {}

  public:
    /**\brief Returns a single line error description
     *
     * This method returns a single line string value with error
     * description. Usually it is the value printed to user in the error
     * message. The value should not include error type, since it can be obtained
     * through getType() method.
     *
     * \return A single line error description
     */
    virtual std::string getMessage() const = 0;

    /**\brief Returns a string with short error type designation
     *
     * This method returns a short string with one or two words describing
     * the error type. For example, this method can return values like
     * "system", "back-end" etc. A value returned by this method usually is used
     * for error message construction.
     *
     * \return A short string with error type designation
     */
    virtual std::string getType() const = 0;

    /**\brief Calls appropriate visitor method
     *
     * This method performs invocation of corresponding visit() method of the
     * visitor, provided by the reference. You should use it each time, when
     * you want to make particular handling action for various exception
     * types, as it suggested by design pattern "visitor". 
     * 
     * \param [in] visitor The reference  to the visitor
     */
    virtual void accept(ExceptionVisitor& visitor) const = 0;
  }; //class AbstractException;

  /**\brief The exception for system call errors
   *
   * This class is used for indication of errors caused by various system
   * calls problems. It automatically analyzes the value of system errno
   * variable and can construct informative error description with text,
   * provided by operating system. An error message
   * consists of two parts: the short string provided by developer with any
   * information he wants and the string given by a operating system. 
   * Developer can save in this string, for example, a name of failed system call
   * with its arguments.
   */
  class SystemException: public AbstractException
  {
  public:
    /**\brief The default constructor
     *
     * This constructor implies automatic errno analyzing, but without
     * developer-given string, so the getMessage() method will return only
     * a single line description, provided by operating system.
     */
    SystemException()
      : m_code(errno) {}

    /**\brief The constructor with developer comment
     *
     * This constructor allows developer to give short comment of the error
     * with automatic adding  a string from operating system, got through errno
     * variable.
     *
     * \param [in] comment A developer error comment
     */
    SystemException(const std::string& comment)
      : m_code(errno), m_comment(comment) {}

    /**\brief The constructor with error code specification
     *
     * Using this constructor developer can provide any error code he wants 
     * without any additional comments. The operating system will be requested
     * for error description using provided value . The developer must
     * give the value, normally obtained through errno variable.
     *
     * \param [in] code An error code
     */
    SystemException(int code)
      : m_code(code) {}

    /**\brief The constructor with error code and comment specification
     *
     * With this constructor developer can provide an error code to request
     * description from operating system and any short comment he
     * wants. The value of error code must be the value usually taken through
     * errno variable.
     *
     * \param [in] code An error code
     * \param [in] comment A developer error additional information
     */
    SystemException(int code, const std::string& comment)
      : m_code(code), 
	m_comment(comment) {}

    /**\brief The destructor*/
    virtual ~SystemException() {}

  public:
    /**\brief Returns an error code
     * \return An erno value of a corresponding error
     */
    int getCode() const
    {
      return m_code;
    }

    /**\brief Returns the OS-generated error description associated with set errno
     * \return A error description from the operating system
     */
    std::string getDescr() const
    {
      return strerror(m_code);
    }

    /**\brief Returns developer error comment
     * \return Developer error description
     */
    std::string getComment() const
    {
      return m_comment;
    }

  public://AbstractException;
    std::string getMessage() const;

    std::string getType() const
    {
      return "system";
    }

    void accept(ExceptionVisitor& visitor) const
    {
      visitor.visit(*this);
    }

  private:
    const int m_code;
    const std::string m_comment;
  }; //class SystemException;

  /**\brief The exception class for notifications the user task is not solvable
   *
   * This exception notifies there is a problem occurred during user task
   * solving. The main its distinction is that problem can be caused only by
   * impossible user request. That means there is no any other implied
   * reasons, neither in package database nor in solver environment.
   *
   * This exception can be one of the several types designating various
   * invalid situations: an unknown package to install, the same package is
   * considered to install and to remove simultaneously, no packages to
   * satisfy a Require entry etc. An exception instance takes one string
   * parameter with additional information, which purpose depends on
   * exception type. For example, if the problem is an unknown package to
   * install the parameter contains its name and so on.
   *
   * \sa OperationCoreException
   */
  class TaskException: public AbstractException
  {
  public:
    enum {
      UnknownPkg, //the parameter contains name of the unknown package;
      Contradiction, //the parameter contains full package designation which is considered to install and remove simultaneously;
      UnsolvableSat, //SAT equation has no solution, parameter is empty;
      Unmet, //the parameter contains the unsatisfied require entry;
CodeCount
    };

  public:
    /**\brief The constructor with error type and string parameter
     *
     * \param [in] code An error code
     * \param [in] param An additional string parameter
     */
    TaskException(int code, const std::string& param)
      : m_code(code), 
	m_param(param) {}

    /**\brief The constructor with error code only
     *
     * \param [in] code The error code
     */
    TaskException(int code)
      : m_code(code) {}

    /**\brief The destructor*/
    virtual ~TaskException() {}

  public:
    /**\brief Returns the error code
     * \return The code of the error
     */
    int getCode() const
    {
      return m_code;
    }

    /**\brief Returns an optional string parameter of the error
     * \return The optional string parameter
     */
    const std::string& getParam() const
    {
      return m_param;
    }

  public://AbstractException;
    std::string getMessage() const;

    std::string getType() const
    {
      return "task";
    }

    void accept(ExceptionVisitor& visitor) const
    {
      visitor.visit(*this);
    }

  private:
    const int m_code;
    const std::string m_param;
  }; //class TaskException;

  /**\brief exception for general operation problems
   *
   * This class is purposed for various general operation problems.
   * These errors can be thrown only by the methods of
   * OperationCore class, covering transaction processing as well as index
   * updating. General error types are a checksum
   * mismatch, invalid content of repo file and so on. Downloading problems
   * have their own exception class name of CurlException.
   */
  class OperationCoreException: public AbstractException
  {
  public:
    enum {
      InvalidInfoFile, //with file name as a param;
      InvalidChecksumData, //with checksum file name (e.g. md5sum.txt) as a param;
      BrokenIndexFile, //with file name as a param;
      LimitExceeded, //limited resource designation as a param;
      InvalidInstalledPkg, //name of the package as a param;
      InvalidRepoPkg, //name of the package as a param;
      CodeCount
    };

  public:
    /**\brief The constructor with error code specification
     *
     * \param [in] code The error code
     */
    OperationCoreException(int code) 
      : m_code(code) {}

    /**\brief The constructor with error code and optional parameter specification
     *
     * \param [in] code The error code
     * \param [in] param The optional string parameter
     */
    OperationCoreException(int code, const std::string& param) 
      : m_code(code), 
	m_param(param) {}

    /**\brief The destructor*/
    virtual ~OperationCoreException() {}

  public:
    /**\brief Returns the error code
     * \return The error code
     */
    int getCode() const
    {
      return m_code;
    }

    /**\brief Returns the optional string parameter of the error occurred
     * \return The optional string parameter of the error
     */
    std::string getParam() const
    {
      return m_param;
    }

  public://AbstractException;
    std::string getMessage() const;

    std::string getType() const
    {
      return "operation";
    }

    void accept(ExceptionVisitor& visitor) const
    {
      visitor.visit(*this);
    }

  private:
    const int m_code;
    const std::string m_param;
  }; //class OperationCoreException;

  /**\brief Indicates repository index manipulation problem
   *
   * The instance of this exception is thrown on some index creation or
   * modifications problems such as corrupted file, not empty target directory etc.
   * This exception created as an  addition to general exception classes,
   * like SystemException, PkgBackEndException and so on, which can be also thrown
   * during index operations.
   */
  class IndexCoreException: public AbstractException 
  {
  public:
    enum {
      DirectoryNotEmpty,
      CorruptedFile,
      MissedChecksumFileName,
      CodeCount
    };

  public:
    /**\brief The constructor with error code specification
     *
     * \param [in] code The error code
     */
    IndexCoreException(int code)
      : m_code(code) {}

    /**\brief The constructor with error code and string parameter specification
     *
     * \param [in] code The error code
     * \param [in] arg The string parameter of the problem
     */
    IndexCoreException(int code, const std::string& param)
      : m_code(code),
	m_param(param) {}

    /**\brief The destructor*/
    virtual ~IndexCoreException() {}

  public:
    /**\brief Returns the error code
     * \return The error code
     */
    int getCode() const
    {
      return m_code;
    }

    /**\brief Returns the optional string parameter
     * \return The additional string parameter
     */
    const std::string& getParam() const
    {
      return m_param;
    }

  public://AbstractException;
    std::string getMessage() const;

    std::string getType() const
    {
      return "index";
    }

    void accept(ExceptionVisitor& visitor) const
    {
      visitor.visit(*this);
    }

  private:
    const int m_code;
    const std::string m_param;
  }; //class IndexCoreException;

  /**\brief The exception class for config file syntax errors
   *
   * This class instance is thrown when something is wrong with
   * configuration file syntax. Through this class various information
   * about the problem can be obtained, like file name, line number,
   * character position, line content and error code. Be careful, character
   * position is given in unibyte string representation, hence, if UTF-8
   * sequences are present additional processing is needed to find real
   * character number. This class does not provide any text descriptions. 
   *
   * \sa ConfigException
   */
  class ConfigFileException: public AbstractException
  {
  public:
    enum {
      SectionInvalidType,
      SectionWaitingOpenBracket,
      SectionWaitingName,
      SectionInvalidNameChar,
      SectionWaitingCloseBracketOrArg,
      SectionUnexpectedArgEnd,
      SectionWaitingCloseBracket,
      ValueWaitingName,
      ValueInvalidNameChar,
      ValueWaitingAssignOrNewName,
      ValueWaitingNewName,
      ValueUnexpectedValueEnd,
      CodeCount
    };

  private:
    struct Descr
    {
      int code;
      const char* message;
    }; //struct Descr;

  private:
    static const Descr m_descr[];

  public:
    /**\brief The constructor
     *
     * \param [in] code The error code
     * \param [in] fileName Name of a file being processed
     * \param [in] lineNumber The number of the line with encountered error
     * \param [in] pos The problem character position (unibyte coding)
     * \param [in] line The content of the invalid line
     */
    ConfigFileException(int code,
			const std::string& fileName,
			size_t lineNumber,
			std::string::size_type pos,
			const std::string& line)
      : m_code(code),
	m_fileName(fileName),
	m_lineNumber(lineNumber),
	m_pos(pos),
	m_line(line) {}

    /**\brief The destructor*/
    virtual ~ConfigFileException() {}

  public:
    /**\brief Returns the error code
     * \return The error code
     */
    int getCode() const
    {
      return m_code;
    }

    /**\brief Returns the name of a file with invalid line
     * \return Name of a file with an invalid line
     */
    const std::string& getFileName() const
    {
      return m_fileName;
    }

    /**\brief Returns a number of the invalid line
     * \return Number of the invalid line
     */
    size_t getLineNumber() const
    {
      return m_lineNumber;
    }

    /**\brief Returns the position of the invalid character
     * \return The number of the invalid character
     */
    std::string::size_type getPos() const
    {
      return m_pos;
    }

    /**\brief Returns the text of the invalid line
     * \return The content of the invalid line
     */
    const std::string& getLine() const
    {
      return m_line;
    }

  public:
    /**\brief Generates English  problem description by the given code
     *
     * \param [in] code The error code to generate description for
     *
     * \return The generated English error description
     */
    static std::string getDescr(int code)
    {
      size_t i = 0;
      while (m_descr[i].code != code && m_descr[i].code >= 0 && m_descr[i].message != NULL)
	++i;
      assert(m_descr[i].code >= 0 && m_descr[i].message != NULL);
      return m_descr[i].message;
  }

  public://AbstractException;
    std::string getMessage() const;

    std::string getType() const
    {
      return "config syntax";
    }

    void accept(ExceptionVisitor& visitor) const
    {
      visitor.visit(*this);
    }

  private:
    const int m_code;
    const std::string m_fileName;
    const size_t m_lineNumber;
    const std::string::size_type m_pos;
    const std::string m_line;
  }; //class ConfigFileException;

  /**\brief Indicates an error in configuration data
   *
   * This class instance indicates any problem in Deepsolver configuration
   * structures. Be careful, it must be not confused with
   * ConfigFileException, used to notify about configuration file syntax
   * errors.
   *
   * The objects of ConfigException provide the error code, option path with section argument if needed
   * and error location in configuration file if there is any. 
   * Note, some sort of problems are still meaningful even without any reference to location 
   * in a configuration file.
   *
   * \sa ConfigFileException
   */
  class ConfigException: public AbstractException
  {
  public:
    enum {
      UnknownOption,
      ValueCannotBeEmpty,
      AddingNotPermitted,
      InvalidBooleanValue,
      InvalidIntValue,
      InvalidUIntValue,
      InvalidUrl,
CodeCount
    };

  private:
    struct Descr
    {
      int code;
      const char* message;
    }; //struct Descr;

  private:
    static const Descr m_descr[];

  public:
    /**\brief The constructor
     *
     * \param [in] code The error code
     * \param [in] path A value path
     * \param [in] sectArg An  argument for first-level section
     * \param [in] line An invalid line value
     * \param [in] fileName The name of the config file with the invalid line
     * \param [in] lineNumber A number of the invalid line
     */
    ConfigException(int code,
		    const StringVector& path,
		    const std::string& sectArg,
		    const std::string& line,
		    const std::string& fileName,
		    size_t lineNumber)
      : m_code(code),
	m_path(path),
	m_sectArg(sectArg),
	m_line(line),
	m_fileName(fileName),
	m_lineNumber(lineNumber) {}

    /**\brief The destructor*/
    virtual ~ConfigException() {}

  public:
    /**\brief Returns the error code
     * \return The error code
     */
    int getCode() const
    {
      return m_code;
    }

    /**\brief Returns a configuration option path
     * \return A configuration option path
     */
    const StringVector& getPath() const
    {
      return m_path;
    }

    /**\brief Returns a first-level section argument
     * \return A first-level section argument if there is any
     */
    std::string getSectArg() const
    {
      return m_sectArg;
    }

    /**\brief Returns an invalid line value
     * \return An invalid line value
     */
    std::string getLine() const
    {
      return m_line;
    }

    /**\brief Returns a file name with an invalid line
     * \return A file name with the invalid line
     */
    std::string getFileName() const
    {
      return m_fileName;
    }

    /**\brief Returns number of a invalid line
     *
     * Use this method to get 1-based number of an invalid line. If this method 
     * returns 0 that means the error is not associated
     * with any location in configuration files.
     *
     * \return A number of the line caused the problem or zero if there is no any
     */
    size_t getLineNumber() const
    {
      return m_lineNumber;
    }

    /**\brief Returns a string designation of a configuration option with an invalid value
     * \return A unique configuration option designation
     */
    std::string getOptionDesignation() const
    {
      if (m_path.empty())
	return "";
      std::string res = m_path[0];
      if (!m_sectArg.empty())
	res += " \"" + m_sectArg + "\"";
      for(StringVector::size_type i = 1;i < m_path.size();i++)
	res += "." + m_path[i];
      return res;
    }

    /**\brief Returns full designation of configuration problem location
     *
     * This method constructs string with unique designation of a place causing 
     * configuration problem. This information usually includes file name and line number.
     *
     * \return A string designation of a configuration problem location
     */
    std::string getLocationDesignation() const
    {
      if (m_lineNumber == 0 || m_fileName.empty())
	return "";
      std::ostringstream ss;
      ss << m_fileName << ":" << m_lineNumber << ":";
      return ss.str();
    }

  public:
    /**\brief Generates English  problem description by the given code
     *
     * \param [in] code The error code to generate description for
     *
     * \return The generated English error description
     */
    static std::string getDescr(int code)
    {
      size_t i = 0;
      while (m_descr[i].code != code && m_descr[i].code >= 0 && m_descr[i].message != NULL)
	++i;
      assert(m_descr[i].code >= 0 && m_descr[i].message != NULL);
      return m_descr[i].message;
  }

  public://AbstractConfiguration;
    std::string getMessage() const;

    std::string getType() const
    {
      return "configuration";
    }

    void accept(ExceptionVisitor& visitor) const
    {
      visitor.visit(*this);
    }

  private:
    const int m_code;
    const StringVector m_path;
    const std::string m_sectArg;
    const std::string m_line;
    const std::string m_fileName;
    const size_t m_lineNumber;
  }; //class ConfigException;

  /**\brief The info file syntax error
   *
   * This exception signales about an invalid syntax of a repository info
   * file. Info files usually come as basic header of remote package
   * repositories.
   *
   * \sa InfoFileValueException
   */
  class InfoFileSyntaxException: public AbstractException
  {
  public:
    enum {
      UnexpectedCharacter,
      IncompleteLine,
CodeCount
    };

  public:
    /**\brief The constructor
     *
     * \param [in] code The error code
     * \param [in] lineNumber The number of the invalid line
     * \param [in] line The invalid line content
     */
    InfoFileSyntaxException(int code,
			    size_t lineNumber,
			    const std::string& line)
      : m_code(code),
      m_lineNumber(lineNumber),
      m_line(line) {}

    /**\brief The destructor*/
    virtual ~InfoFileSyntaxException() {}

  public:
    /**\brief Returns the error code
     * \return The error code
     */
    int getCode() const
    {
      return m_code;
    }

    /**\brief Returns the number of the invalid line
     * \return The number of the invalid line
     */
    size_t getLineNumber() const
    {
      return m_lineNumber;
    }

    /**\brief Returns the wrong line content
     * \return The wrong line content
     */
    std::string getLine() const
    {
      return m_line;
    }

  public://AbstractException;
    std::string getMessage() const;

    std::string getType() const
    {
      return "info file syntax";
    }

    void accept(ExceptionVisitor& visitor) const
    {
      visitor.visit(*this);
    }

  private:
    const int m_code;
    const size_t m_lineNumber;
    const std::string m_line;
  }; //class InfoFileSyntaxException;

  /**\brief The exception class for invalid info file value
   *
   * This exception class indicates that info file, got as a repository
   * header, contains an incorrect value. This error should not be confused
   * with other types of info file problems,
   * e.g. InfoFileSyntaxException. An incorrect value means cases when
   * obtained string does not belong to the list of proper values forsome
   * particular parameter. Such problem can be faced with boolean
   * parameters or with the parameters implying fixed set of values, like
   * compression type ("none", "gzip" etc).
   *
   * \sa InfoFileSyntaxException
   */
  class InfoFileValueException: public AbstractException
  {
  public:
    enum {
      InvalidFormatType,
      InvalidCompressionType,
      InvalidBooleanValue
    };

  public:
    /**\brief The constructor
     *
     * \param [in] code The error code
     * \param [in] param The string error parameter
     */
    InfoFileValueException(int code, const std::string& param)
      : m_code(code),
	m_param(param) {}

    /**\brief The destructor*/
    virtual ~InfoFileValueException() {}

  public:
    /**\brief Returns the error code
     * \return The error code
     */
    int getCode() const
    {
      return m_code;
    }

    /**\brief Returns the error parameter 
     * \return The error parameter
     */
    const std::string& getParam() const
    {
      return m_param;
    } 

  public://AbstractException;
    std::string getMessage() const;

    std::string getType() const
    {
      return "info file value";
    }

    void accept(ExceptionVisitor& visitor) const
    {
      visitor.visit(*this);
    }

  private:
    const int m_code;
    const std::string m_param;
  }; //class InfoFileValueException;

  /**\brief The exception for errors of GZip handling code
   *
   * This class brings information about the errors raised in GZip
   * wrapper. It allows to get only single-line description and does not
   * provide any additional information about error structure.  
   */
  class GzipException: public AbstractException
  {
  public:
    /**\brief The constructor
     *
     * \param [in] message An error description
     */
    GzipException(const std::string& msg)
      : m_msg(msg) {}

    /**\brief The destructor*/
    virtual ~GzipException() {}

  public://AbstractException;
    std::string getMessage() const;

    std::string getType() const
    {
      return "gzip";
    }

    void accept(ExceptionVisitor& visitor) const
    {
      visitor.visit(*this);
    }

  private:
    const std::string m_msg;
  }; //class GzipException;

  /**\brief The exception class for libcurl errors
   *
   * This class is used for notifications about the errors occurred during
   * files fetching. It instances contain error code, URL of the resource,
   * caused the error, and short single-line description. For download
   * operations libcurl is used and, hence, this exception is basically
   * purposed to be a wrapper for libcurl failures.
   */
  class CurlException: public AbstractException
  {
  public:
    /**\brief The constructor
     *
     * \param [in] code The error code
     * \param [in] url A The URL caused a problem
     * \param [in] descr The error description
     */
    CurlException(int code,
		  const std::string& url,
		  const std::string& descr)
      : m_code(code), 
	m_url(url),
	m_descr(descr) {}

    /**\brief The destructor*/
  virtual ~CurlException() {}

  public:
    /**\brief Returns the error code
     * \return The error code
     */
    int getCode() const
    {
      return m_code;
    }

    /**\brief Returns the URL caused the error
     * \return The URL caused the error
     */
    std::string getUrl() const
    {
      return m_url;
    }

    /**\brief Returns the error description
     * \return The error description
     */
    std::string getDescr() const
    {
      return m_descr;
    }

  public://AbstractException;
    std::string getMessage() const;

    std::string getType() const
    {
      return "curl";
    }

    void accept(ExceptionVisitor& visitor) const
    {
      visitor.visit(*this);
    }

  private:
    const int m_code;
    const std::string m_url;
    const std::string m_descr;
  }; //class CurlException;

  /**\brief Notifies about the errors occurred during regular  expressions processing
   *
   * This class brings an information about errors caused by regular
   * expressions processing. The only value, provided by this exception, is
   * single-line message with short description what's wrong.
   */
  class RegExpException: public AbstractException
  {
  public:
    /**\brief The constructor
     *
     * \param [in] message The error message
     */
    RegExpException(const std::string& message)
      : m_message(message) {}

    /**\brief The destructor*/
    virtual ~RegExpException() {}

  public://AbstractException;
    std::string getMessage() const;

    std::string getType() const
    {
      return "regular expression";
    }

    void accept(ExceptionVisitor& visitor) const
    {
      visitor.visit(*this);
    }

  private:
    const std::string m_message;
  }; //class RegExpException;

  /**\brief The exception class for md5file syntax errors
   *
   * This class instance is thrown each time when md5file syntax error is
   * encountered. The client application can access various information
   * through it about the problem, like file name, line number,
   * line content and error code.
   */
  class Md5FileException: public AbstractException
  {
  public:
    enum {
      TooShortLine,
      InvalidChecksumFormat
    };

  public:
    /**\brief The constructor
     *
     * \param [in] code The error code
     * \param [in] fileName Name of a file being processed
     * \param [in] lineNumber The number of a line with encountered error
     * \param [in] line The text of an invalid line
     */
    Md5FileException(int code,
		     const std::string& fileName,
		     size_t lineNumber,
		     const std::string& line)
      : m_code(code),
	m_fileName(fileName),
	m_lineNumber(lineNumber),
	m_line(line) {}

    /**\brief The destructor*/
    virtual ~Md5FileException() {}

  public:
    /**\brief Returns the error code
     * \return The error code
     */
    int getCode() const
    {
      return m_code;
    }

    /**\brief Returns the name of a file with invalid line
     * \return Name of a file with an invalid line
     */
    const std::string& getFileName() const
    {
      return m_fileName;
    }

    /**\brief Returns number of an invalid line
     * \return The number of an invalid line
     */
    size_t getLineNumber() const
    {
      return m_lineNumber;
    }

    /**\brief Returns the text of the invalid line
     * \return The content of the invalid line
     */
    const std::string& getLine() const
    {
      return m_line;
    }

  public://AbstractException; 
    std::string getMessage() const;

    std::string getType() const
    {
      return "md 5file";
    }

    void accept(ExceptionVisitor& visitor) const
    {
      visitor.visit(*this);
    }

  private:
    const int m_code;
    const std::string m_fileName;
    const size_t m_lineNumber;
    const std::string m_line;
  }; //class Md5FileException;

  /**\brief Indicates an error inside of package back-end
   *
   * This class represents an exception in package back-end layer. There
   * can be several package back-end implementations (rpm, dpkg etc), but
   * all of them should use this type of exception to unify errors
   * handling in command line tools and other libdeepsolver clients. The
   * preferable parameter for this class instance is a failed function name.
   */
  class PkgBackEndException: public AbstractException
  {
  public:
    /**\brief The constructor
     *
     * \param [in] fnName A name of a failed function
     */
    PkgBackEndException(const std::string& fnName)
      : m_fnName(fnName) {}

    /**\brief The destructor*/
    virtual ~PkgBackEndException() {}

  public:
    /**\brief Returns the name of the function caused the error
     * \return The name of the function caused the error
     */
    const std::string& getFnName() const
    {
      return m_fnName;
    }

  public://AbstractException;
    std::string getMessage() const;

    std::string getType() const
    {
      return "back-end";
    }

    void accept(ExceptionVisitor& visitor) const
    {
      visitor.visit(*this);
    }

  private:
    const std::string m_fnName;
  }; //class PkgBackEndException;

  class NotImplementedException: public AbstractException
  {
  public:
    /**\brief The constructor
     *
     * \param [in] message A description of non-implemented feature
     */
    NotImplementedException(const std::string& message) 
      : m_message(message) {}

    /**\brief The destructor*/
    virtual ~NotImplementedException() {}

  public:
    std::string getMessage() const
    {
      return m_message;
    }

    std::string getType() const
    {
      return "not implemented";
    }


    void accept(ExceptionVisitor& visitor) const
    {
      visitor.visit(*this);
    }


  private:
    const std::string m_message;
  }; //class NotImplementedException;
} //namespace Deepsolver;

#endif //DEEPSOLVER_EXCEPTIONS_H;
