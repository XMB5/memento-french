add_library(
    ankisettings
    ankisettings.cpp
    ankisettings.ui
    cardbuilder.cpp
    cardbuilder.ui
    ankisettingshelp.cpp
    ankisettingshelp.ui
)
target_link_libraries(
    ankisettings
    globalmediator
    Qt5::Widgets
    anki
    scrollcombobox
    scrolldoublespinbox
)

add_library(
    searchsettings
    searchsettings.cpp
    searchsettings.ui
)
target_link_libraries(
    searchsettings
    Qt5::Widgets
    Qt5::Core
)

add_library(
    interfacesettings
    interfacesettings.cpp
    interfacesettings.ui
)
target_link_libraries(
    interfacesettings
    Qt5::Widgets
    Qt5::Core
)

add_library(
    optionswindow
    optionswindow.cpp
    optionswindow.ui
)
target_link_libraries(
    optionswindow
    ankisettings
    searchsettings
    interfacesettings
)