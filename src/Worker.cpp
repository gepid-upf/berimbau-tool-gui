/**
 * @file Worker.cpp
 * 
 * @author
 * Angelo Elias Dalzotto (150633@upf.br)
 * 
 * @brief Threads for calling the Berimbau Tool and updating the GUI
 * 
 * @copyright
 * Copyright 2019 Angelo Elias Dalzotto
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <Worker.h>
#include <BerimbauTool.h>
#include <Gui.h>

int Worker::status = 0;

// https://mail.gnome.org/archives/gtk-list/2006-February/msg00040.html
void Worker::run_flash()
{
	status = BerimbauTool::dump();
	Gui::notify_end_dump();
}