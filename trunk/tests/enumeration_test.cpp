#include "test.h"
#include "fixture.h"
#include "database/enumeration.h"

using namespace Driller;

TEST_SUITE(enum_tests)

FIXTURE(enum_fixture)

  void set_up(){
    _enum = Enumeration();
  }

  Enumeration _enum;
};

TEST(constructor)
  Enumeration _enum;
  assert(_enum.case_count()).equals(0);
}

FIXTURE_TEST(add_case, enum_fixture)
  _enum.add_case("case_value");

  assert(_enum.case_count()).equals(1);
  std::list<EnumCase> case_list = _enum.get_case_list();
  std::list<EnumCase>::iterator iter = case_list.begin();
  assert(iter->id).equals(0);
}

FIXTURE_TEST(add_case_2, enum_fixture)
  _enum.add_case(10, "value 1");
  _enum.add_case(20, "value 2");

  assert(_enum.case_count()).equals(2);

  std::list<EnumCase> case_list = _enum.get_case_list();
  std::list<EnumCase>::iterator iter = case_list.begin();

  assert(iter->id).equals(10);
  assert(iter->value).equals("value 1");
  iter++;

  assert(iter->id).equals(20);
  assert(iter->value).equals("value 2");

  // TODO: insert a duplicate ID and check for a DuplicateEnumID exception
}

FIXTURE_TEST(change_id, enum_fixture)
  _enum.add_case("case_value");
  _enum.change_id(0, 1);

  assert(_enum.case_count()).equals(1);
  std::list<EnumCase> case_list = _enum.get_case_list();
  std::list<EnumCase>::iterator iter = case_list.begin();
  assert(iter->id).equals(1);
  assert(iter->value).equals("case_value");
}

FIXTURE_TEST(change_value, enum_fixture)
  _enum.add_case("case_value");
  _enum.change_value(0, "new_value");

  assert(_enum.case_count()).equals(1);
  std::list<EnumCase> case_list = _enum.get_case_list();
  std::list<EnumCase>::iterator iter = case_list.begin();
  assert(iter->id).equals(0);
  assert(iter->value).equals("new_value");
}

FIXTURE_TEST(remove_id, enum_fixture)
  _enum.add_case("case_value");
  _enum.remove_id(0);

  assert(_enum.case_count()).equals(0);
}

FIXTURE_TEST(get_value, enum_fixture)
  _enum.add_case("case_value");
  assert(_enum.get_value(0)).equals("case_value");
}

FIXTURE_TEST(case_count, enum_fixture)
  assert(_enum.case_count()).equals(0);
  _enum.add_case("case_value");
  assert(_enum.case_count()).equals(1);
}

FIXTURE_TEST(get_case_list, enum_fixture)
  _enum.add_case("value 1");
  _enum.add_case("value 2");
  _enum.add_case("value 3");

  // Check that sorting works correctly
  _enum.change_id(0, 10);
  _enum.change_id(2, 0);

  assert(_enum.case_count()).equals(3);
  std::list<EnumCase> case_list = _enum.get_case_list();
  std::list<EnumCase>::iterator iter = case_list.begin();

  assert(iter->id).equals(0);
  assert(iter->value).equals("value 3");
  iter++;

  assert(iter->id).equals(1);
  assert(iter->value).equals("value 2");
  iter++;

  assert(iter->id).equals(10);
  assert(iter->value).equals("value 1");
}

}
