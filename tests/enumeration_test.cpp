#include <copper.hpp>
#include "../src/database/enumeration.h"

using namespace Driller;

TEST_SUITE(enum_tests) {

FIXTURE(enum_fixture) {
  Enumeration _enum;

  SET_UP {
    _enum = Enumeration();
  }
}

TEST(constructor) {
  Enumeration _enum;
  ASSERT(equal(0u, _enum.case_count()));
}

FIXTURE_TEST(add_case, enum_fixture) {
  _enum.add_case("case_value");

  ASSERT(equal(1u, _enum.case_count()));
  std::list<EnumCase> case_list = _enum.get_case_list();
  std::list<EnumCase>::iterator iter = case_list.begin();
  ASSERT(equal(0u, iter->id));
}

FIXTURE_TEST(add_case_2, enum_fixture) {
  _enum.add_case(10, "value 1");
  _enum.add_case(20, "value 2");

  ASSERT(equal(2u, _enum.case_count()));

  std::list<EnumCase> case_list = _enum.get_case_list();
  std::list<EnumCase>::iterator iter = case_list.begin();

  ASSERT(equal(10u, iter->id));
  ASSERT(equal("value 1", iter->value));
  iter++;

  ASSERT(equal(20u, iter->id));
  ASSERT(equal("value 2", iter->value));

  // TODO: insert a duplicate ID and check for a DuplicateEnumID exception
}

FIXTURE_TEST(change_id, enum_fixture) {
  _enum.add_case("case_value");
  _enum.change_id(0, 1);

  ASSERT(equal(1u, _enum.case_count()));
  std::list<EnumCase> case_list = _enum.get_case_list();
  std::list<EnumCase>::iterator iter = case_list.begin();
  ASSERT(equal(1u, iter->id));
  ASSERT(equal("case_value", iter->value));
}

FIXTURE_TEST(change_value, enum_fixture) {
  _enum.add_case("case_value");
  _enum.change_value(0, "new_value");

  ASSERT(equal(1u, _enum.case_count()));
  std::list<EnumCase> case_list = _enum.get_case_list();
  std::list<EnumCase>::iterator iter = case_list.begin();
  ASSERT(equal(0u, iter->id));
  ASSERT(equal("new_value", iter->value));
}

FIXTURE_TEST(remove_id, enum_fixture) {
  _enum.add_case("case_value");
  _enum.remove_id(0);

  ASSERT(equal(0u, _enum.case_count()));
}

FIXTURE_TEST(get_value, enum_fixture) {
  _enum.add_case("case_value");
  ASSERT(equal("case_value", _enum.get_value(0)));
}

FIXTURE_TEST(case_count, enum_fixture) {
  ASSERT(equal(0u, _enum.case_count()));
  _enum.add_case("case_value");
  ASSERT(equal(1u, _enum.case_count()));
}

FIXTURE_TEST(get_case_list, enum_fixture) {
  _enum.add_case("value 1");
  _enum.add_case("value 2");
  _enum.add_case("value 3");

  // Check that sorting works correctly
  _enum.change_id(0, 10);
  _enum.change_id(2, 0);

  ASSERT(equal(3u, _enum.case_count()));
  std::list<EnumCase> case_list = _enum.get_case_list();
  std::list<EnumCase>::iterator iter = case_list.begin();

  ASSERT(equal(0u, iter->id));
  ASSERT(equal("value 3", iter->value));
  iter++;

  ASSERT(equal(1u, iter->id));
  ASSERT(equal("value 2", iter->value));
  iter++;

  ASSERT(equal(10u, iter->id));
  ASSERT(equal("value 1", iter->value));
}

}
