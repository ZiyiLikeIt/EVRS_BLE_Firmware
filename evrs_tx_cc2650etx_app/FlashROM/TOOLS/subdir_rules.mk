################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-31899311:
	@$(MAKE) --no-print-directory -Onone -f TOOLS/subdir_rules.mk build-31899311-inproc

build-31899311-inproc: C:/ti/ble_sdk_2_02_02_25/examples/cc2650lp/simple_peripheral/ccs/config/app_ble.cfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: XDCtools'
	"C:/ti/xdctools_3_32_00_06_core/xs" --xdcpath="C:/ti/tirtos_cc13xx_cc26xx_2_21_01_08/packages;C:/ti/tirtos_cc13xx_cc26xx_2_21_01_08/products/tidrivers_cc13xx_cc26xx_2_21_01_01/packages;C:/ti/tirtos_cc13xx_cc26xx_2_21_01_08/products/bios_6_46_01_38/packages;C:/ti/tirtos_cc13xx_cc26xx_2_21_01_08/products/uia_2_01_00_01/packages;C:/ti/ccsv8/ccs_base;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M3 -p ti.platforms.simplelink:CC2650F128 -r release -c "C:/ti/ccsv8/tools/compiler/ti-cgt-arm_16.9.4.LTS" --compileOptions "-mv7M3 --code_state=16 -me -O4 --opt_for_speed=0 --include_path=\"E:/workbench/git/EVRS_BLE_Firmware/evrs_tx_cc2650etx_app/src\" --include_path=\"E:/workbench/git/EVRS_BLE_Firmware/evrs_tx_cc2650etx_app/drv\" --include_path=\"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_16.9.4.LTS/include\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/inc\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/icall/inc\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/profiles/roles/cc26xx\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/profiles/roles\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/profiles/dev_info\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/common/cc26xx\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/components/heapmgr\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/controller/cc26xx/inc\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/components/hal/src/target/_common\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/target\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/components/hal/src/target/_common/cc26xx\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/components/hal/src/inc\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/components/osal/src/inc\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/components/services/src/sdata\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/components/services/src/saddr\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/components/icall/src/inc\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/inc\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/rom\" --include_path=\"C:/ti/tirtos_cc13xx_cc26xx_2_21_01_08/products/cc26xxware_2_24_03_17272\" --define=BOARD_DISPLAY_EXCLUDE_LCD --define=CC2650_ETX --define=xBOARD_DISPLAY_EXCLUDE_UART --define=xCC2650_LAUNCHXL --define=CC26XX --define=xDisplay_DISABLE_ALL --define=GAPROLE_TASK_STACK_SIZE=540 --define=HEAPMGR_SIZE=0 --define=ICALL_MAX_NUM_ENTITIES=6 --define=ICALL_MAX_NUM_TASKS=3 --define=POWER_MEASURE --define=POWER_SAVING --define=USE_ICALL --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL -g --c99 --gcc --diag_suppress=48 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi " "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

configPkg/linker.cmd: build-31899311 C:/ti/ble_sdk_2_02_02_25/examples/cc2650lp/simple_peripheral/ccs/config/app_ble.cfg
configPkg/compiler.opt: build-31899311
configPkg/: build-31899311


