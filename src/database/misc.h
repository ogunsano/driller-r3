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


#ifndef DRILLER_DATABASE_MISC_H
#define DRILLER_DATABASE_MISC_H

#include <string>
#include <libxml/tree.h>
#include "../file_errors.h"

namespace Errors {

/** Any kind of error parsing a description file */
class FileParseError : public FileError {
public:
  /**
    Default constructor

    @param file The file that could not be parsed
  */
  FileParseError(const std::string& file)
    throw ();

  /** Empty destructor */
  virtual ~FileParseError() throw () {}

  /**
    Returns an error message stating the file could not be parsed, along with
    the message given when this error was thrown

    @return A message suitable for being shown to a user
  */
  virtual std::string error_message() const throw ();

protected:
  /** The error message for this parse error */
  std::string error;
};

/** When an attribute is missing while parsing an element */
class MissingAttributeError : public FileParseError {
public:
  /**
    Default constructor

    @param file The file that could not be parsed
    @param element The name of the element missing an attribute
    @param attribute The name of the missing attribute
  */
  MissingAttributeError(const std::string& file, const std::string& element,
    const std::string& attribute) throw ();

  /** Empty destructor */
  virtual ~MissingAttributeError() throw () {}
};

/** When an attribute contains a value it shouldn't */
class InvalidAttributeError : public FileParseError {
public:
  /**
    Default constructor

    @param file The file this was constructed from
    @param attribute The invalid attribute
    @param bad_value The bad value of attribute
  */
  InvalidAttributeError(
    const std::string& file,
    const std::string& attribute,
    const std::string& bad_value) throw ();

  /** Default destructor */
  ~InvalidAttributeError() throw ();
};

} // namespace

namespace Driller {

// Integer data types
#ifdef WIN32
  typedef char int8;
  typedef unsigned char uint8;
  typedef short int int16;
  typedef unsigned short int uint16;
  typedef long int int32;
  typedef unsigned long int uint32;
#else
  typedef int8_t int8;
  typedef uint8_t uint8;
  typedef int16_t int16;
  typedef uint16_t uint16;
  typedef int32_t int32;
  typedef uint32_t uint32;
#endif

/**
  Possible data types
*/
typedef enum {
  COLUMN_UNKNOWN = 0, /**< Type is not known */

  COLUMN_BOOL,        /**< Boolean value */

  // Integer types
  COLUMN_INT8,        /**< Signed 8-bit integer */
  COLUMN_UINT8,       /**< Unsigned 8-bit integer (AKA byte) */
  COLUMN_INT16,       /**< Signed 16-bit integer */
  COLUMN_UINT16,      /**< Unsigned 16-bit integer */
  COLUMN_INT32,       /**< Signed 32-bit integer */
  COLUMN_UINT32,      /**< Unsigned 32-bit integer */

  // String types
  COLUMN_BLOB,        /**< A blob of bytes. It is output as a hexidecimal string */
  COLUMN_STRING,      /**< Fixed length, NULL-terminated string */
  COLUMN_VARSTRING,   /**< Variable-length string. Not NULL terminated */

  // Special types
  COLUMN_PHONE,       /**< Phone number */
  COLUMN_DATE,        /**< Date */
  COLUMN_CURRENCY,    /**< Money */
  COLUMN_ENUM,        /**< Enumerated type */


  COLUMN_NUM_TYPES    /**< How many types there are, not a real type */
} ColumnType;

/**
  Mapping from ColumnType to string
*/
const std::string column_strings[COLUMN_NUM_TYPES] = {
  "Unknown",

  "Boolean",

  "Signed 8-bit integer",
  "Unsigned 8-bit integer",
  "Signed 16-bit integer",
  "Unsigned 16-bit integer",
  "Signed 32-bit integer",
  "Unsigned 32-bit integer",

  "Binary blob",
  "Fixed-length string",
  "Variable-length string",

  "Phone number",
  "Date",
  "Currency",
  "Enumeration"
};

/**
  "Short" names for the types
*/
const std::string short_column_strings[COLUMN_NUM_TYPES] = {
  "unknown",

  "bool",

  "int8",
  "uint8",
  "int16",
  "uint16",
  "int32",
  "uint32",

  "blob",
  "string",
  "varstring",

  "phone",
  "date",
  "currency",
  "enum"
};

/**
  Get an attribute from some XML Node

  @param node The XML node
  @param attribute The name of the attribute to retrieve

  @return The value of the attribute
*/
std::string get_xml_node_attribute(xmlNode* node,
  const std::string& attribute)
  throw (Errors::MissingAttributeError);

} // namespace

#endif // DRILLER_DATABASE_MISC_H
