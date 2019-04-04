/**
 * @file main.cpp
 * 
 * @author
 * Angelo Elias Dalzotto (150633@upf.br)
 * GEPID - Grupo de Pesquisa em Cultura Digital (gepid.upf.br)
 * Universidade de Passo Fundo (upf.br)
 * 
 * @brief berimbau-tool is a software to help creating and editing recordings
 * to the "Berimbau" toy.
 * 
 * @copyright
 * Copyright 2019 Angelo Elias Dalzotto
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtkmm/application.h>
#include <glibmm/fileutils.h>
#include <glibmm/markup.h>
#include <gtkmm/builder.h>

#include <Gui.h>
#include <BerimbauTool.h>
#include <Util.h>
#include <iostream>

int main(int argc, char *argv[])
{
    auto app = Gtk::Application::create(argc, argv, "com.github.aedalzotto.berimbau-tool-gui");
    std::string progbin = Util::System::get_program_path(); 
    std::string progpath = progbin.substr(0, progbin.find("bin/berimbau-tool-gui"));
    
    BerimbauTool::set_path(progpath+"lib/berimbau-tool-gui/");

    try {
        int ret = app->run(*Gui::run(progpath));
        BerimbauTool::clean();
        return ret;
    } catch(const Glib::FileError& ex){
        std::cerr << "FileError: " << ex.what() << std::endl;
        return 1;
    } catch(const Glib::MarkupError& ex){
        std::cerr << "MarkupError: " << ex.what() << std::endl;
        return 1;
    } catch(const Gtk::BuilderError& ex){
        std::cerr << "BuilderError: " << ex.what() << std::endl;
        return 1;
    } catch(const std::runtime_error& ex){
        std::cerr << "BuilderError: " << ex.what() << std::endl;
        return 1;
    }

}