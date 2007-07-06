/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * data_extraction_dialog.h
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

#ifndef DRILLER_QT_DATA_EXTRACTION_DIALOG_H
#define DRILLER_QT_DATA_EXTRACTION_DIALOG_H

#include "../database/database.h"
#include <QDialog>

class QCheckBox;
class QSpinBox;
class QComboBox;
class QGroupBox;
class QLineEdit;
class QPushButton;

namespace Driller {

class DataExtractionDialog : public QDialog {
  Q_OBJECT
public:
  DataExtractionDialog(QWidget* parent = NULL);
  ~DataExtractionDialog();

public slots:
  void exec(const Database& db);

protected:
  // Widgets for all extractions

  /** Used to select whether all rows should be extracted */
  QCheckBox* all_rows;

  /** Selects the number of rows to extract */
  QSpinBox* row_count;

  /** Used to select which type of output to show */
  QComboBox* output_selection;

  // Widgets for text extractions

  /** Used to show or hide all text options at once */
  QGroupBox* text_options;

  /** Which directory the extracted data should be stored in */
  QLineEdit* text_output_path;

  /** Button to browse for a data output directory */
  QPushButton* text_output_path_browse;

  // Widgets for MySQL extractions

  /** Used to show or hide all MySQL options at once */
  QGroupBox* mysql_options;

  /** Server host name */
  QLineEdit* mysql_host_name;

  /** Server port */
  QSpinBox* mysql_port;

  /** User name */
  QLineEdit* mysql_user_name;

  /** Password */
  QLineEdit* mysql_password;

  /** Database to store extracted data in */
  QLineEdit* mysql_database_name;

protected slots:
  void on_all_rows_stateChanged(const int state);
  void find_text_output_path();
  void on_output_selection_currentIndexChanged(const int index);
};

} // namespace

#endif // DRILLER_QT_DATA_EXTRACTION_DIALOG_H
