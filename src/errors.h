/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * errors.h
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

#include <stdexcept>
#include <string>
#include <ostream>

#ifndef DRILLER_ERRORS_H
#define DRILLER_ERRORS_H

namespace Errors {

/**
  The base class for all errors. This class is easier to output than
  std::exception
*/
class BaseError : public std::exception {
  friend std::ostream& operator<<(std::ostream& out,
    const BaseError& error) throw();
public:
  /** Empty constructor */
  BaseError() throw(){}

  /** Empty destructor */
  virtual ~BaseError() throw() {}

  /**
    Get the message for this exception

    @return A message suitable for being shown to a user
  */
  virtual std::string error_message() const throw() = 0;

  /**
    Get the message for this exception in a C string. You should use
    error_message() instead

    @return A user-friendly error message that must be deleted after use
  */
  const char* what() const throw();
};

/**
  A generic error, when nothing better is available
*/
class GenericError : public BaseError {
public:
  /**
    Create a new generic error with the specified message

    @param message The error message
  */
  GenericError(const std::string& message) throw();

  /** Empty destructor */
  virtual ~GenericError() throw() {}

  /**
    Get the message for this exception

    @return Whatever this generic error was constructed with for a message
  */
  virtual std::string error_message() const throw();

protected:
  const std::string message;
};

} // namespace

#endif // DRILLER_ERRORS_H
