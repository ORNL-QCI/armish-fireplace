# This can be made more robust in the future, but for now this is good enough
set(installFile "/usr/local/bin/sabot")

if(EXISTS ${installFile})
	message(STATUS "Removing file: '${installFile}'")
	execute_process(COMMAND ${CMAKE_COMMAND} -E remove ${installFile}
			OUTPUT_VARIABLE rm_out
			RESULT_VARIABLE rm_retval)
	if(NOT ${rm_retval} STREQUAL 0)
		message(FATAL_ERROR "Failed to remove file: '${installFile}'. Do you have permission?")
	endif()
else()
	message(STATUS "File '${installFile}' does not exist.")
endif()
