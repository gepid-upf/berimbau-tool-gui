/**
 * @file Gui.h
 * 
 * @author
 * Angelo Elias Dalzotto (150633@upf.br)
 * 
 * @brief The GTK3mm GUI for the Berimbau Tool.
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

#ifndef _GUI_H_
#define _GUI_H_

#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/aboutdialog.h>
#include <gtkmm/textview.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/enums.h>
#include <glibmm/dispatcher.h>

#include <thread>

class Gui {
public:
    static Gtk::Window* run();

    static void notify_end_dump();

private:
    static Gtk::Window *window_main;

    static Gtk::ImageMenuItem *item_about;

    static Gtk::AboutDialog *about_dialog;

    static Gtk::CheckButton *checkbox_loaded;
    static Gtk::Button *button_download;
    static Gtk::Button *button_upload;
    static Gtk::Button *button_create;
    static Gtk::Button *button_export;
    static Gtk::Button *button_logs;

    static Gtk::Dialog *dialog_download;

    static Glib::Dispatcher disp_end_dump;

    static std::thread worker;

    static bool loaded;

    static void on_item_about_activate();
    static void on_about_dialog_response(int response);

    static void on_button_download_clicked();
    static void on_button_create_clicked();

    static void on_notification_end_dump();

    static void create_from(std::string fname);
};

#endif /* _GUI_H_ */