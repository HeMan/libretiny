/* Copyright (c) Kuba Szczodrzyński 2023-05-22. */

#include <libretiny.h>
#include <sdk_private.h>

extern uint8_t lt_uart_port;

void lt_init_family() {
	// make the SDK less verbose by default
	ConfigDebugErr	= 0;
	ConfigDebugWarn = 0;
	ConfigDebugInfo = 0;
	lt_uart_port	= LT_UART_DEFAULT_PORT;
}

/* _____ _____  _    _
  / ____|  __ \| |  | |
 | |    | |__) | |  | |
 | |    |  ___/| |  | |
 | |____| |    | |__| |
  \_____|_|     \____*/
lt_cpu_model_t lt_cpu_get_model() {
	uint32_t *addr	   = (uint32_t *)0x40000038;
	uint8_t flash_mode = (addr[0] >> 5) & 0b11;
	uint32_t chip_id   = 0;
	hal_get_chip_id(&chip_id);
	chip_id <<= 2;
	return CPU_MODEL_ENUM(FAMILY, (chip_id & 0xFF) | flash_mode);
}

const char *lt_cpu_get_core_type() {
	return "ARM Cortex-M33 (ARMv8-M)";
}

uint32_t lt_cpu_get_freq() {
	return hal_syson_query_sys_clk();
}

/*_____             _
 |  __ \           (_)
 | |  | | _____   ___  ___ ___
 | |  | |/ _ \ \ / / |/ __/ _ \
 | |__| |  __/\ V /| | (_|  __/
 |_____/ \___| \_/ |_|\___\__*/
void lt_get_device_mac(uint8_t *mac) {
	efuse_logical_read(0x11A, 6, mac);
}

void lt_reboot() {
	sys_cpu_reset();
	while (1) {}
}

bool lt_reboot_download_mode() {
	sys_uart_download_mode();
	while (1) {}
	return true;
}

lt_reboot_reason_t lt_get_reboot_reason() {
	hal_reset_reason_t reason = -1;
	rtl8710c_reset_reason_get(&reason);
	switch (reason) {
		case HAL_RESET_REASON_POWER_ON:
			return REBOOT_REASON_POWER;
		case HAL_RESET_REASON_SOFTWARE:
			return REBOOT_REASON_SOFTWARE;
		case HAL_RESET_REASON_WATCHDOG:
			return REBOOT_REASON_WATCHDOG;
		case HAL_RESET_REASON_JTAG:
			return REBOOT_REASON_DEBUGGER;
		default:
			return REBOOT_REASON_UNKNOWN;
	}
}

bool lt_set_debug_mode(lt_debug_mode_t mode) {
	switch (mode) {
		case DEBUG_MODE_OFF:
			if (hal_misc_jtag_pin_ctrl(0) != HAL_OK)
				return false;
			if (hal_misc_swd_pin_ctrl(0) != HAL_OK)
				return false;
			return true;
		case DEBUG_MODE_JTAG:
			if (hal_misc_swd_pin_ctrl(0) != HAL_OK)
				return false;
			if (hal_misc_jtag_pin_ctrl(1) != HAL_OK)
				return false;
			return true;
		case DEBUG_MODE_SWD:
			if (hal_misc_jtag_pin_ctrl(0) != HAL_OK)
				return false;
			if (hal_misc_swd_pin_ctrl(1) != HAL_OK)
				return false;
			return true;
		default:
			return false;
	}
}

/*__  __
 |  \/  |
 | \  / | ___ _ __ ___   ___  _ __ _   _
 | |\/| |/ _ \ '_ ` _ \ / _ \| '__| | | |
 | |  | |  __/ | | | | | (_) | |  | |_| |
 |_|  |_|\___|_| |_| |_|\___/|_|   \__, |
									__/ |
								   |__*/
uint32_t lt_ram_get_size() {
	return 256 * 1024;
}

/* ____ _______
  / __ \__   __|/\
 | |  | | | |  /  \
 | |  | | | | / /\ \
 | |__| | | |/ ____ \
  \____/  |_/_/    \*/
lt_ota_type_t lt_ota_get_type() {
	return OTA_TYPE_DUAL;
}

bool lt_ota_is_valid(uint8_t index) {
	return false;
}

uint8_t lt_ota_dual_get_current() {
	return 0;
}

uint8_t lt_ota_dual_get_stored() {
	return 0;
}

bool lt_ota_switch(bool revert) {
	return false;
}
