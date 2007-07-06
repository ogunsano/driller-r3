/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * file_sink.h
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

#ifndef DRILLER_FILE_SINK_H
#define DRILLER_FILE_SINK_H

#include "data_sink.h"
#include "errors.h"

namespace Driller {

/**
  A FileSink will extract data from a Database into a tab-delimited file
*/
class FileSink : public DataSink {
public:
  /**
    Default constructor

    @param directory The directory extracted files should be stored in. This
    must exist, or no data will be output
  */
  FileSink(const std::string& directory) throw ();

  /** Default destructor */
  virtual ~FileSink() throw ();

  void output_table(const Table& table, const unsigned int row_limit = 0)
    throw (Errors::FileReadError, Errors::FileWriteError);

protected:
  const std::string directory;
};

} // namespace

#endif // DRILLER_FILE_SINK_H
