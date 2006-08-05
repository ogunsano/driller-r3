/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * mysql_sink.h
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

#ifndef DRILLER_st_mysql_SINK_H
#define DRILLER_st_mysql_SINK_H

#include "data_sink.h"
#include "errors.h"

struct st_mysql;

namespace Errors {

class MySQLError : public BaseError {
public:
  /**
    Default constructor

    @param connection The MySQL connection that caused this error
  */
  MySQLError(st_mysql* connection) throw();

  /** Default destructor */
  virtual ~MySQLError() throw();

  /**
    Return the MySQL error string

    @return A message suitable for being shown to a user
  */
  virtual std::string error_message() const throw();

protected:
  const std::string message;
};

} // namespace

namespace Driller {

/**
  A MySQLSink will extract data into a MySQL database
*/
class MySQLSink : public DataSink {
public:
  /**
    Default constructor

    @param host The host to connect to. This may be either a string
    (data.foo.com), or IP address (192.168.x.x)
    @param username The user to connect as
    @param password The password of the user to connect as
    @param database The database to extract to
  */
  MySQLSink(
    const std::string& host,
    const std::string& username,
    const std::string& password,
    const std::string& database) throw (Errors::MySQLError);

  /** Default destructor */
  virtual ~MySQLSink() throw();

  MySQLSink& operator<<(const Database& db) throw (Errors::MySQLError);

protected:
  MySQLSink& operator<<(const Table& table) throw (Errors::MySQLError);

  /**
    Make a string safe to send to MySQL

    @param string The string to make safe

    @return A pointer to the safe version of the string. Do not free or delete
    this. This pointer will be overwritten every time this function is called
  */
  const char* make_safe_string(const std::string& string) const throw();

  /**
    Make a string safe for being a name in MySQL

    @param name The possibly unsafe name

    @return A pointer to the safe version of the name. Do not free or delete
    this. This pointer will be overwritten every time this function is called
  */
  const char* make_safe_name(std::string name) const throw();

  /**
    Send a MySQL query, checking for errors and throwing an exception if needed

    @param query The query to send
  */
  void send_query(const std::string& query) const throw(Errors::MySQLError);

  /**
    Get the SQL description for a column's type

    @param col The column

    @return The SQL type definition for the column
  */
  std::string sql_type_from_column(const Column& col) const throw();

  st_mysql* connection;
};

} // namespace

#endif // DRILLER_st_mysql_SINK_H
