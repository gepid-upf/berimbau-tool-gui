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
#include <Python.h>

#ifdef _WIN32

#include <windows.h>

#elif defined(__linux__)

#include <sys/wait.h>

#else

#error "Platform not available"

#endif

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

int Util::System::call_and_wait(int argc, char *argv[])
{
#ifdef _WIN32
    std::string argv_concat;
    for(int i = 0; i < argc; i++){
        argv_concat += argv[i];
        if(i != argc-1) argv_concat += " ";
    }
    char argvbuf[512] = {};
    strcpy(argvbuf, argv_concat.c_str());

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    if(!CreateProcess(argv[0], argvbuf, nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi))
        return -1;

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    DWORD ret;
    GetExitCodeProcess(pi.hProcess, &ret);

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

    return (int)ret;

#elif defined(__linux__)
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
#else 
    #error Invalid Platform
#endif
}

std::string Util::Python::err_msg;

int Util::Python::call_funct(std::string module, std::string funct, int argc, char *argv[])
{
    Py_Initialize();

    PySys_SetArgv(argc, argv); // Sets esptool.py path and args

    PyObject* sysPath = PySys_GetObject((char*)"path");
    PyObject* programName = PyString_FromString(std::string(argv[0]).substr(0, std::string(argv[0]).find_last_of(SP)).c_str());
    PyList_Append(sysPath, programName);
    Py_DECREF(programName);

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

// https://stackoverflow.com/questions/143174/how-do-i-get-the-directory-that-a-program-is-running-from/198099#198099?newreg=30e6b2ca6b7743779a3218025156cbe7
std::string Util::System::get_program_path()
{
    char pBuf[256] = {};
    size_t len = sizeof(pBuf);

#ifdef _WIN32
    GetModuleFileName(NULL, pBuf, len);
#elif defined(__linux__)
    char szTmp[32] = {};
    sprintf(szTmp, "/proc/%d/exe", getpid());
    readlink(szTmp, pBuf, len);
#else
    #error Invalid Platform
#endif

    return std::string(pBuf);
}

void Util::System::copy_file_overwrite_workaround(const fs::path &src, const fs::path &dst)
{
	try {
	#ifdef _WIN32
		if(!fs::exists(dst))
			fs::copy_file(src, dst);
		else {
			fs::remove(dst);
			fs::copy_file(src, dst);
		}
	#else
		fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
	#endif
	} catch(...) {
		throw;
	}
}

void Util::System::merge_folders_overwrite_workaround(const fs::path &src, const fs::path &dst)
{
	try {
	#ifdef _WIN32
		for(auto &p : fs::directory_iterator(src)){
            if(fs::is_directory(p.path()))  // recursive
                merge_folders_overwrite_workaround(p.path(), dst);
			copy_file_overwrite_workaround(p.path(), dst/p.path().filename());
		}
	#else
		fs::copy(src, dst, fs::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
	#endif
	} catch(...){
		throw;
	}
}