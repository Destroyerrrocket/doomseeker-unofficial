function(append_file_list file_name var_name file_list)
	file(APPEND ${file_name} "set(${var_name}\n")
	list(SORT file_list)
	foreach(file ${file_list})
		file(APPEND ${file_name} "\t${file}\n")
	endforeach(file)
	file(APPEND ${file_name} ")\n")
endfunction()

set(FILE_NAME "CMakeFileListing.cmake")
file(WRITE ${FILE_NAME} "")

file(GLOB_RECURSE UI_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.ui")
append_file_list(${FILE_NAME} "UI_FILES" "${UI_FILES}")

file(GLOB_RECURSE HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h" "*.hpp")
append_file_list(${FILE_NAME} "HEADER_FILES" "${HEADER_FILES}")

file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
append_file_list(${FILE_NAME} "SOURCE_FILES" "${SOURCE_FILES}")
