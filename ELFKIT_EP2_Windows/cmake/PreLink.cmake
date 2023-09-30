function(redefine_sym SYM_IN SYM_OUT OBJECT_FILE)
	add_custom_command(
		TARGET ${PROJECT_NAME}
		PRE_LINK
		COMMAND ${CMAKE_OBJCOPY} --redefine-sym ${SYM_IN}=${SYM_OUT} ${OBJECT_FILE}
		COMMENT "Redefining ${SYM_IN} to ${SYM_OUT} in ${OBJECT_FILE}"
		VERBATIM
	)
endfunction()
