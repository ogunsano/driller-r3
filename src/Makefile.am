SUBDIRS=database

if ENABLE_MYSQL
MYSQL_SOURCES=mysql_sink.cpp
endif

bin_PROGRAMS = driller
driller_LDADD = database/libdriller_database.a
driller_SOURCES= \
  binreloc.c \
  data_sink.cpp \
  driller.cpp \
  errors.cpp \
  file_errors.cpp \
  file_sink.cpp \
  $(MYSQL_SOURCES)

if ENABLE_QT_GUI
SUBDIRS += qt
driller_LDADD += qt/libdriller_qt.a
endif

AM_CXXFLAGS = -DDB_DIR=\"$(pkgdatadir)/driller/Databases\" -DENABLE_BINRELOC
