/**
 * @file Util.cpp
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

#include <Util.h>
#include <unistd.h>
#include <sys/wait.h>

#include <Python.h>

// http://www.cplusplus.com/articles/2wA0RXSz/
const std::vector<std::string> Util::String::explode(const std::string& str, const char& separator)
{
    std::string buff{""};
    std::vector<std::string> v;

    for(auto n:str)
    {
        if(n != separator) buff+=n; else
        if(n == separator && buff != "") { v.push_back(buff); buff = ""; }
    }
    if(buff != "") v.push_back(buff);

    return v;
}

// https://stackoverflow.com/questions/2844817/how-do-i-check-if-a-c-string-is-an-int/2845275#2845275
bool Util::String::is_integer(const std::string &str)
{
    if(str.empty() || ((!isdigit(str[0])) && (str[0] != '-') && (str[0] != '+')))
        return false;

    char * p;
    strtol(str.c_str(), &p, 10);

    return (*p == 0);
}

int Util::System::call_and_wait(char *argv[])
{
    int pid = fork();
    if(!pid){
        exit(execvp(argv[0], argv));
    } else {
        int status;
        pid = waitpid(pid, &status, 0);

        if(pid < 0)
            return pid; // Wait error
       
        return WEXITSTATUS(status);
    }
}

std::string Util::Python::err_msg;

int Util::Python::call_funct(std::string module, std::string funct, int argc, char *argv[])
{
    Py_Initialize();

    PySys_SetArgv(argc, argv); // Sets esptool.py path and args

    // Import whole esptool.py
    PyObject *src = PyString_FromString(module.c_str());
    PyObject *mod = PyImport_Import(src);
    if(!mod){
        PyObject *type, *value, *tb;
        PyErr_Fetch(&type, &value, &tb);
        err_msg = PyString_AS_STRING(value);
        return 2;
    }

    // Get context from module
    PyObject *dict = PyModule_GetDict(mod);
    if(!dict){
        PyObject *type, *value, *tb;
        PyErr_Fetch(&type, &value, &tb);
        err_msg = PyString_AS_STRING(value);
        return 3;
    }

    // Get function from context
    PyObject *func = PyDict_GetItemString(dict, funct.c_str());

    if (!PyCallable_Check(func)){
        err_msg = "Impossível chamar função";
        return 4;
    }

    PyObject_CallObject(func, NULL);

    if(PyErr_Occurred()){
        PyObject *type, *value, *tb;
        PyErr_Fetch(&type, &value, &tb);
        err_msg = PyString_AS_STRING(PyObject_Str(value));
        return 5;
    }

    Py_Finalize();

    return 0;
}