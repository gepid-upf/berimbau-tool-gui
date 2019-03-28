/**
 * @file Cli.cpp
 * 
 * @author
 * Angelo Elias Dalzotto (150633@upf.br)
 * GEPID - Grupo de Pesquisa em Cultura Digital (gepid.upf.br)
 * Universidade de Passo Fundo (upf.br)
 * 
 * @brief Command-line interface source code.
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

#include <Cli.h>
#include <BerimbauTool.h>
#include <esptool.h>

#include <cstring>
#include <cctype>

Cli::Option Cli::opt;
std::string Cli::fname;

int Cli::run(int &argc, char *argv[])
{
    int ret = 0;
    if((ret = parse_args(argc, argv)))
        return ret;

    switch(opt){
    case Option::CREATE:
        ret = create_rec();
        break;
    case Option::DUMP:
        ret = dump_flash();
        break;
    case Option::EXPORT:
        ret = export_rec();
        break;
    case Option::FLASH:
        ret = flash_esp();
        break;
    case Option::LOGS:
        ret = mv_log();
        break;
    default:
        ret = -4;
        break;
    }

    return ret;
}

int Cli::parse_args(int &argc, char *argv[])
{
    if(argc < 2){
        print_usage();
        return -1;
    }

    if(!strcmp(argv[1], "create")){
        opt = Option::CREATE;
    } else if(!strcmp(argv[1], "dump")){
        opt = Option::DUMP;
    } else if(!strcmp(argv[1], "export")){
        opt = Option::EXPORT;
    } else if(!strcmp(argv[1], "flash")){
        opt = Option::FLASH;
    } else if(!strcmp(argv[1], "logs")) {
        opt = Option::LOGS;
    } else {
        std::cout << "Opção desconhecida!" << std::endl << std::endl;
        print_usage();
        return -2;
    }
    

    if(opt == Option::CREATE || opt == Option::EXPORT || opt == Option::LOGS){
        if(argc != 3){
            std::cout << "Necessário informar arquivo de entrada/saída" << std::endl << std::endl;
            print_usage();
            return -3;
        } else {
            fname = argv[2];
        }
    } else if((opt == Option::DUMP || opt == Option::FLASH) && argc > 2){
        std::cout << "Número de argumentos incorreto" << std::endl << std::endl;
        print_usage();
        return -4;
    }

    return 0;
}

void Cli::print_usage()
{
    std::cout << "Usage: berimbau-tool OPTION [INPUT]" << std::endl << std::endl;
    std::cout << "\tOPT:\tcreate INPUT: Cria ritmo para berimbau." << std::endl;
    std::cout << "\t\texport INPUT: Exporta ritmo criado para imagem." << std::endl;
    std::cout << "\t\tflash: Grava imagem para o Berimbau." << std::endl;
    std::cout << "\t\tdump: Copia imagem do Berimbau." << std::endl;
    std::cout << "\t\t      Recomendado executar antes de export e flash." << std::endl;
    std::cout << "\t\tlogs: Importa logs de execução do Berimbau." << std::endl;
}

int Cli::create_rec()
{
    int ret = 0;
    switch((ret = BerimbauTool::create(fname))){
    case 0:
        std::cout << "Arquivo criado em /local/recs/" << fname.substr(0, fname.length()-4) << ".dat" << std::endl;
        break;    
    case 1:
        std::cout << "Arquivo " << fname << " não existente." << std::endl;
        break;
    case 2:
        std::cout << "Formato de entrada inválido na linha " << BerimbauTool::get_error_line() << std::endl;
        break;
    case 3:
        std::cout << "Tempo inválido na linha " << BerimbauTool::get_error_line() << ": " << BerimbauTool::get_error_value() << std::endl;
        break;
    case 4:
        std::cout << "Instrumento inválido na linha " << BerimbauTool::get_error_line() << ": " << BerimbauTool::get_error_value() << std::endl;
        break;
    default:
        ret = -1;
        break;
    }
    return ret;
}

int Cli::dump_flash()
{
    int ret = BerimbauTool::dump();
    switch(ret){
    case 0:
        std::cout << std::endl << "Memória descompactada em ./img/" << std::endl;
        break;
    // esptool errors
    case 2:
    case 3:
    case 4:
    case 5:
        std::cout << std::endl << "Erro: " << ESPTool::get_err_msg() << std::endl;
        break;
    case 6:
        std::cout << std::endl << "Erro ao esperar mkspiffs" << std::endl;
        break;
    case 0xFF:
        std::cout << std::endl << "mkspiffs não encontrado" << std::endl;
        break;
    // mkspiffs errors
    default:
        std::cout << std::endl << "Erro no mkspiffs" << std::endl;
        break;
    }

    return ret;
}

int Cli::export_rec()
{
    int ret = BerimbauTool::merge(fname);
    switch(ret){
    case 0:
        std::cout << "Arquivo " << fname << " copiado para exportação" << std::endl;
        break;
    case 1:
        std::cout << "Arquivo de entrada inválido" << std::endl;
        break;
    case 2:
        std::cout << "Imagem para exportação não presente. Execute 'berimbau-tool dump'" << std::endl;
        break;
    case 3:
        std::cout << "Erro ao copiar arquivo" << std::endl;
        break;
    default:
        std::cout << "Erro desconhecido" << std::endl;
        break;
    }

    return ret;
}

int Cli::flash_esp()
{
    int ret = BerimbauTool::flash();
    switch(ret){
    case 0:
        std::cout << std::endl << "Memória gravada com sucesso" << std::endl;
        break;
    default:
        std::cout << std::endl << "Erro: " << ESPTool::get_err_msg() << std::endl;
        break;
    }

    return ret;
}

int Cli::mv_log()
{
    int ret = BerimbauTool::log(fname);
    switch(ret){
    case 0:
        std::cout << std::endl << "Log salvo em " << fname << std::endl;
        break;
    case 1:
        std::cout << std::endl << "Nenhum dump encontrado" << std::endl;
        break;
    case 2:
        std::cout << std::endl << "Nenhum log para ser copiado no dump" << std::endl;
        break;
    case 3:
        std::cout << std::endl << "Falha ao copiar logs" << std::endl;
        break;
    default:
        break;
    }

    return ret;
}