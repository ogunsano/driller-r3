/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * main_window.h
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

#ifndef DRILLER_QT_MAIN_WINDOW_H
#define DRILLER_QT_MAIN_WINDOW_H

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <QApplication>
#include <QStandardItemModel>
#include "table_model.h"
#include "ui_MainWindow.h"
#include "../database/database.h"
#include <QDir>

class QItemEditorFactory;
class ColumnTypeEditorCreator;

namespace Driller {

class DataExtractionDialog;
class ColumnTypeEditorCreator;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow();
  ~MainWindow();

  void show_error(const Errors::BaseError& error);

protected:
  Table* current_table();

  void refresh_all();
  void refresh_tables();
  void refresh_window_title();

  void save_to_file(QString filename) const;

  void show_error(const QString& string);

  void refresh_item_buttons();

  void select_table(const QModelIndex& index);

  Ui::MainWindow ui;
  Database db;

  QStandardItemModel tableListModel;
  TableModel columnListModel;

  QItemEditorFactory* item_factory;
  ColumnTypeEditorCreator* column_type_editor_creator;

  QString current_filename;
  QDir current_directory;

  DataExtractionDialog* extraction_dialog;

protected slots:

  void on_actionQuit_activated();

  void on_actionOpen_activated();
  void on_actionSave_activated();
  void on_actionSaveAs_activated();
  void on_actionExtract_Data_activated();

  void on_actionUndo_activated();
  void on_actionRedo_activated();
  void on_actionCut_activated();
  void on_actionCopy_activated();
  void on_actionPaste_activated();
  void on_actionDelete_activated();
  void on_actionSelect_All_activated();
  void on_actionSettings_activated();

  void on_addItemButton_clicked(bool checked);
  void on_removeItemButton_clicked(bool checked);

  void on_addTableButton_clicked(bool checked);
  void on_removeTableButton_clicked(bool checked);

  void on_databaseName_textEdited(const QString& text);

  void on_tableName_textEdited(const QString& text);
  void on_tableFileName_textEdited(const QString& text);
  void on_tableDataOffset_valueChanged(int i);
  void on_tableRowLength_valueChanged(int i);

  void columnList_currentChanged();
  void refresh_current_table();
};

} // namespace

#endif // DRILLER_QT_MAIN_WINDOW_H
