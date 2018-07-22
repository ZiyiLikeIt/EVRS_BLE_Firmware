################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-117036593:
	@$(MAKE) --no-print-directory -Onone -f TOOLS/subdir_rules.mk build-117036593-inproc

build-117036593-inproc: E:/Documents/git/ble_examples/examples/cc2650lp/simple_central/ccs/config/app_ble.cfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: XDCtools'
	"C:/ti/xdctools_3_32_00_06_core/xs" --xdcpath="C:/ti/tirtos_cc13xx_cc26xx_2_21_01_08/packages;C:/ti/tirtos_cc13xx_cc26xx_2_21_01_08/products/tidrivers_cc13xx_cc26xx_2_21_01_01/packages;C:/ti/tirtos_cc13xx_cc26xx_2_21_01_08/products/bios_6_46_01_38/packages;C:/ti/tirtos_cc13xx_cc26xx_2_21_01_08/products/uia_2_01_00_01/packages;C:/ti/ble_sdk_2_02_02_25;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M3 -p ti.platforms.simplelink:CC2650F128 -r release -c "C:/ti/ccsv8/tools/compiler/ti-cgt-arm_16.9.4.LTS" --compileOptions "-mv7M3 --code_state=16 --float_support=vfplib -me -O4 --opt_for_speed=0 --include_path=\"E:/git/EVRS_TX_PROTOTYPE/simple_central_cc2650lp_app\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/examples/simple_central/cc26xx/app\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/controller/cc26xx/inc\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/inc\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/common/cc26xx\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/icall/inc\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/inc\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/profiles/roles\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/profiles/roles/cc26xx\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/profiles/simple_profile\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/target\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/components/hal/src/inc\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/components/hal/src/target/_common\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/components/hal/src/target/_common/cc26xx\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/components/heapmgr\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/components/icall/src/inc\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/components/osal/src/inc\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/components/services/src/saddr\" --include_path=\"C:/ti/ble_sdk_2_02_02_25/src/components/services/src/sdata\" --include_path=\"C:/ti/tirtos_cc13xx_cc26xx_2_21_01_08/products/cc26xxware_2_24_03_17272\" --include_path=\"E:/Documents/git/ble_examples/src/components/display_eng\" --include_path=\"C:/ti/tirtos_cc13xx_cc26xx_2_21_01_08/products/tidrivers_cc13xx_cc26xx_2_21_01_01/packages\" --include_path=\"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_16.9.4.LTS/include\" --define=USE_ICALL --define=MAX_PDU_SIZE=162 --define=POWER_SAVING --define=GAPCENTRALROLE_TASK_STACK_SIZE=510 --define=HEAPMGR_SIZE=0 --define=xDisplay_DISABLE_ALL --define=xBOARD_DISPLAY_EXCLUDE_UART --define=BOARD_DISPLAY_EXCLUDE_LCD --define=GAPCENTRALROLE_NUM_RSSI_LINKS=1 --define=ICALL_MAX_NUM_TASKS=3 --define=ICALL_MAX_NUM_ENTITIES=6 --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL --define=CC26XX --define=CC2650_LAUNCHXL -g --c99 --relaxed_ansi --plain_char=unsigned --diag_suppress=48 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --embedded_constants=on --unaligned_access=on --enum_type=packed --wchar_t=16 --abi=eabi --common=on --fp_reassoc=off --sat_reassoc=off  " "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

configPkg/linker.cmd: build-117036593 E:/Documents/git/ble_examples/examples/cc2650lp/simple_central/ccs/config/app_ble.cfg
configPkg/compiler.opt: build-117036593
configPkg/: build-117036593


