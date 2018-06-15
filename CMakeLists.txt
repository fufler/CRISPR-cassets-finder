cmake_minimum_required(VERSION 3.8)
project(CRISPR-casses-finder)

set(CMAKE_CXX_STANDARD 17)

# Хэдэры не нужно перечислять в SOURCE_FILES — только *.cpp
# В хэдэрах должны быть только объявления функций, структур/классов и и.д. — никаких реализаций
# Реализация может быть в хэдэре только в одном случае — если это шаблон
set(SOURCE_FILES main.cpp graph_assembly.h visualise.h graph_analysis.h)
add_executable(CRISPR-cassets-finder ${SOURCE_FILES})
