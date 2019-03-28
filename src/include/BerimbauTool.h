/**
 * @file BerimbauTool.h
 * 
 * @author
 * Angelo Elias Dalzotto (150633@upf.br)
 * GEPID - Grupo de Pesquisa em Cultura Digital (gepid.upf.br)
 * Universidade de Passo Fundo (upf.br)
 * 
 * @brief Berimbau-tool is a software to manage the recordings of the "Berimbau"
 * toy. This is the header to the tool API.
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

#ifndef _BERIMBAU_TOOL_H_
#define _BERIMBAU_TOOL_H_

#include <string>
#include <vector>

class BerimbauTool {
public:
    /**
     * Instrument options enumerator.
     * Has the same values of the embedded device.
     */
    enum class Instrument {
        CAXIXI,
        MOEDA,
        PRESA,
        SOLTA
    };

    /**
     * Definition of the data structure of the recordings.
     * Has the same values of the embedded device.
     */
    struct beat_t {
        uint32_t d_time;
        uint16_t res0;
        uint8_t instr;
        uint8_t res1;
    };

    /**
     * @brief Creates a recording based on a user .csv file.
     * 
     * @details The .csv must be structure as follows:
     *          # Indicates a commentary that will be ignored
     *          time(ms),[caxixi,moeda,presa,solta]
     *          # Example
     *          0,caxixi
     *          500,solta,
     *          1000,presa
     * 
     * @param fname The input csv file name.
     * 
     * @return  0 if success.
     *          1 if input file not existent.
     *          2 if invalid column number. Sets the line_no to the invalid row.
     *          3 if time column is not a integer. Sets line_no to invalid row and line_value to invalid value.
     *          4 if instrument not recognized. Sets line_no to invalid row and line_value to invalid text.
     */
    static int create(std::string &fname);

    /**
     * @brief Gets the row number of the csv input error.
     *
     * @return The number of the row the error has occurred.
     */
    static unsigned int get_error_line() { return line_no; }

    /**
     * @brief Gets the value that caused the error in the csv file.
     * 
     * @return The string that caused the error.
     */
    static std::string get_error_value() { return line_value; }

    /**
     * @brief Dumps the whole flash SPIFFS partition to ./img/
     * 
     * @return 0 if success
     *         6 if wait for mkspiffs error
     *         0xF0 added to mkspiffs exit code if error in the child process.
     *         255 if no mkspiffs found.
     *         Refer to ESPTool::read_flash(int, char**) for errors.
     */
    static int dump();

    /**
     * @brief Exports .dat rec to img folder.
     * 
     * @param filename The name of the .dat recording.
     * 
     * @return 0 if success.
     *         1 if invalid file name
     *         2 if image not present
     *         3 if could not copy
     */
    static int merge(std::string filename);

    /**
     * @brief Creates a flash image and writes to ESP32 memory.
     * 
     * @return 0 if success.
     *         6 if wait for mkspiffs error
     *         0xF0 added to mkspiffs exit code if error in the child process.
     *         255 if no mkspiffs found.
     *         Refer to ESPTool::read_flash(int, char**) for errors.
     */
    static int flash();

    /**
     * @brief Copies the log dumped from ESP32.
     * 
     * @param path The path to move the log folder to.
     * 
     * @return 0 if success.
     *         1 if no dump available.
     *         2 if no log in the dump.
     *         3 fail to copy.
     */
    static int log(std::string path);

    /**
     * @brief Clean temporary image files.
     * 
     * @return 0 if success.
     *         1 if nothing to clean.
     *         2 if unable to delete.
     */
    static int clean();

private:
    static const uint32_t START_ADDR = 0x210000;
    static const uint32_t PART_SIZE  = 0x1f0000;

    static unsigned int line_no;
    static std::string line_value;
};

#endif /* _BERIMBAU_TOOL_H_ */