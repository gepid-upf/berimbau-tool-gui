# berimbau-tool-gui

berimbau-tool-gui is a GUI frontend to the [berimbau-tool](https://github.com/aedalzotto/berimbau-tool): a software for creating recordings for the [berimbau-esp32](https://github.com/aedalzotto/berimbau-esp32) based on a .csv input file, dumping the recordings and recovering its logs.

## Built with

* g++ (GCC) 8.2.1
* std=c++17
* stdc++fs
* Python2.7
* GTKmm 3
* esptool.py v2.6-beta1
* mkspiffs SPIFFS_OBJ_META_LEN=4

## Compiling

On linux, it will automatically compile, copy files and build mkspiffs too with:
```
make
```

To compile for Windows, the cross mingw/pixman/cairo/gtkmm is with problems.
You may have to compile using MSYS2 and then copy the files to the folder structure:
```
g++.exe src/*.cpp -lstdc++fs -stc=c++17 `pkg-config --libs --cflags gtkmm-3.0 python-2.7`
cd mkspiffs
make
```

## Authors

* **Angelo Elias Dalzotto** - *Berimbau-tool and GUI* - [aedalzotto](https://github.com/aedalzotto)

## Acknowledgements

* **Ivan Grokhotkov** - *[mkspiffs](https://github.com/igrr/mkspiffs)* - [igrr](https://github.com/igrr)

## License

This project is licensed under the GPLv2 - see the [LICENSE.md](LICENSE.md) file for details
