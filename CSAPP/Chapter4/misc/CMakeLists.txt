cmake_minimum_required(VERSION 3.8)
project(CSCPP C)

set(CMAKE_C_STANDARD 99)

#find_package(BISON)
#find_package(FLEX)
#
#
#FLEX_TARGET(YasGramer yas-grammar.lex ${CMAKE_CURRENT_BINARY_DIR}/yas-grammar.c)
#BISON_TARGET(HclTab hcl.y ${CMAKE_CURRENT_BINARY_DIR}/hcl.tab.c DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/hcl.tab.h)
#FLEX_TARGET(LexYY hcl.lex ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c)
#ADD_FLEX_BISON_DEPENDENCY(YasGramer HclTab)
#ADD_FLEX_BISON_DEPENDENCY(LexYY HclTab)
#
#include_directories(${CMAKE_CURRENT_BINARY_DIR})
#
#add_executable(yis yis.c isa.c)
#
#add_executable(yas
#        yas.c
#        isa.c
#        ${FLEX_YasGramer_OUTPUTS}
#        )
#
#add_executable(hcl2c
#        node.c
#        outgen.c
#        ${BISON_HclTab_OUTPUTS}
#        ${FLEX_LexYY_OUTPUTS}
#        )