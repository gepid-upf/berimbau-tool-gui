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
#include <gtkmm/label.h>
#include <gtkmm/enums.h>
#include <glibmm/dispatcher.h>

#include <glibmm/fileutils.h>
#include <iostream>

#include <thread>

class Gui {
public:
    static Gtk::Window* run();

    static void notify_end_dump();
    static void notify_end_flash();

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

    static Gtk::Label *label_available;
    static Gtk::Label *label_logs;

    static Gtk::TextView *text_recs;
    static Glib::RefPtr<Gtk::TextBuffer> txtbuf;

    static Gtk::Dialog *dialog_download;

    static Gtk::Dialog *dialog_upload;

    static Glib::Dispatcher disp_end_dump;
    static Glib::Dispatcher disp_end_flash;

    static std::thread worker;

    static bool loaded;
    static unsigned int logcnt;

    static void on_item_about_activate();
    static void on_about_dialog_response(int response);

    static void on_button_download_clicked();
    static void on_button_create_clicked();
    static void on_button_logs_clicked();
    static void on_button_export_clicked();
    static void on_button_upload_clicked();

    static void on_notification_end_dump();
    static void on_notification_end_flash();

    static void create_from(std::string fname);

    static void add_rec(std::string name);

};

#endif /* _GUI_H_ */