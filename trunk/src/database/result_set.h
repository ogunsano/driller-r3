/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * result_set.h
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

#ifndef DRILLER_DATABASE_RESULT_SET_H
#define DRILLER_DATABASE_RESULT_SET_H

#include "block_allocator.h"

namespace Driller {

class Table;

/**
  A result set, basically just a thin wrapper around a char*** to automatically
  de-allocate the used memory
*/
class ResultSet {
public:
  /**
    Construct a new ResultSet, with the given number of rows and columns

    @param rows How many rows will be in the result
    @param columns How many columns will be in the result
  */
  ResultSet(const Table& table, unsigned int rows, unsigned int columns)
    throw ();

  /**
    De-allocate all memory used by the result set
  */
  ~ResultSet() throw ();

  /**
    Set a single result cell

    @param row The row of the cell
    @param column The column of the cell
    @param value The new value of the cell
  */
  void set_cell(const unsigned int row, const unsigned int column,
    const char* value) throw ();

  /**
    Retrieve a row of results

    @param row The row to retrieve

    @return The cell values for the given row
  */
  const char** operator[](const unsigned int row) const throw ();

  /**
    Get how many rows are in the result set

    @return How many rows are in the result set
  */
  unsigned int row_count() const throw ();

  /**
    Get how many columns are in the result set

   @return How many columns are in the result set
  */
  unsigned int column_count() const throw ();

  /** The table this result set was extracted from */
  const Table& table;

protected:
  /** How many rows are in the result set */
  const unsigned int rows;

  /** How many columns are in the result set */
  const unsigned int columns;

  /** Stores pointers to the cell data*/
  char** data;

  /** Used to allocate memory in large blocks */
  BlockAllocator allocator;
};

} // namespace

#endif // DRILLER_DATABASE_RESULT_SET_H
