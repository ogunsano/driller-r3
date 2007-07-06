/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * database.h
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

#ifndef DRILLER_DATABASE_H
#define DRILLER_DATABASE_H

// Disable warnings about throw specifications in VS 2003
#ifdef _MSC_VER
#pragma warning(disable: 4290)
#endif

#include <vector>
#include <string>
#include "../errors.h"
#include "table.h"

namespace Driller {

/**
  Contains many tables
*/
class Database {
public:
  /**
    Default constructor

    @param name The name of the database
  */
  Database(const std::string& name = "") throw ();

  /**
    Load a database from a properly formatted XML file. This will only work for
    files that can be opened by the basic C file API

    @param file The name of the file to load a database from

    @return The Database representing the given file
  */
  static Database from_file(const std::string& file)
    throw (
      Errors::DuplicateEnumID,
      Errors::FileParseError,
      Errors::FileReadError);

  /**
    Load a database from a memory buffer. This is very useful for parsing files
    that cannot be opened using the normal C file API

    @param buffer The in-memory buffer containing the document definition
  */
  static Database from_buffer(const char* buffer)
    throw (Errors::DuplicateEnumID, Errors::FileParseError);

  /**
    Default constructor, calls clear()
  */
  ~Database() throw ();

  /**
    Load a database from a properly formatted XML file

    @param file The name of the file to load a database from
  */
  void load(const std::string& file) throw (
    Errors::DuplicateEnumID,
    Errors::FileParseError,
    Errors::FileReadError);

  /**
    Load a database from a properly formatted XML buffer

    @param buffer The character buffer to parse
  */
  void load_from_buffer(const char* buffer) throw (
    Errors::DuplicateEnumID,
    Errors::FileParseError);

  /**
    Get the number of tables in this database

    @return The number of tables in this database
  */
  unsigned int table_count() const throw();

  /**
    Clear all tables from the database
  */
  void clear() throw ();

  /**
    Set this database's name

    @param name The new name of the database
  */
  void set_name(const std::string& name) throw();

  /**
    Get this database's name

    @return This database's name
  */
  std::string get_name() const throw();

  /**
    Get a list of all tables in the database

    @return Every table in the database
  */
  std::vector<Table> get_tables() const throw();

  /**
    Find the table at the specified index, and return it

    @param index The index of the table to find

    @return The table at index
  */
  Table& table_at(const unsigned int index) throw();

  /**
    Find the table at the specified index, and return it (const version)

    @param index The index of the table to find

    @return The table at index
  */
  const Table& table_at(const unsigned int index) const throw();

  /**
    Add a table to the table list

    @param table The table to add
  */
  void add_table(const Table& table) throw();

  /**
    Remove a table from the table list

    @param index The index of the table to remove
  */
  void remove_table(const unsigned int index) throw();

  /**
    Set the global data path for all databases

    @param new_path The new data path
  */
  static void set_data_path(const std::string& new_path) throw();

  /**
    Get the global data path for all databases

    @return The global data path
  */
  static std::string get_data_path() throw();

protected:
  /**
    The path to where this tables data files are stored. For
    example, if this table's data file is at /opt/db/somefile.dat, data_path
    should be "/opt/db". The last slash is not required.
  */
  static std::string data_path;

  /**
    Name of this database
  */
  std::string name;

  /**
    Tables in the database
  */
  std::vector<Table> tables;
};

/**
  Output a database to a stream, in XML format

  @param out The output stream
  @param db The database
*/
std::ostream& operator<<(std::ostream& out, const Database& db)
  throw (Errors::FileParseError);

} // namespace

#endif // DRILLER_DATABASE_H
