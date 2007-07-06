/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * enumeration.h
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

#ifndef DRILLER_DATABASE_ENUMERATION_H
#define DRILLER_DATABASE_ENUMERATION_H

// Disable warnings about throw specifications in VS 2003
#ifdef _MSC_VER
#pragma warning(disable: 4290)
#endif

#include <list>
#include <map>
#include <string>
#include "../errors.h"
#include "misc.h"

namespace Errors {

/**
  Thrown when a duplicate ID is added to an enumeration column
*/
class DuplicateEnumID : public BaseError {
public:
  /**
    Default constructor

    @param id The duplicated ID
  */
  DuplicateEnumID(Driller::uint8 id);

  /**
    Return an error message indicating the duplicated ID

    @return A message suitable for being shown to a user
  */
  virtual std::string error_message() const throw ();

protected:
  /**
    The duplicated ID
  */
  unsigned int id;
};

/**
  Thrown when the maximum number of enumeration cases has been reached for a
  column
*/
class MaxEnumCases : public BaseError {
public:
  /** Default constructor */
  MaxEnumCases();

  /**
    Return an error message indicating that no more enumeration cases may be
    added

    @return A message suitable for being shown to a user
  */
  virtual std::string error_message() const throw ();
};

} // namespace

namespace Driller {

/**
  A single case in an enumeration case
*/
class EnumCase {
public:
  /**
    Default constructor

    @param id The case's ID
    @param value The case's value
  */
  EnumCase(const unsigned int id, const std::string& value) throw();

  /** This case's ID */
  unsigned int id;

  /** This case's value */
  std::string value;
};

/** An enumeration of ID / String pairs */
class Enumeration {
public:
  /** Default constructor. Creates an empty enumeration */
  Enumeration() throw();

  /**
    Add a case to this enumeration. The ID will be chosen from one of
    the available IDs

    @param value The string value of the case
  */
  void add_case(const std::string& value) throw (Errors::MaxEnumCases);

  /**
    Add a case to this enumeration, with a specified ID and value. This is
    more dangerous than using add_case(), since it might throw a
    DuplicateEnumID exception

    @param id The ID for the new case
    @param value The value for the new case
  */
  void add_case(const uint8 id, const std::string& value)
    throw (Errors::DuplicateEnumID, Errors::MaxEnumCases);

  /**
    Change the ID of a case

    @param id The current enumeration ID
    @param new_id What the ID should be changed to
  */
  void change_id(const uint8 id, const uint8 new_id)
    throw (Errors::DuplicateEnumID);

  /**
    Change the value of an case

    @param id The ID of the case
    @param new_value What the case's value should be changed to
  */
  void change_value(const uint8 id, const std::string& new_value)
    throw ();

  /**
    Remove a case from this enumeration

    @param id The ID of the case to remove
  */
  void remove_id(const uint8 id) throw ();

  /**
    Get the string value for a given ID. If a non-existant id is chosen, will
    return the string ""

    @return The value of the case with the given ID
  */
  std::string get_value(const uint8 id) const throw ();

  /**
    Return how many cases are in this enumeration

    @return How many cases are in this enumeration
  */
  unsigned int case_count() const throw();

  std::list<EnumCase> get_case_list() const throw();

protected:
  /** Maps between case ID and string */
  std::map<uint8, std::string> cases;
};

} // namespace

#endif // DRILLER_DATABASE_ENUMERATION_H
