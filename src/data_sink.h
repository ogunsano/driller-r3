/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * data_sink.h
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

#ifndef DRILLER_DATA_SINK_H
#define DRILLER_DATA_SINK_H

#include "database/database.h"

namespace Driller {

/**
  A DataSink serves as a place to shove data extracted with a Database

  For example, you might create a MySQLDataSink sink, and Database db. To
  extract data, simply type <code>sink << db;</code>
*/
class DataSink {
public:
  /** Default constructor, does nothing */
  DataSink();

  /** Default destructor, does nothing */
  virtual ~DataSink();

  /**
    Extract data from a table to wherever this data sink is directed to

    @param table The table to extract
  */
  virtual void output_table(
    const Table& table,
    const unsigned int row_limit = 0) = 0;

  /**
    Output an entire database to this sink

    @param database The database to extract from
  */
  void output_database(const Database& db);
};

} // namespace

#endif // DRILLER_DATA_SINK_H
