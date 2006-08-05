/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * gui.h
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

#ifndef DRILLER_GUI_H
#define DRILLER_GUI_H

#if ENABLE_GTK_GUI
  #include "gtk-gui.h"
  namespace Driller { typedef GtkGUI GUI; }
#endif

#if ENABLE_QT_GUI
  #include "qt/gui.h"
  namespace Driller { typedef QtGUI GUI; }
#endif

#endif // DRILLER_GUI_H
