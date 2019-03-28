/**
 * @file esptool.h
 * 
 * @author
 * Angelo Elias Dalzotto (150633@upf.br)
 * GEPID - Grupo de Pesquisa em Cultura Digital (gepid.upf.br)
 * Universidade de Passo Fundo (upf.br)
 * 
 * @brief A C++ wrapper for python2 esptool.py.
 * 
 * @copyright
 * Copyright (C) 2014-2016 Fredrik Ahlberg, Angus Gratton, Espressif Systems (Shanghai) PTE LTD, other contributors as noted.
 * https://github.com/espressif/esptool
 * 
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

#ifndef _ESPTOOL_H_
#define _ESPTOOL_H_

#include <string>
#include <cstdint>

class ESPTool {
public:
    /**
     * @brief Equivalent of 'esptool.py read_flash address size filename'
     * 
     * @details Creates an image from ESP flash and saves to filename.
     * 
     * @param address  Start address of image dump
     * @param size     Size of the image dump
     * @param filename Output file name
     * 
     * @return 0 if success
     *         1 if esptool.py not found
     *         Other values refer to Util::Python::call_funct
     */
    static int read_flash(uint32_t address, uint32_t size, std::string filename);

    /**
     * @brief Equivalent of 'esptool.py write_flash address filename'
     * 
     * @details Writes an image to ESP flash.
     * 
     * @param address  Start address to write
     * @param filename Input file name
     * 
     * @return 0 if success
     *         1 if esptool.py not found
     *         Other values refer to Util::Python::call_funct
     */
    static int write_flash(uint32_t address, std::string filename);

    /**
     * @brief Returns the last error message
     * 
     * @return Empty if no error occurred.
     *         Last error message otherwise.
     */
    static std::string get_err_msg() { return err_msg; }

private:
    static std::string err_msg;
    static std::string get_esptool();    
};

#endif /* _ESPTOOL_H_ */