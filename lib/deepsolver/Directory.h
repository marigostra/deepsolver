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

#ifndef DEEPSOLVER_DIRECTORY_H
#define DEEPSOLVER_DIRECTORY_H

namespace Deepsolver
{
  /**\brief The wrapper for file system directory operations
   *
   * This class wraps the most popular operations with directories in
   * files system. Its generally purposed for automatic verifying the error code
   * returned by system calls and throwing SystemException instances on failures.
   *
   * \sa File SystemException
   */
  class Directory
  {
  public:
    class Iterator;

  public:
    /**\brief Checks if the directory exists 
     *
     * \param [in] path The path of the directory to check
     *
     * \return Non-zero if the directory exists and zero otherwise
     */
    static bool exists(const std::string& path);

    /**\brief Creates new directory if it does not exist
     *
     * \param [in path The path of the directory to create]
     */
    static void ensureExists(const std::string& path);

    /**\brief Prepares an empty directory
     *
     * This method helps to get an empty directory at specified path. It can
     * do in two modes depending on the action performed in case when the
     * directory already exists. In first mode this method always erases
     * directory content, in second it only checks is existing directory
     * empty or not but never touches its contents. The result of checking in
     * second mode is returned through the return value.
     *
     * \param [in] path The path of the directory to get empty
     * \param [in] needEraseContent Allows to delete all subdirectories and files if the directory exists and not empty
     *
     * \return Non-zero if the directory is empty and zero if it is not empty and we may not delete its content
     */
    static bool ensureExistsAndEmpty(const std::string& name, bool needEraseContent = 0);

    /**\brief Removes any content of the directory
     *
     * \param [in] path The path of the directory to erase content in
     */
    static void eraseContent(const std::string& path);

    /**\brief Checks if the directory is empty
     *
     * Note, if the directory does not exists this method 
     * signales that through SystemException and returns nothing.
     * 
     * \param [in] path The path of the directory to check
     *
     * \return Non-zero if the directory is empty and zero otherwise  
     */
    static bool empty(const std::string& path);

    /**\brief Removes the empty directory
     *
     * \param [in] path The path of the directory to remove
     */
    static void remove(const std::string& path);

    /**\brief Combines two parts of the UNIX path
     *
     * This method just takes care of the double slashes absence in constructed name.
     *
     * \param [in] part1 The first part of the path to construct
     * \param [in] part2 The second part of the path to construct
     *
     * \return The constructed path
     */
    static std::string mixNameComponents(const std::string& part1, const std::string& part2);

    /**\brief Creates the iterator over the entries in the directory
     *
     * This method creates the iterator prepared to enumerate all files
     * and subdirectories inside of the directory given by path. If the
     * directory is inaccessible for reading the SystemException will be
     * thrown.
     *
     * \param [in] path The path of the directory to enumerate files in
     *
     * \return The prepared iterator
     */
    static std::shared_ptr<Iterator> enumerate(const std::string& path);
  }; //class Directory;

  /**\brief The iterator over the files and subdirectories
   *
   * This class enumerates all items in the directory. Each item can be
   * examined both as just the name of a file and as a full path with all parent
   * directories.
   *
   * The iterator is created pointed before the first item. Hence, a user
   * has to call moveNext() method at least once to be able call methods providing item information.
   */
  class Directory::Iterator
  {
  public:
    typedef std::shared_ptr<Iterator> Ptr;

  public:
    /**\brief The constructor 
     *
     * \param [in] path The name of the directory to be used for proper full path construction
     * \param [in] dir The valid directory handle to read data from
     */
    Iterator(const std::string& path, DIR* dir)
      : m_path(path),
	m_dir(dir) {}

    /**\brief The destructor*/
    virtual ~Iterator()
    {
      if (m_dir)
	closedir(m_dir);
    }

  public:
    /**\brief Moves the iterator to next item 
     *
     * This method moves the iterator to the first item on its first
     * invocation or to the next on each consequent. The method returns zero
     * if next item does not exist. If method returns zero on first call that
     * means the directory is empty.
     *
     * \return Non-zero if next item exists or zero otherwise
     */
    bool moveNext();

    /**\brief Returns the name of the directory item this iterator is pointing to
     * \return The name of the item the iterator is pointing
     */
    std::string name() const;

    /**\brief Returns the name of the directory item this iterator is pointing to with all parent directories
     * \return The full path of the item this iterator is pointing to
     */
    std::string fullPath() const;

  private:
    const std::string m_path;
    DIR* m_dir;
    std::string m_currentName;
  }; //class Directory::Iterator;
} //namespace Deepsolver;

#endif //DEEPSOLVER_DIRECTORY_H;
