/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * file_errors.h
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef DRILLER_FILE_ERRORS_H
#define DRILLER_FILE_ERRORS_H

#include "errors.h"

namespace Errors {

/**
  Class for errors involving File I/O
*/
class FileError : public BaseError {
public:
  /**
    Default constructor

    @param file The name of the file this error is about
    @param error_code The error code, which will be fed into strerror
    for a more detailed error message
  */
  FileError(const std::string& file, const int error_code = -1) throw();

  /** Empty destructor */
  virtual ~FileError() throw() {}

  /**
    Return a "generic file error" message

    @return A message suitable for being shown to a user
  */
  virtual std::string error_message() const throw();

protected:
  /**
    The file this error involves
  */
  const std::string file;

  /**
    The error code for this error, if available. If this is 0, it shouldn't
    be used
  */
  const int error_code;
};

/**
  Class for errors during file input
*/
class FileReadError : public FileError {
public:
  /**
    Default constructor

    @param file The file that couldn't be read
    @param error_code The error code, which will be fed into strerror
    for a more detailed error message
  */
  FileReadError(const std::string& file, const int error_code = -1) throw();

  /**
    Returns an error message stating the file could not be read. If possible,
    it also gives a reason

    @return A message suitable for being shown to a user
  */
  virtual std::string error_message() const throw();
};

/**
  Class for errors during file output
*/
class FileWriteError : public FileError {
public:
  /**
    Default constructor

    @param file The file that couldn't be written to
    @param error_code The error code, which will be fed into strerror
    for a more detailed error message
  */
  FileWriteError(const std::string& file, const int error_code = -1) throw();

  /**
    Returns an error message stating the file could not be written to. If
    possible, it also gives a reason

    @return A message suitable for being shown to a user
  */
  virtual std::string error_message() const throw();
};

} // namespace

#endif // FILE_ERRORS
