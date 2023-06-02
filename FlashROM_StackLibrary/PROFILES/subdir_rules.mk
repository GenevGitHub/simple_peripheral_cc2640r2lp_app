################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
PROFILES/%.obj: ../PROFILES/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti-cgt-arm_18.12.5.LTS/bin/armcl" --cmd_file="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/config/build_components.opt" --cmd_file="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/config/factory_config.opt" --cmd_file="C:/Users/Public/Documents/TI-workspace/simple_peripheral_cc2640r2lp_stack_library/TOOLS/build_config.opt"  -mv7M3 --code_state=16 -me -O4 --opt_for_speed=1 --include_path="C:/Users/Public/Documents/TI-workspace/simple_peripheral_cc2640r2lp_app" --include_path="C:/Users/Public/Documents/TI-workspace/simple_peripheral_cc2640r2lp_app/Application" --include_path="C:/Users/Public/Documents/TI-workspace/simple_peripheral_cc2640r2lp_app/Startup" --include_path="C:/Users/Public/Documents/TI-workspace/simple_peripheral_cc2640r2lp_app/PROFILES" --include_path="C:/Users/Public/Documents/TI-workspace/simple_peripheral_cc2640r2lp_app/Include" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/controller/cc26xx_r2/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/rom" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/common/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/examples/rtos/CC2640R2_LAUNCHXL/blestack/simple_peripheral/src/app" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/icall/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/profiles/dev_info" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/profiles/roles/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/profiles/roles" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/profiles/simple_profile/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/profiles/simple_profile" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/target" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/hal/src/target/_common" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/hal/src/target/_common/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/hal/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/heapmgr" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/icall/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/osal/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/services/src/saddr" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/services/src/sdata" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/devices/cc26x0r2" --include_path="C:/ti/ti-cgt-arm_18.12.5.LTS/include" --include_path="C:/Users/HarryYeungYikSiu/Documents/Texas Instruments/Sensor Controller Studio/projects/ADCSampling" --define=CC2640R2_LAUNCHXL=1 --define=DeviceFamily_CC26X0R2 --define=BOARD_DISPLAY_USE_LCD=0 --define=BOARD_DISPLAY_USE_UART=1 --define=BOARD_DISPLAY_USE_UART_ANSI=1 --define=CC26XX --define=CC26XX_R2 --define=ICALL_EVENTS --define=ICALL_JT --define=ICALL_LITE --define=ICALL_MAX_NUM_ENTITIES=12 --define=ICALL_MAX_NUM_TASKS=4 --define=ICALL_STACK0_ADDR --define=MAX_NUM_BLE_CONNS=1 --define=POWER_SAVING --define=STACK_LIBRARY --define=USE_ICALL --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="PROFILES/$(basename $(<F)).d_raw" --obj_directory="PROFILES" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

PROFILES/devinfoservice.obj: C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/profiles/dev_info/cc26xx/devinfoservice.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti-cgt-arm_18.12.5.LTS/bin/armcl" --cmd_file="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/config/build_components.opt" --cmd_file="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/config/factory_config.opt" --cmd_file="C:/Users/Public/Documents/TI-workspace/simple_peripheral_cc2640r2lp_stack_library/TOOLS/build_config.opt"  -mv7M3 --code_state=16 -me -O4 --opt_for_speed=1 --include_path="C:/Users/Public/Documents/TI-workspace/simple_peripheral_cc2640r2lp_app" --include_path="C:/Users/Public/Documents/TI-workspace/simple_peripheral_cc2640r2lp_app/Application" --include_path="C:/Users/Public/Documents/TI-workspace/simple_peripheral_cc2640r2lp_app/Startup" --include_path="C:/Users/Public/Documents/TI-workspace/simple_peripheral_cc2640r2lp_app/PROFILES" --include_path="C:/Users/Public/Documents/TI-workspace/simple_peripheral_cc2640r2lp_app/Include" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/controller/cc26xx_r2/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/rom" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/common/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/examples/rtos/CC2640R2_LAUNCHXL/blestack/simple_peripheral/src/app" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/icall/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/profiles/dev_info" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/profiles/roles/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/profiles/roles" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/profiles/simple_profile/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/profiles/simple_profile" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/target" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/hal/src/target/_common" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/hal/src/target/_common/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/hal/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/heapmgr" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/icall/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/osal/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/services/src/saddr" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/blestack/services/src/sdata" --include_path="C:/ti/simplelink_cc2640r2_sdk_5_10_00_02/source/ti/devices/cc26x0r2" --include_path="C:/ti/ti-cgt-arm_18.12.5.LTS/include" --include_path="C:/Users/HarryYeungYikSiu/Documents/Texas Instruments/Sensor Controller Studio/projects/ADCSampling" --define=CC2640R2_LAUNCHXL=1 --define=DeviceFamily_CC26X0R2 --define=BOARD_DISPLAY_USE_LCD=0 --define=BOARD_DISPLAY_USE_UART=1 --define=BOARD_DISPLAY_USE_UART_ANSI=1 --define=CC26XX --define=CC26XX_R2 --define=ICALL_EVENTS --define=ICALL_JT --define=ICALL_LITE --define=ICALL_MAX_NUM_ENTITIES=12 --define=ICALL_MAX_NUM_TASKS=4 --define=ICALL_STACK0_ADDR --define=MAX_NUM_BLE_CONNS=1 --define=POWER_SAVING --define=STACK_LIBRARY --define=USE_ICALL --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="PROFILES/$(basename $(<F)).d_raw" --obj_directory="PROFILES" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


