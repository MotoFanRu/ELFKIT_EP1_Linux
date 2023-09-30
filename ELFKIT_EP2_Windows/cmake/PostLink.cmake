add_custom_command(TARGET ${PROJECT_NAME}
	COMMAND ${P2K_EP2_TOOLCHAIN_ROOT}/libgen/postlink.exe ${PROJECT_NAME}.elfp -o ${PROJECT_NAME}.elf
	COMMENT "Postlinking ${PROJECT_NAME}.elfp to ${PROJECT_NAME}.elf"
	VERBATIM
)
