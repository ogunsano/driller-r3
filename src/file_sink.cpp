/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * file_sink.cpp
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

#include "file_sink.h"
#include <errno.h>
#include <fstream>

namespace Driller {

FileSink::FileSink(const std::string& _directory) throw ():
  directory(_directory){}

FileSink::~FileSink() throw () {}

void FileSink::output_table(const Table& table, const unsigned int row_limit)
  throw (Errors::FileReadError, Errors::FileWriteError) {

  std::string real_name = directory + "/" + table.get_name() + ".txt";

  std::ofstream file(real_name.c_str());

  if (!file.is_open()){
    throw Errors::FileWriteError(real_name, errno);
  }

  const ResultSet* result = table.extract_data(row_limit);
  for (unsigned int row = 0; row < result->row_count(); row++){
    const char** data = (*result)[row];
    for (unsigned int col = 0; col < result->column_count(); col++){
      file << data[col] << "\t";
    }
    file << "\n";
  }
  file.close();

  delete result;
}

} // namespace
