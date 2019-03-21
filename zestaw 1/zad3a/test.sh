#! /bin/bash

make clean
make static
./main create_table 100 search_directory /usr main main_res search_directory /usr .c c_res remove_block 1 search_directory /usr .cpp cpp_res remove_block 2
make clean
make shared
./main create_table 100 search_directory /usr main main_res search_directory /usr .c c_res remove_block 1 search_directory /usr .cpp cpp_res remove_block 2
make clean
make dynamic
./main create_table 100 search_directory /usr main main_res search_directory /usr .c c_res remove_block 1 search_directory /usr .cpp cpp_res remove_block 2