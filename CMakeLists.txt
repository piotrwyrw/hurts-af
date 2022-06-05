cmake_minimum_required(VERSION 3.16)
project(PainLang C)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY bin)

set(CMAKE_C_STANDARD 99)

add_executable(PainLang src/main.c src/tokenizer.c src/tokenizer.h src/parser.c src/parser.h src/util.c src/util.h)
