################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
src/main.obj: ../src/main.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" --cmd_file="C:/ti/ble_sdk_2_02_02_25/src/config/build_components.opt" --cmd_file="C:/ti/ble_sdk_2_02_02_25/ble_examples/examples/cc2650lp/simple_central/ccs/app/../../iar/stack/build_config.opt" --cmd_file="C:/ti/ble_sdk_2_02_02_25/ble_examples/examples/cc2650lp/simple_central/ccs/app/../config/ccs_compiler_defines.bcfg"  -mv7M3 --code_state=16 --float_support=vfplib -me -O4 --opt_for_speed=0 --include_path="E:/git/EVRS_TX_PROTOTYPE/simple_central_cc2650lp_app/src" --include_path="E:/git/EVRS_TX_PROTOTYPE/simple_central_cc2650lp_app" --include_path="C:/ti/ble_sdk_2_02_02_25/src/controller/cc26xx/inc" --include_path="C:/ti/ble_sdk_2_02_02_25/src/inc" --include_path="C:/ti/ble_sdk_2_02_02_25/src/common/cc26xx" --include_path="C:/ti/ble_sdk_2_02_02_25/src/icall/inc" --include_path="C:/ti/ble_sdk_2_02_02_25/src/inc" --include_path="C:/ti/ble_sdk_2_02_02_25/src/profiles/roles" --include_path="C:/ti/ble_sdk_2_02_02_25/src/profiles/roles/cc26xx" --include_path="C:/ti/ble_sdk_2_02_02_25/src/target" --include_path="C:/ti/ble_sdk_2_02_02_25/src/components/hal/src/inc" --include_path="C:/ti/ble_sdk_2_02_02_25/src/components/hal/src/target/_common" --include_path="C:/ti/ble_sdk_2_02_02_25/src/components/hal/src/target/_common/cc26xx" --include_path="C:/ti/ble_sdk_2_02_02_25/src/components/heapmgr" --include_path="C:/ti/ble_sdk_2_02_02_25/src/components/icall/src/inc" --include_path="C:/ti/ble_sdk_2_02_02_25/src/components/osal/src/inc" --include_path="C:/ti/ble_sdk_2_02_02_25/src/components/services/src/saddr" --include_path="C:/ti/ble_sdk_2_02_02_25/src/components/services/src/sdata" --include_path="C:/ti/tirtos_cc13xx_cc26xx_2_21_01_08/products/cc26xxware_2_24_03_17272" --include_path="C:/ti/ble_sdk_2_02_02_25/ble_examples/src/components/display_eng" --include_path="C:/ti/tirtos_cc13xx_cc26xx_2_21_01_08/products/tidrivers_cc13xx_cc26xx_2_21_01_01/packages" --include_path="E:/git/EVRS_TX_PROTOTYPE/ble_board_driver" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --define=USE_ICALL --define=MAX_PDU_SIZE=162 --define=POWER_SAVING --define=GAPCENTRALROLE_TASK_STACK_SIZE=510 --define=HEAPMGR_SIZE=0 --define=xDisplay_DISABLE_ALL --define=xBOARD_DISPLAY_EXCLUDE_UART --define=BOARD_DISPLAY_EXCLUDE_LCD --define=GAPCENTRALROLE_NUM_RSSI_LINKS=1 --define=ICALL_MAX_NUM_TASKS=3 --define=ICALL_MAX_NUM_ENTITIES=6 --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL --define=CC26XX --define=CC2650_LAUNCHXL -g --c99 --relaxed_ansi --plain_char=unsigned --diag_suppress=48 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --embedded_constants=on --unaligned_access=on --enum_type=packed --wchar_t=16 --abi=eabi --common=on --fp_reassoc=off --sat_reassoc=off --preproc_with_compile --preproc_dependency="src/main.d_raw" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

src/simple_central.obj: ../src/simple_central.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" --cmd_file="C:/ti/ble_sdk_2_02_02_25/src/config/build_components.opt" --cmd_file="C:/ti/ble_sdk_2_02_02_25/ble_examples/examples/cc2650lp/simple_central/ccs/app/../../iar/stack/build_config.opt" --cmd_file="C:/ti/ble_sdk_2_02_02_25/ble_examples/examples/cc2650lp/simple_central/ccs/app/../config/ccs_compiler_defines.bcfg"  -mv7M3 --code_state=16 --float_support=vfplib -me -O4 --opt_for_speed=0 --include_path="E:/git/EVRS_TX_PROTOTYPE/simple_central_cc2650lp_app/src" --include_path="E:/git/EVRS_TX_PROTOTYPE/simple_central_cc2650lp_app" --include_path="C:/ti/ble_sdk_2_02_02_25/src/controller/cc26xx/inc" --include_path="C:/ti/ble_sdk_2_02_02_25/src/inc" --include_path="C:/ti/ble_sdk_2_02_02_25/src/common/cc26xx" --include_path="C:/ti/ble_sdk_2_02_02_25/src/icall/inc" --include_path="C:/ti/ble_sdk_2_02_02_25/src/inc" --include_path="C:/ti/ble_sdk_2_02_02_25/src/profiles/roles" --include_path="C:/ti/ble_sdk_2_02_02_25/src/profiles/roles/cc26xx" --include_path="C:/ti/ble_sdk_2_02_02_25/src/target" --include_path="C:/ti/ble_sdk_2_02_02_25/src/components/hal/src/inc" --include_path="C:/ti/ble_sdk_2_02_02_25/src/components/hal/src/target/_common" --include_path="C:/ti/ble_sdk_2_02_02_25/src/components/hal/src/target/_common/cc26xx" --include_path="C:/ti/ble_sdk_2_02_02_25/src/components/heapmgr" --include_path="C:/ti/ble_sdk_2_02_02_25/src/components/icall/src/inc" --include_path="C:/ti/ble_sdk_2_02_02_25/src/components/osal/src/inc" --include_path="C:/ti/ble_sdk_2_02_02_25/src/components/services/src/saddr" --include_path="C:/ti/ble_sdk_2_02_02_25/src/components/services/src/sdata" --include_path="C:/ti/tirtos_cc13xx_cc26xx_2_21_01_08/products/cc26xxware_2_24_03_17272" --include_path="C:/ti/ble_sdk_2_02_02_25/ble_examples/src/components/display_eng" --include_path="C:/ti/tirtos_cc13xx_cc26xx_2_21_01_08/products/tidrivers_cc13xx_cc26xx_2_21_01_01/packages" --include_path="E:/git/EVRS_TX_PROTOTYPE/ble_board_driver" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --define=USE_ICALL --define=MAX_PDU_SIZE=162 --define=POWER_SAVING --define=GAPCENTRALROLE_TASK_STACK_SIZE=510 --define=HEAPMGR_SIZE=0 --define=xDisplay_DISABLE_ALL --define=xBOARD_DISPLAY_EXCLUDE_UART --define=BOARD_DISPLAY_EXCLUDE_LCD --define=GAPCENTRALROLE_NUM_RSSI_LINKS=1 --define=ICALL_MAX_NUM_TASKS=3 --define=ICALL_MAX_NUM_ENTITIES=6 --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL --define=CC26XX --define=CC2650_LAUNCHXL -g --c99 --relaxed_ansi --plain_char=unsigned --diag_suppress=48 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --embedded_constants=on --unaligned_access=on --enum_type=packed --wchar_t=16 --abi=eabi --common=on --fp_reassoc=off --sat_reassoc=off --preproc_with_compile --preproc_dependency="src/simple_central.d_raw" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

