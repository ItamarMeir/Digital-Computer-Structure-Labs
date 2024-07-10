################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Ref/CCS/source/api.obj: ../Ref/CCS/source/api.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs740/ccsv7/tools/compiler/ti-cgt-msp430_16.9.11.LTS/bin/cl430" -vmsp --use_hw_mpy=none --include_path="C:/ti/ccs740/ccsv7/ccs_base/msp430/include" --include_path="C:/Users/Michael/Desktop/Digital-Computer-Structure-Labs/Lab_4" --include_path="C:/ti/ccs740/ccsv7/tools/compiler/ti-cgt-msp430_16.9.11.LTS/include" --advice:power=all --define=__MSP430G2553__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="Ref/CCS/source/api.d_raw" --obj_directory="Ref/CCS/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

Ref/CCS/source/bsp.obj: ../Ref/CCS/source/bsp.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs740/ccsv7/tools/compiler/ti-cgt-msp430_16.9.11.LTS/bin/cl430" -vmsp --use_hw_mpy=none --include_path="C:/ti/ccs740/ccsv7/ccs_base/msp430/include" --include_path="C:/Users/Michael/Desktop/Digital-Computer-Structure-Labs/Lab_4" --include_path="C:/ti/ccs740/ccsv7/tools/compiler/ti-cgt-msp430_16.9.11.LTS/include" --advice:power=all --define=__MSP430G2553__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="Ref/CCS/source/bsp.d_raw" --obj_directory="Ref/CCS/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

Ref/CCS/source/halGPIO.obj: ../Ref/CCS/source/halGPIO.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs740/ccsv7/tools/compiler/ti-cgt-msp430_16.9.11.LTS/bin/cl430" -vmsp --use_hw_mpy=none --include_path="C:/ti/ccs740/ccsv7/ccs_base/msp430/include" --include_path="C:/Users/Michael/Desktop/Digital-Computer-Structure-Labs/Lab_4" --include_path="C:/ti/ccs740/ccsv7/tools/compiler/ti-cgt-msp430_16.9.11.LTS/include" --advice:power=all --define=__MSP430G2553__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="Ref/CCS/source/halGPIO.d_raw" --obj_directory="Ref/CCS/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

Ref/CCS/source/main.obj: ../Ref/CCS/source/main.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs740/ccsv7/tools/compiler/ti-cgt-msp430_16.9.11.LTS/bin/cl430" -vmsp --use_hw_mpy=none --include_path="C:/ti/ccs740/ccsv7/ccs_base/msp430/include" --include_path="C:/Users/Michael/Desktop/Digital-Computer-Structure-Labs/Lab_4" --include_path="C:/ti/ccs740/ccsv7/tools/compiler/ti-cgt-msp430_16.9.11.LTS/include" --advice:power=all --define=__MSP430G2553__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="Ref/CCS/source/main.d_raw" --obj_directory="Ref/CCS/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


