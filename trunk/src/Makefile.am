bin_PROGRAMS = driller

if ENABLE_MYSQL
MYSQL_SOURCES=mysql_sink.cpp
endif

if ENABLE_QT_GUI
SUFFIXES=.ui

## Run UIC on the .ui files
.h.ui:
	$(UIC) -o $@ $<

QT_FORMS = \
  qt/DataExtractionDialog.ui \
  qt/ExtractedDataWindow.ui \
  qt/MainWindow.ui \
  qt/SettingsDialog.ui

QT_UI = \
  ui_DataExtractionDialog.h \
  ui_ExtractedDataWindow.h \
  ui_MainWindow.h \
  ui_SettingsDialog.h

ui_DataExtractionDialog.h: qt/DataExtractionDialog.ui
	$(UIC) -o $@ $<

ui_ExtractedDataWindow.h: qt/ExtractedDataWindow.ui
	$(UIC) -o $@ $<

ui_MainWindow.h: qt/MainWindow.ui
	$(UIC) -o $@ $<

ui_SettingsDialog.h: qt/SettingsDialog.ui
	$(UIC) -o $@ $<

## Run MOC on all the Qt stuff
%.moc.cpp: %.h
	$(MOC) -o $@ $<

QT_MOC = \
  qt/custom_delegate.moc.cpp \
  qt/data_extraction_dialog.moc.cpp \
  qt/extracted_data_window.moc.cpp \
  qt/gui.moc.cpp \
  qt/main_window.moc.cpp \
  qt/column_type_editor_creator.moc.cpp \
  qt/column_type_editor.moc.cpp \
  qt/qt_column_type.moc.cpp \
  qt/result_model.moc.cpp \
  qt/settings_dialog.moc.cpp \
  qt/table_model.moc.cpp

QT_SOURCES= \
	$(QT_UI) \
	$(QT_MOC) \
  qt/custom_delegate.cpp \
  qt/data_extraction_dialog.cpp \
  qt/extracted_data_window.cpp \
  qt/gui.cpp \
  qt/main_window.cpp \
  qt/column_type_editor_creator.cpp \
  qt/column_type_editor.cpp \
  qt/qt_column_type.cpp \
  qt/result_model.cpp \
  qt/settings_dialog.cpp \
  qt/table_model.cpp

clean-local:
	$(RM) $(QT_UI) $(QT_MOC)
endif

driller_SOURCES = \
  binreloc.c \
  data_sink.cpp \
  database/block_allocator.cpp \
  database/column.cpp \
  database/enumeration.cpp \
  database/misc.cpp \
  database/result_set.cpp \
  database/serialization.cpp \
  database/table.cpp \
  database.cpp \
  driller.cpp \
  errors.cpp \
  file_errors.cpp \
  file_sink.cpp \
  $(MYSQL_SOURCES) \
  $(QT_SOURCES)

CXXFLAGS = -DDB_DIR=\"$(pkgdatadir)/driller/Databases\" -DENABLE_BINRELOC
