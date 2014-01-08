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

#ifndef DEEPSOLVER_FILE_H
#define DEEPSOLVER_FILE_H

namespace Deepsolver
{
  /**\brief The wrapper for file operations
   *
   * This class simplifies the mostly popular operations with files. It is
   * generally purposed to take care over the error code
   * returned by system calls and throw SystemException if there are any problems.
   *
   * \sa Directory SystemException
   */
  class File
  {
  public:
    /**\brief The default constructor*/
    File()
      : m_fd(-1) {}

    /**\brief The destructor*/
    virtual ~File()
    {
      close();
    }

  public:
    /**\brief Opens existing file in read/write mode
     *
     * \param [in] fileName The name of the file to open
     */ 
    void open(const std::string fileName);

    /**\brief Opens existing file in read-only mode
     *
     * \param [in] fileName The name of the file to open
     */ 
    void openReadOnly(const std::string& fileName);

    /**\brief Creates a new file in file system
     *
     *
     * \param [in] fileName The name of the file to create
     */
    void create(const std::string& fileName);

    /**\brief Closes previously opened file
     *
     * This method closes previously opened file. If file is already closed
     * or never be opened it is not an error, in this case nothing is
     * done at all. This method is always called automatically on object
     * destruction.
     */
    void close();

    /**\brief Returns the opening state of  this object
     * \return Non-zero if current object is associated with opened file or zero otherwise
     */
    bool opened() const
    {
      return m_fd != -1;
    }

    /**\brief Reads data from an opened file
     *
     * The length of the buffer is not limited. In case of providing the large buffer this
     * method makes several reading attempts of smaller size. In addition this method takes care to be sure the operation
     * is performed completely until entire requested data is read or error
     * occurred.
     *
     * \param [out] buf The buffer to save read data to
     * \param [in] bufSize The size of the provided buffer
     *
     * \return The number of read bytes, can be less then bufSize only if reading near the end of file
     */
    size_t read(void* buf, size_t bufSize);

    /**\brief Writes data to an opened file
     *
     * The length of the buffer is not limited. In case of providing the large buffer this
     * method makes several writing attempts of smaller size. In addition this method takes care to be sure the operation
     * is performed completely until entire requested data is written or error
     * occurred.
     *
     * \param [in] buf The buffer to write data from
     * \param [in] bufSize The size of the provided buffer
     * \return The number of the successfully written bytes
     */
    size_t write(const void* buf, size_t bufSize);

    /**\brief Returns the handle of an opened file
     * \return The handle of the opened file
     */
    int getFd() const
    {
      assert(m_fd >= 0);
      return m_fd;
    }

    /**\brief Reads the file as a text file and saves lines in a string vector
     *
     * \param [out] lines The reference to a string vector to save lines to
     */
    void readTextFile(StringVector& lines);

    /**\brief Reads the file as a text file and saves lines as one long string value
     * \param [out] text The reference to a string object to save read data to
     */
    void readTextFile(std::string& text);

    /**\brief Returns the file name without parent directories
     *
     * \param [in] The file name to take base part from
     *
     * \return The file name without parent directories
     */
    static std::string baseName(const std::string& fileName);

    /**\brief Extracts the file name of  a URL
     *
     * 
     * \param [in url The URL value to extract file name from
     *
     * \return The extracted file name
     */
    static std::string baseNameFromUrl(const std::string& url);

    /**\brief Removes the file or remove the hard link
     *
     * \param [in] fileName The name of the file to remove hard link to
     */
    static void unlink(const std::string& fileName);

    /**\brief Moves (renames) file inside of the same file system
     *
     * \param [in oldFileName The path to move file from
     * \param [in]] newFileName The path to move file to
     */
  static void move(const std::string& oldFileName, const std::string& newFileName);

    /**\brief Checks if the file is a regular file
     *
     * \param [in] fileName The name of the file to check
     *
     * \return Non-zero if the file is a regular file (not a directory, not a symlink etc) or zero otherwise
     */
    static bool isRegFile(const std::string& fileName);

    /**\brief Checks if the file is a directory
     *
     * \param [in] fileName The name of the file to check
     *
     * \return Non-zero if the file is a directory or zero otherwise
     */
    static bool isDir(const std::string& fileName);

    /**\brief Checks if the file is a symlink
     *
     * \param [in] fileName The name of the file to check
     *
     * \return Non-zero if the file is a symlink or zero otherwise
     */
    static bool isSymLink(const std::string& fileName);

    /**\brief Loads file content into file system cache
     *
     * \param [in] fileName The path of the file to load data from
     */
    static void readAhead(const std::string& fileName);

  protected:
    int m_fd;
  }; //class File;
} //namespace Deepsolver;

#endif //DEEPSOLVER_FILE_H;
