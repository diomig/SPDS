################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccs831/ccsv8/tools/compiler/c5500_4.4.1/bin/cl55" --memory_model=large -g --include_path="C:/ti/ccs831/ccsv8/tools/compiler/c5500_4.4.1/include" --include_path="C:/ti/ccs831/ccsv8/ccs_base/usbstk5515_BSL_RevA/usbstk5515_v1/include" --define=c5515 --diag_warning=225 --ptrdiff_size=32 --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


