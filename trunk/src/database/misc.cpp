/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * misc.cpp
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

#include "misc.h"
#include <libxml/xmlerror.h>

namespace Errors {

FileParseError::FileParseError(const std::string& _file) throw ():
  FileError(_file){

  xmlError* last_error = xmlGetLastError();
  if (last_error){
    error = last_error->message;
  }
}

std::string FileParseError::error_message() const throw () {
  return "Error parsing " + file + ": " + error;
}

MissingAttributeError::MissingAttributeError(
  const std::string& _file,
  const std::string& _element,
  const std::string& _attribute) throw ():

  FileParseError(_file){

  error = "missing attribute '" + _attribute + "' for '" + _element + "'";
}

InvalidAttributeError::InvalidAttributeError(
  const std::string& file,
  const std::string& attribute,
  const std::string& bad_value) throw ():

  FileParseError(file) {

  error = "Bad value '" + bad_value + "' for attribute '" + attribute + "'";
}

InvalidAttributeError::~InvalidAttributeError() throw () {}

} // namespace

namespace Driller {

std::string get_xml_node_attribute(xmlNode* node,
  const std::string& attribute)
   throw (Errors::MissingAttributeError) {

  xmlChar* raw_value = xmlGetProp(node, BAD_CAST(attribute.c_str()));
  if (!raw_value){
    throw Errors::MissingAttributeError(
      (node->doc->name ? node->doc->name : ""),
      reinterpret_cast<const char*>(node->name), attribute);
  }
  std::string value = reinterpret_cast<const char*>(raw_value);
  xmlFree(raw_value);
  return value;
}

} // namespace
