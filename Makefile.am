SUBDIRS = src Databases

# Used for 'make check'
check_PROGRAMS=check_driller
check_driller_SOURCES=\
  tests/main.cpp \
  tests/column_test.cpp \
  tests/enumeration_test.cpp \
  tests/serialization_test.cpp \
  tests/database_test.cpp \
  tests/misc_test.cpp \
  tests/table_test.cpp

check_driller_LDADD = \
  src/binreloc.o \
  src/data_sink.o \
  src/errors.o \
  src/file_errors.o \
  src/file_sink.o \
  src/database/libdriller_database.a

check_driller_CXXFLAGS = `pkg-config --cflags libcu`
check_driller_LDFLAGS = `pkg-config --libs libcu`

check:
	./check_driller
