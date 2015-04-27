message(STATUS "    ${PLUGIN_NAME} provides translations")
if (UPDATE_TRANSLATIONS)
	qt4_create_translation(TS_FILES
		${FILES_TO_TRANSLATE}
		${TRANSLATIONS_FILES}
	)
	add_custom_target(${PLUGIN_NAME}_translations_target DEPENDS ${TS_FILES})
	message(STATUS "    make ${PLUGIN_NAME}_translations_target")
else ()
	add_custom_target(${PLUGIN_NAME}_translations_target)
endif ()

qt4_add_translation(QM_FILES ${TRANSLATIONS_FILES})
