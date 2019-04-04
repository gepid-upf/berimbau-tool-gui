/**
 * @file Worker.h
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

#ifndef _WORKER_H_
#define _WORKER_H_

#include <gtkmm/textview.h>

class Worker {
public:
	static void run_dump();
	static void run_flash();
	static int get_status() { return status; }

private:
	static int status;
};

#endif /* _WORKER_H_ */