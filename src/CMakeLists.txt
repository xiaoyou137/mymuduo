aux_source_directory(. SRC_LIST)

add_definitions(-DMUDEBUG)

add_library(mymuduo SHARED ${SRC_LIST})