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

#ifndef DEEPSOLVER_EXCEPTION_MESSAGES_EN_H
#define DEEPSOLVER_EXCEPTION_MESSAGES_EN_H

namespace Deepsolver
{
  /**\brief The error messages generator for English language
   *
   * This class generates complete exception messages, which are really
   * appropriate to be shown to user, but only in case when you are sure
   * that English language suits. Although every exception instance can
   * provide single-line description through its getMessage() method, these
   * values are purposed only for logging. They are not user-friendly and
   * do not support any localization technique. 
   *
   * Respecting the idea that errors messages should be constructing in
   * various languages, there are set of classes taking care of that. This
   * generator does this work for English language and everybody is
   * encouraged to suggest flexible approaches for others. 
   *
   * This class is designed as a visitor. You should visit some exception
   * instances with it and call getText() method to obtain generated
   * text. You can do multiple visits, it is safe but such operation is
   * meaningless.
   */
  class ExceptionMessagesEn: public ExceptionVisitor
  {
  public:
    /**\brief The default constructor*/
    ExceptionMessagesEn() {}

    /**\brief The destructor*/
    virtual ~ExceptionMessagesEn() {}

  public:
    /**\brief Returns generated text
     *
     * You should use this method only after visiting some exception
     * instances, otherwise it returns an empty string. 
     *
     * \return Generated text
     */
    std::string getText() const
    {
      return m_stream.str();
    }

  public://ExceptionVisitor;
    void visit(const SystemException& e);
    void visit(const ConfigFileException& e);
    void visit(const ConfigException& e);
    void visit(const CurlException& e);
    void visit(const PkgBackEndException& e);
    void visit(const OperationCoreException& e);
    void visit(const TaskException& e);

    /**\brief Returns the constructed message
     * \return The constructed message
     >*/
    std::string getMsg() const
    {
      return m_stream.str();
    }

  private:
    std::ostringstream m_stream;
  }; //class ExceptionMessagesEn;
} ///namespace Deepsolver;

#endif //DEEPSOLVER_EXCEPTION_MESSAGES_EN_H;
