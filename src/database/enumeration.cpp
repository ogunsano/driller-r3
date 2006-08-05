#include "enumeration.h"
#include <sstream>

namespace Errors {

DuplicateEnumID::DuplicateEnumID(Driller::uint8 _id):
id(_id){}

std::string DuplicateEnumID::error_message() const throw () {
  std::ostringstream out;
  out << "Duplicate ID '" << id << "'";
  return out.str();
}

MaxEnumCases::MaxEnumCases(){}

std::string MaxEnumCases::error_message() const throw () {
  return "Only 255 cases may be added to each enumeration";
}

}

namespace Driller {

EnumCase::EnumCase(const unsigned int _id, const std::string& _value) throw ():
  id(_id), value(_value){}

Enumeration::Enumeration() throw () {}

void Enumeration::add_case(const std::string& value)
  throw (Errors::MaxEnumCases) {

  if (cases.size() >= 255){
    throw Errors::MaxEnumCases();
  }

  // Find the first available enumeration ID
  uint8 id;
  for (id = 0; cases.count(id) > 0; id++){}

  cases[id] = value;
}

void Enumeration::add_case(const uint8 id, const std::string& value)
  throw (Errors::DuplicateEnumID, Errors::MaxEnumCases) {

  if (cases.size() >= 255){
    throw Errors::MaxEnumCases();
  }

  if (cases.count(id) > 0){
    throw Errors::DuplicateEnumID(id);
  }

  cases[id] = value;
}

void Enumeration::change_id(const uint8 id, const uint8 new_id)
  throw (Errors::DuplicateEnumID) {

  if (id == new_id){
    return;
  }

  // Check that the ID has not already been used
  if (cases.count(new_id) > 0){
    throw Errors::DuplicateEnumID(new_id);
  }

  // Add the new ID to the map
  cases[new_id] = cases[id];

  // Erase the old ID from the map
  cases.erase(id);
}

void Enumeration::change_value(const uint8 id, const std::string& new_value)
  throw () {

  cases[id] = new_value;
}

void Enumeration::remove_id(const uint8 id) throw () {
  if (cases.count(id) > 0){
    cases.erase(id);
  }
}

std::string Enumeration::get_value(const uint8 id) const throw () {
  if (cases.count(id) > 0){
    // FIXME
    // This is very ugly, but map doesn't seem to have a const operator[]
    std::map<uint8, std::string>* cases_ptr =
      const_cast<std::map<uint8, std::string>*>(&cases);
    return (*cases_ptr)[id];
  }

  return "";
}

unsigned int Enumeration::case_count() const throw () {
  return cases.size();
}

std::list<EnumCase> Enumeration::get_case_list() const throw () {
  std::list<EnumCase> list;

  std::map<uint8, std::string>::const_iterator iter;
  for (iter = cases.begin(); iter != cases.end(); iter++){
    list.push_back(EnumCase(
      (*iter).first,
      (*iter).second
    ));
  }

  return list;
}

} // namespace
