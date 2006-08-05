/* Driller, a data extraction program
 * Copyright (C) 2005 John Millikin
 * gtk-gui.h

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef DRILLER_GTK_GUI_H
#define DRILLER_GTK_GUI_H

#include <gtk/gtk.h>
#include "base-gui.h"

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

namespace Driller {

class GtkGUI : public BaseGUI {
public:
  GtkGUI(App& driller, Database& db);
  virtual ~GtkGUI();
  void append_entry(Entry& entry);

  void set_filename(const std::string& filename);
  void set_aos(unsigned long aos);
  void set_ros(unsigned long ros);

  void show_error(const std::string& message);

  void extract_to_window(long row_limit);

  void reload_settings();

protected:
  void init();

private:
  /* Toplevel window */
  GtkWindow*        main_window;

  /* Database properties */
  GtkEntry*          db_filename_entry;
  GtkSpinButton*    db_offset_sbutton;
  GtkSpinButton*    db_rowlen_sbutton;

  /* Entry modification buttons */
  GtkButton*        add_entry_button;
  GtkButton*        remove_entry_button;
  GtkButton*        move_entry_up_button;
  GtkButton*        move_entry_down_button;

  /* Toolbutton dialogs */
  GtkDialog*        open_file_dialog;
  GtkDialog*        save_file_dialog;
  GtkDialog*        open_dir_dialog;
  GtkDialog*        extraction_dialog;

  /* Widgets for the extraction dialog */
  GtkRadioButton*    output_text_radio;
  GtkRadioButton*    output_window_radio;

  GtkFrame*          text_frame;
  GtkEntry*          output_text_filename;

  GtkSpinButton*    row_count_sbutton;
  GtkCheckButton*    extract_all_rows_cbutton;

  /* MySQL settings */
  GtkRadioButton*    output_mysql_radio;

  /* PostgreSQL settings */
  GtkRadioButton*   output_postgresql_radio;

  /* General SQL settings */
  GtkFrame*          sql_frame;
  GtkEntry*          sql_server_entry;
  GtkSpinButton*    sql_port_sbutton;
  GtkEntry*          sql_username_entry;
  GtkEntry*          sql_password_entry;
  GtkEntry*          sql_database_entry;
  GtkEntry*          sql_table_entry;

  /* Tree view of entries */
  GtkTreeView*      entry_view;

/* Signal callbacks */

/* Main window */
  static gboolean on_main_window_delete_event(GtkWidget*, GdkEvent*, gpointer);

/* Toolbar buttons */
  static void on_button_new_clicked(GtkWidget* widget, gpointer data);
  static void on_button_open_clicked(GtkWidget* widget, gpointer data);
  static void on_button_save_clicked(GtkWidget* widget, gpointer data);
  static void on_button_open_dir_clicked(GtkWidget* widget, gpointer data);
  static void on_button_extract_clicked(GtkWidget* widget, gpointer data);

/* Entry manipulation buttons */
  static void on_button_add_entry_clicked(GtkWidget* widget, gpointer data);
  static void on_button_remove_entry_clicked(GtkWidget* widget, gpointer data);
  static void on_button_move_entry_up_clicked(GtkWidget* widget, gpointer data);
  static void on_button_move_entry_down_clicked(GtkWidget* widget,
    gpointer data);

/* Callbacks for the entry display treeview */
  static void on_renderer_edited(GtkCellRendererText* renderer, gchar* arga,
    gchar* argb, gpointer data);
  static void on_renderer_toggled(GtkCellRendererToggle* renderer, gchar* path,
    gpointer* data);

/* Database changes take effect immediately */
  int filename_callback_id; // used for changing the filename in reload_settings()
  static void on_db_filename_entry_changed(GtkWidget* widget, gpointer data);
  static void on_db_offset_sbutton_value_changed(GtkWidget* widget, gpointer data);
  static void on_db_rowlen_sbutton_value_changed(GtkWidget* widget, gpointer data);

/* There is a selection in the entry tree view */
  static void on_entry_view_selection_changed(GtkTreeSelection* selection,
  gpointer data);

/* Shows/hides output-specific options */
  static void on_radio_button_clicked(GtkWidget* widget, gpointer data);
};

} // namespace

#endif /* DRILLER_GTK_GUI_H */
