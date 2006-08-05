/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * table.h
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

#ifndef DRILLER_TABLE_H
#define DRILLER_TABLE_H

#include <vector>
#include <string>
#include "errors.h"
#include "column.h"
#include "result_set.h"

namespace Driller {

/**
  Contains a single table in the database
*/
class Table {
public:
  /**
    Create a new table

    @param friendly_name The friendly name of the table
    @param file_name The name of the file this table should read from
    @param data_offset The file's data offset
    @param row_length How long each row of the table is
  */
  Table(
    const std::string& friendly_name = "",
    const std::string& file_name = "",
    const unsigned int data_offset = 0,
    const unsigned int row_length = 1) throw ();

  /**
    Create a table from an XML node

    @param node The node representing this table
  */
  Table(xmlNode* node) throw (Errors::FileParseError, Errors::DuplicateEnumID);

  /**
    Default destructor
  */
  ~Table() throw ();

  /**
    Get the number of columns in this table

    @return The number of columns in this table
  */
  unsigned int column_count() const throw ();

  /**
    Clear all columns from the table
  */
  void clear() throw ();

  /**
    Set this table's name

    @param name The new name of the table
  */
  void set_name(const std::string& name) throw ();

  /**
    Get this table's name

    @return This table's name
  */
  std::string get_name() const throw ();

  /**
    Set the name of the file this table should extract from

    @param file_name The new file name of this table
  */
  void set_file_name(const std::string& file_name) throw ();

  /**
    Get the name of the file this table should extract from

    @return This table's file name
  */
  std::string get_file_name() const throw ();

  /**
    Set this table's data offset

    @param offset The offset from the start of the file to where the data
    starts
  */
  void set_data_offset(const unsigned int offset) throw ();

  /**
    Get this table's data offset

    @return The offset from the start of the file to where the data starts
  */
  unsigned int get_data_offset() const throw ();

  /**
    Set this table's row length

    @param length The length of each row in the table. If the table contains
    variable-length rows, this should be 0
  */
  void set_row_length(const unsigned int length) throw ();

  /**
    Get this table's row length

    @return The length of each row in the table. If the table contains
    variable-length rows, this should be 0
  */
  unsigned int get_row_length() const throw ();

  /**
    Find the column at the specified index, and return it

    @param index The index of the column to find

    @return The column at index
  */
  Column& column_at(const unsigned int index) throw ();

  /** A const version of the previous function */
  const Column& column_at(const unsigned int index) const throw ();

  /**
    Add a table to the table list

    @param column The column to add
  */
  void add_column(const Column& column) throw ();

  /**
    Remove a column from the table

    @param index The index of the column to remove
  */
  void remove_column(const unsigned int index) throw ();

  /**
    Get a list of all columns in the table

    @return Every column in the table
  */
  std::vector<Column> get_columns() const throw ();

  /**
    Extract data from a table

    @param row_limit If this is greater than 0, limit the number of rows
    extracted from the table

    @return The extracted data. This should be deleted.
  */
  const ResultSet* extract_data(const unsigned int row_limit = 0) const
    throw(Errors::FileReadError);

protected:
  /**
    Columns in the table
  */
  std::vector<Column> columns;

  /**
    Friendly name of a table. For example, if the filename is proc_log.dat,
    the friendly name might be "Procedure Log"
  */
  std::string friendly_name;

  /**
    File name of the table
  */
  std::string file_name;

  /**
    Offset from the start of the file to where the data starts
  */
  unsigned int data_offset;

  /**
    Length of each row in the table. If the table contains variable-length
    rows, this should be 0
  */
  unsigned int row_length;

private:
  /**
    Used for storing temporary data for data extraction
  */
  struct ExtractionState {
    /** The size of the loaded file */
    unsigned int data_length;

    /** The file's data */
    uint8* data;
  };

  /**
    Load data from a file into an array of bytes

    @return A new ExtractionState containing information on the loaded file
  */
  ExtractionState* load_data() const throw (Errors::FileReadError);

  /**
    Unload the data, by freeing it or munmapping it or whatever

    @param state The state to unload
  */
  void unload_data(ExtractionState* state) const throw ();
};

} // namespace

#endif // DRILLER_TABLE_H
