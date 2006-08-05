/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * main_window.cpp
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

#include "data_extraction_dialog.h"
#include "settings_dialog.h"
#include "main_window.h"
#include "column_type_editor_creator.h"
#include "column_type_editor.h"

#include <sstream>
#include <errno.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QItemDelegate>

namespace Driller {

MainWindow::MainWindow():
  current_directory("Databases"),
  extraction_dialog(NULL){
  ui.setupUi(this);

  ui.tableList->setModel(&tableListModel);
  ui.columnList->setModel(&columnListModel);

  tableListModel.insertColumns(0, 1);

  // Create a single empty table by default
  db.add_table(Table());
  columnListModel.set_table(current_table());
  refresh_tables();

  item_factory = new QItemEditorFactory;
  column_type_editor_creator = new ColumnTypeEditorCreator;
  item_factory->registerEditor(QVariant::UserType, column_type_editor_creator);
  static_cast<QItemDelegate*>(ui.columnList->itemDelegate())->
    setItemEditorFactory(item_factory);

  connect(
    ui.tableList->selectionModel(),
    SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
    this, SLOT(refresh_current_table())
  );

  connect (
    ui.columnList->selectionModel(),
    SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
    this, SLOT(columnList_currentChanged())
  );

  QCoreApplication::setOrganizationName("Apollonia Dental Centers");
  QCoreApplication::setApplicationName("Driller");
}

MainWindow::~MainWindow(){
  delete item_factory;
  delete column_type_editor_creator;
}

void MainWindow::show_error(const Errors::BaseError& error){
  show_error(error.error_message().c_str());
}

Table* MainWindow::current_table(){
  if (db.table_count() > 0){
    QModelIndex index = ui.tableList->selectionModel()->currentIndex();

    if (index.isValid()){
      return &(db.table_at(index.row()));
    }

    else {
      return &(db.table_at(0));
    }
  }

  else {
    return NULL;
  }
}

void MainWindow::refresh_all(){
  ui.databaseName->setText(db.get_name().c_str());
  refresh_window_title();
  refresh_tables();

  // Select the first table
  select_table(tableListModel.index(0,0));

  refresh_current_table();
}

void MainWindow::refresh_tables(){
  tableListModel.clear();

  tableListModel.insertColumns(0, 1);

  std::vector<Table> tables = db.get_tables();
  std::vector<Table>::reverse_iterator table;
  unsigned int row = 0;
  for (table = tables.rbegin(); table != tables.rend(); table++){
    tableListModel.insertRows(0, 1);

    tableListModel.setData(tableListModel.index(0, 0),
      table->get_name().c_str(), Qt::DisplayRole);
    ++row;
  }
}

void MainWindow::refresh_current_table(){
  Table* table = current_table();

  if (!table){
    return;
  }

  else {
    // Enable table property editing
    ui.tableName->setEnabled(true);
    ui.tableFileName->setEnabled(true);
    ui.tableDataOffset->setEnabled(true);
    ui.tableRowLength->setEnabled(true);
  }

  // "Friendly" table name
  ui.tableName->setText(table->get_name().c_str());

  // File name
  ui.tableFileName->setText(table->get_file_name().c_str());

  // Data offset
  ui.tableDataOffset->setValue(table->get_data_offset());

  // Row length
  ui.tableRowLength->setValue(table->get_row_length());

  columnListModel.set_table(current_table());

  ui.columnList->resizeColumnToContents(0);
  ui.columnList->resizeColumnToContents(1);

  refresh_item_buttons();
}

void MainWindow::refresh_window_title(){
  QString db_name = db.get_name().c_str();

  if (db_name.isEmpty()){
    setWindowTitle("Driller");
  }

  else {
    setWindowTitle("Driller - " + db_name);
  }
}

void MainWindow::save_to_file(QString filename) const {
  // User entered a name not ending with ".xml"
  if (!filename.contains(".xml", Qt::CaseInsensitive)){
    filename.append(".xml");
  }

  // Open whatever Qt can support
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)){
    throw Errors::FileWriteError(filename.toUtf8().constData(), errno);
  }

  // Since the Database class uses standard C++ I/O, use a string buffer
  // as an inbetween between it and QFile
  std::stringstream ss;
  ss << db;
  file.write(ss.str().c_str());
  file.close();
}

