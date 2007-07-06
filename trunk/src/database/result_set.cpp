/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * result_set.cpp
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

#include "result_set.h"

namespace Driller {

ResultSet::ResultSet(const Table& _table, unsigned int _rows,
  unsigned int _columns) throw ():

  table(_table),
  rows(_rows),
  columns(_columns),
  allocator(rows * columns * 5) { // most strings are 5 bytes or less

  data = new char*[rows * columns];
  for (unsigned int ii = 0; ii < rows * columns; ii++){
    data[ii] = NULL;
  }
}

ResultSet::~ResultSet() throw () {
  delete[] data;
}

void ResultSet::set_cell(
  const unsigned int row, const unsigned int column,
  const char* value) throw () {

  const unsigned int value_length = static_cast<const unsigned int>(
    strlen(value));
  data[(row * columns) + column] = allocator.allocate<char>(
    value_length + 1);
  memcpy(data[(row * columns) + column], value, value_length + 1);
}

const char** ResultSet::operator[](const unsigned int row) const throw () {
  return const_cast<const char**>(data + (row * columns));
}

unsigned int ResultSet::row_count() const throw () {
  return rows;
}

unsigned int ResultSet::column_count() const throw () {
  return columns;
}

} // namespace
