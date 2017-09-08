# See PluginHeader.txt

target_include_directories(${PLUGIN_NAME}
PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR} 
)

target_link_libraries(${PLUGIN_NAME} LINK_PUBLIC ${PLUGIN_LIBS} ${QT_LIBRARIES} doomseeker)

set_target_properties(${PLUGIN_NAME} PROPERTIES AUTOMOC ON)

# Installation is on by default unless explicitly disabled.
if (NOT DONT_INSTALL) # if install
	if(UNIX AND NOT APPLE)
		install(FILES $<TARGET_FILE:${PLUGIN_NAME}> DESTINATION share/doomseeker/engines)
		if (QM_FILES)
			install(FILES ${QM_FILES} DESTINATION share/doomseeker/translations)
		endif ()
	elseif(WIN32)
		install(TARGETS ${PLUGIN_NAME} 
			RUNTIME DESTINATION engines
			LIBRARY DESTINATION engines
			OPTIONAL
		)
		if (QM_FILES)
			install(FILES ${QM_FILES} DESTINATION translations)
		endif ()
	endif()
endif()

add_custom_command(TARGET ${PLUGIN_NAME} PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/engines
)
add_custom_command(TARGET ${PLUGIN_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${PLUGIN_NAME}> ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/engines
)

# Copy translations
if (QM_FILES)
	add_custom_command(TARGET ${PLUGIN_NAME} PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/translations
	)

	foreach(FILE_TO_COPY ${QM_FILES})
		add_custom_command(TARGET ${PLUGIN_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			${FILE_TO_COPY} ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/translations
		)
	endforeach(FILE_TO_COPY)
endif()