void MainWindow::show_error(const QString& error){
  QMessageBox::warning(this, "Error", error,
    QMessageBox::Cancel, QMessageBox::NoButton);
}

void MainWindow::refresh_item_buttons(){
  QModelIndex index = ui.columnList->selectionModel()->currentIndex();

  if (!index.isValid()){
    ui.removeItemButton->setEnabled(false);
    ui.addItemButton->setText("Add Column");
    ui.removeItemButton->setText("Select an item");
    return;
  }

  ui.removeItemButton->setEnabled(true);

  // Enumeration case
  if (columnListModel.is_enum_case(index)){
    ui.addItemButton->setText("Add Case");
    ui.removeItemButton->setText("Remove Case");
  }

  // Normal column
  else {
    ui.removeItemButton->setText("Remove Column");
  }
}

void MainWindow::select_table(const QModelIndex& index){
  QItemSelectionModel* selection = ui.tableList->selectionModel();
  selection->select(index,
    QItemSelectionModel::Rows | QItemSelectionModel::SelectCurrent);

  selection->setCurrentIndex(index,
    QItemSelectionModel::Rows | QItemSelectionModel::SelectCurrent);
}

void MainWindow::on_actionQuit_activated(){
  qApp->quit();
}

void MainWindow::on_actionOpen_activated(){
  QFileDialog* dialog = new QFileDialog(this);

  // Only open existing files
  dialog->setFileMode(QFileDialog::ExistingFile);

  // Open mode
  dialog->setAcceptMode(QFileDialog::AcceptOpen);

  // Set directory to the current directory
  dialog->setDirectory(current_directory);

  // Only open .xml files
  dialog->setFilter("Database files (*.xml)");

  // If the user has selected a name before, use it as a default
  if (!current_filename.isEmpty()){
    dialog->selectFile(current_filename);
  }

  // Actually execute the dialog, to get the file name
  if (dialog->exec()){
    // Save the current directory
    current_directory = dialog->directory();

    QString file = dialog->selectedFiles().at(0);

    try {
      current_filename = file;
      db.load(file.toUtf8().constData());
      refresh_all();
    }

    catch (const Errors::BaseError& error) {
      show_error(error);
    }
  }
}

void MainWindow::on_actionSave_activated(){
  if (current_filename.isEmpty()){
    // Open a Save As dialog if no filename has been selected yet
    on_actionSaveAs_activated();
  }
  else {
    save_to_file(current_filename);
  }
}

void MainWindow::on_actionSaveAs_activated(){
  QFileDialog* dialog = new QFileDialog(this);

  // Accept any file
  dialog->setFileMode(QFileDialog::AnyFile);

  // Save mode
  dialog->setAcceptMode(QFileDialog::AcceptSave);

  // Confirm overwriting existing files
  dialog->setConfirmOverwrite(true);

  // Set directory to the current directory
  dialog->setDirectory(current_directory);

  // Only save .xml files
  dialog->setFilter("Database files (*.xml)");

  // If the user has not selected a name before, default to the name of the
  // database
  if (current_filename.isEmpty()){
    // By default, select the current database name + ".xml"
    // If the database name is empty, use "unnamed.xml"
    if (db.get_name().empty()){
      dialog->selectFile("unnamed.xml");
    }

    else {
      dialog->selectFile(QString(db.get_name().c_str()) + ".xml");
    }
  }

  else {
    dialog->selectFile(current_filename);
  }

  // Actually execute the dialog, to get the file name
  if (dialog->exec()){
    // Save the current directory
    current_directory = dialog->directory();

    QString file = dialog->selectedFiles().at(0);

    try {
      current_filename = file;
      save_to_file(file);
    }

    catch (const Errors::BaseError& error) {
      show_error(error);
    }
  }
}

void MainWindow::on_actionExtract_Data_activated(){
  // If the extraction dialog has not yet been created, do so now
  if (!extraction_dialog){
    extraction_dialog = new DataExtractionDialog(this);
  }

  extraction_dialog->set_database(db);

  try {
    extraction_dialog->exec();
  } catch (const Errors::BaseError& error) {
    show_error(error);
  }
}

void MainWindow::on_actionUndo_activated(){
  QWidget* widget = QApplication::focusWidget();

  if (!widget){
    return;
  }

  // FIXME
  // I have a gut feeling that this is a horribly broken way to implement undo
  // The above also applies for other Edit menu actions
  QMetaObject::invokeMethod(widget, "undo", Qt::DirectConnection);
}

