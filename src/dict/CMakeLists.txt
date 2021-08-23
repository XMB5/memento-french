add_library(
    dictionary_db
    dictionary.cpp
    expression.h
        dictreader.cpp dictreader.h frenchprocessor.cpp frenchprocessor.h)
target_link_libraries(
    dictionary_db
    ZLIB::ZLIB
    Qt5::Core
    Qt5::Widgets
)