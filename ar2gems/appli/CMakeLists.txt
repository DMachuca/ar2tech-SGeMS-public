# Process all headers with Q_OBJECT
QT4_WRAP_CPP(appli_MOC
  project.h
  root_model.h
)

add_library(ar2gems_appli
  library_appli_init.cpp
  project.cpp
  root_model.cpp
  utilities.cpp
  project.h
  root_model.h
  utilities.h  
  ${appli_MOC}
)

SOURCE_GROUP("Moc Files" REGULAR_EXPRESSION moc_.*cxx$)
SOURCE_GROUP("Uic Files" REGULAR_EXPRESSION ui_.*h)

target_link_libraries(ar2gems_appli
      ar2gems_utils
    )
set_target_properties( ar2gems_appli PROPERTIES OUTPUT_NAME_RELEASE ar2gems_appli )
set_target_properties( ar2gems_appli PROPERTIES OUTPUT_NAME_DEBUG   ar2gems_appli_d )

if (WIN32)
  add_definitions(-DAPPLI_EXPORTS -DGSTLAPPLI_SILENT)
endif()

         
INSTALL(TARGETS ar2gems_appli DESTINATION "${INSTALL_DIR_LIB}" COMPONENT application)
