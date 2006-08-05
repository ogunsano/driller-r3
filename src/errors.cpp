/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * errors.cpp
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

#include "errors.h"

namespace Errors {

std::ostream& operator<<(std::ostream& out, const BaseError& error) throw(){
  out << error.error_message();
  return out;
}

const char* BaseError::what() const throw() {
  std::string message = error_message();
  char* return_value = new char[message.size() + 1];
  memcpy(return_value, message.c_str(), message.size() + 1);

  return return_value;
}

GenericError::GenericError(const std::string& _message) throw():
  message(_message){}

std::string GenericError::error_message() const throw(){
  return message;
}

} // namespace
