cmake_minimum_required(VERSION 3.8)
project(CRISPR-casses-finder)

set(CMAKE_CXX_STANDARD 17)


set(SOURCE_FILES main.cpp graph_assembly.h visualise.h graph_analysis.h)
add_executable(CRISPR-cassets-finder ${SOURCE_FILES})