void MainWindow::on_actionRedo_activated(){
  QWidget* widget = QApplication::focusWidget();

  if (!widget){
    return;
  }

  QMetaObject::invokeMethod(widget, "redo", Qt::DirectConnection);
}

void MainWindow::on_actionCut_activated(){
  QWidget* widget = QApplication::focusWidget();

  if (!widget){
    return;
  }

  QMetaObject::invokeMethod(widget, "cut", Qt::DirectConnection);
}

void MainWindow::on_actionCopy_activated(){
  QWidget* widget = QApplication::focusWidget();

  if (!widget){
    return;
  }

  QMetaObject::invokeMethod(widget, "copy", Qt::DirectConnection);
}

void MainWindow::on_actionPaste_activated(){
  QWidget* widget = QApplication::focusWidget();

  if (!widget){
    return;
  }

  QMetaObject::invokeMethod(widget, "paste", Qt::DirectConnection);
}

void MainWindow::on_actionDelete_activated(){
  QWidget* widget = QApplication::focusWidget();

  if (!widget){
    return;
  }

  QMetaObject::invokeMethod(widget, "delete", Qt::DirectConnection);
}

void MainWindow::on_actionSelect_All_activated(){
  QWidget* widget = QApplication::focusWidget();

  if (!widget){
    return;
  }

  QMetaObject::invokeMethod(widget, "selectAll", Qt::DirectConnection);
}

void MainWindow::on_actionSettings_activated(){
  // Open the Settings dialog
  SettingsDialog* dialog = new SettingsDialog(this);

  dialog->exec();

  delete dialog;
}

void MainWindow::on_addItemButton_clicked(bool){
  QModelIndex index = ui.columnList->selectionModel()->currentIndex();

  if (!index.isValid()){
    return;
  }

  // Enumeration case
  if (columnListModel.is_enum_case(index)){
    columnListModel.insertRow(0, columnListModel.parent(index));
  }

  // Normal column
  else {
    columnListModel.insertRow(0);
  } 
}

void MainWindow::on_removeItemButton_clicked(bool){
  QModelIndex index = ui.columnList->selectionModel()->currentIndex();

  if (!index.isValid()){
    return;
  }

  // Enumeration case
  if (columnListModel.is_enum_case(index)){
    columnListModel.removeRow(index.row(), columnListModel.parent(index));
  }

  // Normal column
  else {
    columnListModel.removeRow(index.row());
  } 

  refresh_item_buttons();
}

void MainWindow::on_addTableButton_clicked(bool){
  // Insert the new table
  db.add_table(Table("New Table"));

  // Refresh the table list
  refresh_tables();

  // Select the table
  select_table(tableListModel.index(tableListModel.rowCount()-1, 0));
}

void MainWindow::on_removeTableButton_clicked(bool){
  const QModelIndex& index =
    ui.tableList->selectionModel()->currentIndex();

  if (index.isValid()){
    db.remove_table(index.row());
  }

  refresh_tables();
}

void MainWindow::on_databaseName_textEdited(const QString& new_name){
  db.set_name(new_name.toUtf8().constData());

  // Update the window title
  refresh_window_title();
}

void MainWindow::on_tableName_textEdited(const QString& text){
  Table* table = current_table();
  if (!table){
    return;
  }

  table->set_name(text.toUtf8().constData());
  QModelIndex index = ui.tableList->selectionModel()->currentIndex();

  if (index.isValid()){
    if (!tableListModel.setData(index, text, Qt::DisplayRole)){
      show_error("couldn't set data");
    }
  }
}

void MainWindow::on_tableFileName_textEdited(const QString& text){
  Table* table = current_table();
  if (table){
    table->set_file_name(text.toUtf8().constData());
  }
}

void MainWindow::on_tableDataOffset_valueChanged(int value){
  Table* table = current_table();
  if (table){
    table->set_data_offset(value);
  }
}

void MainWindow::on_tableRowLength_valueChanged(int value){
  Table* table = current_table();
  if (table){
    table->set_row_length(value);
  }
}

void MainWindow::columnList_currentChanged(){
  refresh_item_buttons();
}

} // namespace
