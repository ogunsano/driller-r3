/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * settings_dialog.cpp
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

#include "settings_dialog.h"
#include "../database.h"
#include <QFileDialog>

namespace Driller {

SettingsDialog::SettingsDialog(QWidget* parent):
  QDialog(parent){

  ui.setupUi(this);
  ui.pathEdit->setText(Database::get_data_path().c_str());
}

SettingsDialog::~SettingsDialog(){
}

void SettingsDialog::exec(){
  // Return early if the user cancels the dialog
  if (QDialog::exec() == QDialog::Rejected) return;

  // Set the global data path
  Database::set_data_path(ui.pathEdit->text().toUtf8().constData());
}


void SettingsDialog::on_browseButton_clicked(bool){
  QString dir = QFileDialog::getExistingDirectory(
    this, "Choose a directory", ui.pathEdit->text(), QFileDialog::ShowDirsOnly
  );

  ui.pathEdit->setText(dir);
}

} // namespace
