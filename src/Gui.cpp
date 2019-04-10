/**
 * @file Gui.cpp
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

#include <Gui.h>
#include <Util.h>
#include <Worker.h>
#include <BerimbauTool.h>

#include <gtkmm/builder.h>

Gtk::Window *Gui::window_main = nullptr;

Gtk::ImageMenuItem *Gui::item_about = nullptr;

Gtk::AboutDialog *Gui::about_dialog = nullptr;

Gtk::CheckButton *Gui::checkbox_loaded = nullptr;
Gtk::Button *Gui::button_download = nullptr;
Gtk::Button *Gui::button_upload = nullptr;
Gtk::Button *Gui::button_create = nullptr;
Gtk::Button *Gui::button_export = nullptr;
Gtk::Button *Gui::button_logs = nullptr;

Gtk::Label *Gui::label_available = nullptr;
Gtk::Label *Gui::label_logs = nullptr;

Gtk::TextView *Gui::text_recs = nullptr;
Glib::RefPtr<Gtk::TextBuffer> Gui::txtbuf;

Gtk::Dialog *Gui::dialog_download = nullptr;
Gtk::Dialog *Gui::dialog_upload = nullptr;

std::thread Gui::worker;
Glib::Dispatcher Gui::disp_end_dump;
Glib::Dispatcher Gui::disp_end_flash;

bool Gui::loaded = false;
unsigned int Gui::logcnt = 0;

#ifdef _WIN32
    namespace fs = std::experimental::filesystem;
#else
    namespace fs = std::filesystem;
#endif

Gtk::Window* Gui::run(std::string &path)
{
    Glib::RefPtr<Gtk::Builder> builder;

    //Open glade file
    try {
        builder = Gtk::Builder::create_from_file(fs::path(path+"/share/berimbau-tool-gui/gui.glade").string());
    } catch(...){
        throw;
    }
    
    //Get pointer to widgets of the main window
    builder->get_widget("window_main", window_main);
    if(!window_main)
        throw std::runtime_error("Unable to access window_main widget");
    
    builder->get_widget("button_download", button_download);
    if(!button_download)
        throw std::runtime_error("Unable to access button_download widget");

    builder->get_widget("button_upload", button_upload);
    if(!button_upload)
        throw std::runtime_error("Unable to access button_upload widget");

    builder->get_widget("button_create", button_create);
    if(!button_create)
        throw std::runtime_error("Unable to access button_create widget");

    builder->get_widget("button_export", button_export);
    if(!button_export)
        throw std::runtime_error("Unable to access button_export widget");

    builder->get_widget("button_logs", button_logs);
    if(!button_logs)
        throw std::runtime_error("Unable to access button_logs widget");

    builder->get_widget("item_about", item_about);
    if(!item_about)
        throw std::runtime_error("Unable to access item_about widget");

    builder->get_widget("about_dialog", about_dialog);
    if(!about_dialog)
        throw std::runtime_error("Unable to access about_dialog widget");

    builder->get_widget("dialog_download", dialog_download);
    if(!dialog_download)
        throw std::runtime_error("Unable to access dialog_download widget");

    builder->get_widget("checkbox_loaded", checkbox_loaded);
    if(!checkbox_loaded)
        throw std::runtime_error("Unable to access checkbox_loaded widget");

    builder->get_widget("label_available", label_available);
    if(!label_available)
        throw std::runtime_error("Unable to access label_available widget");

    builder->get_widget("text_recs", text_recs);
    if(!text_recs)
        throw std::runtime_error("Unable to access text_recs widget");

    builder->get_widget("dialog_upload", dialog_upload);
    if(!dialog_upload)
        throw std::runtime_error("Unable to access dialog_upload widget");

    builder->get_widget("label_logs", label_logs);
    if(!label_logs)
        throw std::runtime_error("Unable to access label_logs widget");

    txtbuf = Gtk::TextBuffer::create();
    text_recs->set_buffer(txtbuf);

    //Signals of the main window
    item_about->signal_activate().connect(&Gui::on_item_about_activate);
    about_dialog->signal_response().connect(&Gui::on_about_dialog_response);

    button_download->signal_clicked().connect(&Gui::on_button_download_clicked);
    button_create->signal_clicked().connect(&Gui::on_button_create_clicked);
    button_logs->signal_clicked().connect(&Gui::on_button_logs_clicked);
    button_export->signal_clicked().connect(&Gui::on_button_export_clicked);
    button_upload->signal_clicked().connect(&Gui::on_button_upload_clicked);

    disp_end_dump.connect(&Gui::on_notification_end_dump);
    disp_end_flash.connect(&Gui::on_notification_end_flash);

    return window_main;
}

void Gui::on_item_about_activate()
{
    about_dialog->show_all();
}

void Gui::on_about_dialog_response(int response)
{
    about_dialog->hide();
}

void Gui::on_button_download_clicked()
{
    worker = std::thread(&Worker::run_dump);
    dialog_download->show_all();
}

void Gui::on_notification_end_dump()
{
    worker.join();
    dialog_download->hide();
    int st = Worker::get_status();
    Gtk::MessageDialog *dialog;
    switch(st){
    case 0:
        dialog = new Gtk::MessageDialog(*window_main, "Sucesso", false, Gtk::MESSAGE_INFO);
        dialog->set_secondary_text("Imagem recuperada com sucesso");
        dialog->run();
        checkbox_loaded->activate();
        button_export->set_sensitive();
        
        loaded = true;
        logcnt = BerimbauTool::get_log_cnt();
        label_available->set_text(std::to_string(logcnt));
        if(logcnt){
            button_logs->set_sensitive();
            label_logs->set_sensitive();
        } 
        break;
    case 1:
        dialog = new Gtk::MessageDialog(*window_main, "Erro no esptool", false, Gtk::MESSAGE_ERROR);
        dialog->set_secondary_text("esptool.py não encontrado");
        dialog->run();
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        dialog = new Gtk::MessageDialog(*window_main, "Erro no esptool", false, Gtk::MESSAGE_ERROR);
        dialog->set_secondary_text(Util::Python::get_last_err());
        dialog->run();
        break;
    default:
        dialog = new Gtk::MessageDialog(*window_main, "Erro no mkspiffs", false, Gtk::MESSAGE_ERROR);
        dialog->run();
        break;
    }

    delete dialog;
}

void Gui::notify_end_dump()
{
    disp_end_dump.emit();
}

void Gui::on_button_create_clicked()
{
    Gtk::FileChooserDialog diag_in(*window_main, "Selecione a descrição do ritmo",
                                    Gtk::FILE_CHOOSER_ACTION_OPEN, Gtk::DIALOG_MODAL | Gtk::DIALOG_DESTROY_WITH_PARENT);

    auto filter = Gtk::FileFilter::create();
    filter->set_name("Arquivos .csv");
#ifdef _WIN32
    filter->add_pattern("*.csv");
#else
    filter->add_mime_type("text/csv");
#endif

    diag_in.add_filter(filter);
    diag_in.add_button("Cancelar", Gtk::RESPONSE_CANCEL);
    diag_in.add_button("Abrir", Gtk::RESPONSE_OK);

    switch(diag_in.run()){
    case Gtk::RESPONSE_OK:
        diag_in.hide();
        create_from(diag_in.get_filename());
        break;
    case Gtk::RESPONSE_CANCEL:
        break;
    }
    
}

void Gui::create_from(std::string fname)
{
    Gtk::FileChooserDialog diag_out(*window_main, "Selecione o arquivo de saída",
        Gtk::FILE_CHOOSER_ACTION_SAVE, Gtk::DIALOG_MODAL | Gtk::DIALOG_DESTROY_WITH_PARENT);

    auto filter = Gtk::FileFilter::create();
    filter->set_name("Arquivos .dat");
#ifdef _WIN32
    filter->add_pattern("*.dat");
#else
    filter->add_mime_type("application/octet-stream");
#endif

    diag_out.add_filter(filter);
    diag_out.add_button("Cancelar", Gtk::RESPONSE_CANCEL);
    diag_out.add_button("Selecionar", Gtk::RESPONSE_OK);

    Gtk::MessageDialog *dialog;
    switch(diag_out.run()){
    case Gtk::RESPONSE_OK:
        diag_out.hide();
        switch(BerimbauTool::create(fname, diag_out.get_filename())){
        case 0:
            dialog = new Gtk::MessageDialog(*window_main, "Sucesso", false, Gtk::MESSAGE_INFO);
            dialog->set_secondary_text("Arquivo de gravação binário criado");
            dialog->run();
            break;
        case 2:
            dialog = new Gtk::MessageDialog(*window_main, "ERRO", false, Gtk::MESSAGE_ERROR);
            dialog->set_secondary_text("Número de colunas inválido na linha "+std::to_string(BerimbauTool::get_error_line()));
            dialog->run();
            break;
        case 3:
            dialog = new Gtk::MessageDialog(*window_main, "ERRO", false, Gtk::MESSAGE_ERROR);
            dialog->set_secondary_text("Tempo inválido "+BerimbauTool::get_error_value()+" na linha "+std::to_string(BerimbauTool::get_error_line()));
            dialog->run();
            break;
        case 4:
            dialog = new Gtk::MessageDialog(*window_main, "ERRO", false, Gtk::MESSAGE_ERROR);
            dialog->set_secondary_text("Instrumento inválido "+BerimbauTool::get_error_value()+" na linha "+std::to_string(BerimbauTool::get_error_line()));
            dialog->run();
            break;
        }
    case Gtk::RESPONSE_CANCEL:
        break;
    }

    delete dialog;
}

void Gui::on_button_logs_clicked()
{
    Gtk::FileChooserDialog diag_out(*window_main, "Selecione a pasta onde os logs serão salvos",
        Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER, Gtk::DIALOG_MODAL | Gtk::DIALOG_DESTROY_WITH_PARENT);

    diag_out.add_button("Cancelar", Gtk::RESPONSE_CANCEL);
    diag_out.add_button("Selecionar", Gtk::RESPONSE_OK);

    Gtk::MessageDialog *dialog;
    switch(diag_out.run()){
    case Gtk::RESPONSE_OK:
        diag_out.hide();
        switch(BerimbauTool::log(diag_out.get_filename())){
        case 0:
            dialog = new Gtk::MessageDialog(*window_main, "Sucesso", false, Gtk::MESSAGE_INFO);
            dialog->set_secondary_text("Pasta de logs copiada para o destino");
            dialog->run();
            break;
        case 3:
            dialog = new Gtk::MessageDialog(*window_main, "ERRO", false, Gtk::MESSAGE_ERROR);
            dialog->set_secondary_text(BerimbauTool::get_error_value());
            dialog->run();
            break;
        }
        break;
    case Gtk::RESPONSE_CANCEL:
        break;
    }

    delete dialog;
}

void Gui::on_button_export_clicked()
{
    Gtk::FileChooserDialog diag_in(*window_main, "Selecione o ritmo em arquivo binário",
                                    Gtk::FILE_CHOOSER_ACTION_OPEN, Gtk::DIALOG_MODAL | Gtk::DIALOG_DESTROY_WITH_PARENT);

    auto filter = Gtk::FileFilter::create();
    filter->set_name("Arquivos .dat");
#ifdef _WIN32
    filter->add_pattern("*.dat");
#else
    filter->add_mime_type("application/octet-stream");
#endif

    diag_in.add_filter(filter);
    diag_in.add_button("Cancelar", Gtk::RESPONSE_CANCEL);
    diag_in.add_button("Abrir", Gtk::RESPONSE_OK);

    Gtk::MessageDialog *dialog;
    switch(diag_in.run()){
    case Gtk::RESPONSE_OK:
        diag_in.hide();
        switch(BerimbauTool::merge(diag_in.get_filename())){
        case 0:
            dialog = new Gtk::MessageDialog(*window_main, "Sucesso", false, Gtk::MESSAGE_INFO);
            dialog->set_secondary_text("Gravação adicionada à imagem");
            dialog->run();
            add_rec(diag_in.get_filename());
            button_upload->set_sensitive();
            break;
        case 3:
            dialog = new Gtk::MessageDialog(*window_main, "ERRO", false, Gtk::MESSAGE_ERROR);
            dialog->set_secondary_text(BerimbauTool::get_error_value());
            dialog->run();
            break;
        }
        break;
    case Gtk::RESPONSE_CANCEL:
        break;
    }

    delete dialog;
}

void Gui::add_rec(std::string name)
{
    name = name.substr(name.find_last_of(SP)+1, name.find(".dat"));
    txtbuf->set_text(txtbuf->get_text() + name + "\n");
}

void Gui::on_button_upload_clicked()
{
    worker = std::thread(&Worker::run_flash);
    dialog_upload->show_all();
}

void Gui::notify_end_flash()
{
    disp_end_flash.emit();
}

void Gui::on_notification_end_flash()
{
    worker.join();
    dialog_upload->hide();
    int st = Worker::get_status();
    Gtk::MessageDialog *dialog;
    switch(st){
    case 0:
        dialog = new Gtk::MessageDialog(*window_main, "Sucesso", false, Gtk::MESSAGE_INFO);
        dialog->set_secondary_text("Imagem gravada com sucesso");
        dialog->run();
        break;
    case 1:
        dialog = new Gtk::MessageDialog(*window_main, "Erro no esptool", false, Gtk::MESSAGE_ERROR);
        dialog->set_secondary_text("esptool.py não encontrado");
        dialog->run();
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        dialog = new Gtk::MessageDialog(*window_main, "Erro no esptool", false, Gtk::MESSAGE_ERROR);
        dialog->set_secondary_text(Util::Python::get_last_err());
        dialog->run();
        break;
    default:
        dialog = new Gtk::MessageDialog(*window_main, "Erro no mkspiffs", false, Gtk::MESSAGE_ERROR);
        dialog->run();
        break;
    }

    delete dialog;
}