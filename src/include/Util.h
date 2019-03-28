/**
 * @file Util.h
 * 
 * @author
 * Angelo Elias Dalzotto (150633@upf.br)
 * 
 * @brief A set of utilities to use with C++. Some code are a mix of what was
 * found searching online. The links for reference are with the source code.
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

#ifndef _UTIL_H_
#define _UTIL_H_

#include <vector>
#include <string>

namespace Util {
    class String;
    class System;
    class Python;
};

class Util::String {
public:
    /**
     * @brief Explodes a string into a vector of strings separated by a character.
     * 
     * @param str The string to explode.
     * @param separator The character separator of the string.
     * 
     * @return A vector of strings exploded. Empty if no separator found.
     */
    static const std::vector<std::string> explode(const std::string& str, const char& separator);

    /**
     * @brief Checks if whole string is integer number.
     * 
     * @param str The string to check.
     * 
     * @return True if is integer.
     */
    static bool is_integer(const std::string &str);
};

class Util::System {
public:
    /**
     * @brief Calls an executable. Waits for it to return before continuing execution.
     * 
     * @param *argv[] Set of strings to call a program
     *                First is the program binary
     *                Sequence is arguments
     *                Last needs to be nullptr.
     * 
     * @return -1 if program not found.
     *         Return code of the program executed.
     */
    static int call_and_wait(char *argv[]);
};

class Util::Python {
public:
    /**
     * @brief Calls a Python function from a module.
     * 
     * @param module  The module name (usually the .py file without the extension)
     * @param funct   The function to call.
     * @param argc    The argument count to pass.
     * @param *argv[] The set of arguments to pass to the file.
     *                Usually the first should be the .py file path.
     *                The last does NOT need to be nullptr.
     * 
     * @return 0 if success.
     *         2 if could not import module.
     *         3 if could not import file env.
     *         4 if could not find function.
     *         5 if code execution error. Sets err_msg to last exception raised.
     */
    static int call_funct(std::string module, std::string funct, int argc, char *argv[]);

    /**
     * @brief Gets the last error message set by Python program.
     * 
     * @return The error String.
     */
    static std::string get_last_err() { return err_msg; }

private:
    static std::string err_msg;
};

#endif /* _UTILS_H_ */