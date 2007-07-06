SUFFIXES=.ui

## Run UIC on the .ui files
ui_%.h: %.ui
	$(UIC) -o $@ $<

## Run MOC on all the Qt stuff
%.moc.cpp: %.h
	$(MOC) -o $@ $<

QT_FORMS = \
  ExtractedDataWindow.ui \
  MainWindow.ui \
  SettingsDialog.ui

QT_UI = \
  ui_ExtractedDataWindow.h \
  ui_MainWindow.h \
  ui_SettingsDialog.h

BUILT_SOURCES = $(QT_UI)

QT_MOC = \
  custom_delegate.moc.cpp \
  data_extraction_dialog.moc.cpp \
  extracted_data_window.moc.cpp \
  gui.moc.cpp \
  main_window.moc.cpp \
  column_type_editor_creator.moc.cpp \
  column_type_editor.moc.cpp \
  qt_column_type.moc.cpp \
  result_model.moc.cpp \
  settings_dialog.moc.cpp \
  table_model.moc.cpp

clean-local:
	$(RM) $(QT_UI) $(QT_MOC)

noinst_LIBRARIES=libdriller_qt.a
libdriller_qt_a_SOURCES = \
  $(QT_MOC) \
  custom_delegate.cpp \
  data_extraction_dialog.cpp \
  extracted_data_window.cpp \
  gui.cpp \
  main_window.cpp \
  column_type_editor_creator.cpp \
  column_type_editor.cpp \
  qt_column_type.cpp \
  result_model.cpp \
  settings_dialog.cpp \
  table_model.cpp

AM_CXXFLAGS = -DDB_DIR=\"$(pkgdatadir)/driller/Databases\" -DENABLE_BINRELOC
