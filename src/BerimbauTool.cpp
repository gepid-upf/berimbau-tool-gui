/**
 * @file BerimbauTool.cpp
 * 
 * @author
 * Angelo Elias Dalzotto (150633@upf.br)
 * GEPID - Grupo de Pesquisa em Cultura Digital (gepid.upf.br)
 * Universidade de Passo Fundo (upf.br)
 * 
 * @brief berimbau-tool API source file.
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

#include <BerimbauTool.h>
#include <Util.h>
#include <esptool.h>

#include <fstream>
#include <sstream>
#include <cstring>

#ifdef _WIN32
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

#ifdef _WIN32
    const std::string BerimbauTool::tmpfolder = "tmp";
#else
    const std::string BerimbauTool::tmpfolder = "/tmp/berimbau";
#endif

std::string BerimbauTool::mkspath;

unsigned int BerimbauTool::line_no = 0;
std::string BerimbauTool::line_value;

int BerimbauTool::create(std::string &fname, std::string fout)
{
    std::vector<beat_t> song;
    song.clear();
    std::ifstream file(fname);

    if(!file.good())
        return 1;   // File not existent

    line_no = 0;
    std::string line;
    beat_t aux = {};
    while(std::getline(file, line)){
        line_no++;
        // Ignore comments starting with '#'
        if(line.c_str()[0] == '#')
            continue;

        std::vector<std::string> col = Util::String::explode(line, ',');
        if(col.size() != 2){
            file.close();
            return 2;    // Invalid column number
        }
        
        if(!Util::String::is_integer(col[0])){
            file.close();
            line_value = col[0];
            return 3;   // First column not integer
        }
        
        Instrument instr;
        if(!col[1].compare("caxixi"))
            instr = Instrument::CAXIXI;
        else if(!col[1].compare("moeda"))
            instr = Instrument::MOEDA;
        else if(!col[1].compare("presa"))
            instr = Instrument::PRESA;
        else if(!col[1].compare("solta"))
            instr = Instrument::SOLTA;
        else {
            file.close();
            line_value = col[1];
            return 4; // Invalid instrument
        }

        aux.d_time = std::stoul(col[0]);
        aux.instr = (uint8_t)instr;

        song.push_back(aux);
    }

    file.close();

    std::ostringstream oss;
    oss << fout;

    if(fout.find(".dat") >= fout.length())
        oss << ".dat";

    std::ofstream rec(oss.str(), std::ofstream::out | std::ofstream::binary);

    for(unsigned int i = 0; i < song.size(); i++)
        rec.write((const char *)&song[i], sizeof(beat_t));
    
    rec.close();

    return 0;
}

int BerimbauTool::dump()
{
    int ret = 0;

    if(!fs::exists(tmpfolder))
        fs::create_directory(tmpfolder);

    if((ret = ESPTool::read_flash(START_ADDR, PART_SIZE, tmpfolder+"/partition.bin")))
        return ret;

    char buffer[10];
    strcpy(buffer, std::to_string(PART_SIZE).c_str());
    char pathbuf[256];
    strcpy(pathbuf, fs::path(mkspath).string().c_str());

    char inbuf[256];
    char outbuf[256];
    strcpy(outbuf, fs::path(tmpfolder+"/img").string().c_str());
    strcpy(inbuf, fs::path(tmpfolder+"/partition.bin").string().c_str());

    char *argv[] = { pathbuf,
                    "-u", outbuf,
                    "-b", "4096",
                    "-p", "256",
                    "-s", buffer,
                    inbuf, nullptr };

    return Util::System::call_and_wait(10, argv);
}

int BerimbauTool::merge(std::string filename)
{
    if(!fs::exists(filename))
        return 1;

    if(!fs::exists(tmpfolder+"/img"))
        return 2;

    if(!fs::exists(tmpfolder+"/img/recs"))
        fs::create_directory(tmpfolder+"/img/recs");

    try {
        Util::System::copy_file_overwrite_workaround(filename, tmpfolder+"/img/recs/"+fs::path(filename).filename().string());
    } catch(std::exception &ex){
        line_value = ex.what();
        return 3;
    }

    return 0;
}

int BerimbauTool::flash()
{
    char buffer[10];
    strcpy(buffer, std::to_string(PART_SIZE).c_str());
    char pathbuf[256];
    strcpy(pathbuf, fs::path(mkspath).string().c_str());
    char outbuf[256];
    char inbuf[256];
    strcpy(outbuf, fs::path(tmpfolder+"/img").string().c_str());
    strcpy(inbuf, fs::path(tmpfolder+"/partition.bin").string().c_str());

    char *argv[] = { pathbuf,
                    "-c", outbuf,
                    "-b", "4096",
                    "-p", "256",
                    "-s", buffer,
                    inbuf, nullptr };

    int ret = 0;
    if((ret = Util::System::call_and_wait(10, argv)))
        return ret;

    return ESPTool::write_flash(START_ADDR, tmpfolder+"/partition.bin");
}

int BerimbauTool::log(std::string path)
{
    if(!fs::exists(tmpfolder+"/img"))
        return 1;

    if(!fs::exists(tmpfolder+"/img/logs"))
        return 2;

    if(!fs::exists(path))
        fs::create_directory(path);

    try {
        Util::System::merge_folders_overwrite_workaround(tmpfolder+"/img/logs", path);
    } catch(std::exception &ex){
        line_value = ex.what();
        return 3;
    }

    return 0;
}

int BerimbauTool::clean()
{
    if(!fs::exists(tmpfolder))
        return 1;

    try {
        fs::remove_all(tmpfolder);
    } catch(...){
        return 2;
    }
}

unsigned int BerimbauTool::get_log_cnt()
{
    if(!fs::exists(tmpfolder+"/img/logs"))
        return 0;

    unsigned int cnt = 0;
    for(auto& p: fs::directory_iterator(tmpfolder+"/img/logs"))
        cnt++;

    return cnt;
}

void BerimbauTool::set_path(std::string path)
{
#ifdef _WIN32
    mkspath = path+"/mkspiffs.exe";
#else
    mkspath = path+"/mkspiffs";
#endif
    ESPTool::set_path(path);
}