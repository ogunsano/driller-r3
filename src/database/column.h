/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * column.h
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

#ifndef DRILLER_DATABASE_COLUMN_H
#define DRILLER_DATABASE_COLUMN_H

// Disable warnings about throw specifications in VS 2003
#ifdef _MSC_VER
#pragma warning(disable: 4290)
#endif

#include <vector>
#include <map>
#include <string>
#include "../errors.h"
#include "../file_errors.h"
#include "misc.h"
#include "enumeration.h"

// For parsing saved database files
#include <libxml/parser.h>
#include <libxml/tree.h>

namespace Driller {

/**
  A single column of a table
*/
class Column {
public:
  /**
    Default constructor for a column

    @param name The name of the column
    @param type The type of the column
    @param offset The offset from the start of each row to this column
    @param length The length of the column, only used for types that require
    it
    @param indexed If the column should be indexed when output to a RDB
  */
  Column(
    const std::string& name = "",
    const ColumnType type = COLUMN_UNKNOWN,
    const unsigned int offset = 0,
    const unsigned int length = 1,
    const bool indexed = false) throw ();

  /**
    Create a column from an XML node

    @param node The node representing this column
  */
  Column(xmlNode* node) throw (Errors::DuplicateEnumID, Errors::FileParseError);

  /**
    Set this column's type

    @param type The new type for this column
  */
  void set_type(const ColumnType type) throw ();

  /**
    Get this column's type

    @return This column's type
  */
  ColumnType get_type() const throw ();

  /**
    Set this column's name

    @param name The new name for this column
  */
  void set_name(const std::string& name) throw ();

  /**
    Get this column's offset

    @return This column's offset
  */
  std::string get_name() const throw ();

  /**
    Set this column's offset

    @param offset The new offset for this column
  */
  void set_offset(const unsigned int offset) throw ();

  /**
    Get this column's offset

    @return This column's offset
  */
  unsigned int get_offset() const throw ();

  /**
    Set this column's length

    @param length The new length for this column
  */
  void set_length(const unsigned int length) throw ();

  /**
    Get this column's length

    @return This column's length
  */
  unsigned int get_length() const throw ();

  /**
    Set whether this column should be indexed when output to an RDBMS

    @param indexed Whether this column should be indexed
  */
  void set_indexed(const bool indexed) throw ();

  /**
    Get whether this column should be indexed when output to an RDBMS

    @return Whether this column should be indexed
  */
  bool get_indexed() const throw ();

  /**
    Get whether this column needs the "length" field
  */
  bool needs_length() const throw ();

  static int8 get_int8(const uint8* file) throw () {
    return file[0];
  }

  static uint8 get_uint8(const uint8* file) throw () {
    return file[0];
  }

  static int16 get_int16(const uint8* file) throw () {
    return file[0] + file[1]*256;
  }

  static uint16 get_uint16(const uint8* file) throw () {
    return file[0] + file[1]*256u;
  }

  static int32 get_int32(const uint8* file) throw () {
    return file[0] + file[1]*256 + file[2]*65536 + file[3]*16777216;
  }

  static uint32 get_uint32(const uint8* file) throw () {
    return file[0] + file[1]*256u + file[2]*65536u + file[3]*16777216u;
  }

  /**
    Extract data from this column

    @param data The start of the row containing this column
    @param buffer A memory buffer used to store temporary formatting strings
    @param buffer_size The size of buffer

    @return A character string representing this column's data
  */
  const char* extract_data(const uint8* data,
                           char*& buffer,
                           unsigned int& buffer_size) const throw ();

  /** If this column is an enumeration, this will store enumeration cases */
  Enumeration enumeration;

protected:
  /** What type of column this is */
  ColumnType type;

  /** Name of the column */
  std::string name;

  /** Offset from the start of each row to this column, in bytes */
  unsigned int offset;

  /** Length of the column. Currently only used for strings and blobs */
  unsigned int length;

  /** Whether this column should be indexed */
  bool indexed;
};

} // namespace

#endif // DRILLER_DATABASE_COLUMN_H
