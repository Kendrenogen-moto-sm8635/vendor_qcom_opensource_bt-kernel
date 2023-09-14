/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2016-2021, The Linux Foundation. All rights reserved.
 * Copyright (c) 2021-2023 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#ifndef __LINUX_BLUETOOTH_POWER_H
#define __LINUX_BLUETOOTH_POWER_H

#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/mailbox_client.h>
#include <linux/mailbox/qmp.h>
#include <linux/workqueue.h>
#include <linux/skbuff.h>

/*
 * voltage regulator information required for configuring the
 * bluetooth chipset
 */

enum power_modes {
	POWER_DISABLE = 0,
	POWER_ENABLE,
	POWER_RETENTION,
	POWER_DISABLE_RETENTION,
};

enum SubSystem {
	BLUETOOTH = 1,
	UWB,
};

enum power_states {
	IDLE = 0,
	BT_ON,
	UWB_ON,
	ALL_CLIENTS_ON,
};

enum retention_states {
	/* Default state */
	RETENTION_IDLE = 0,
	/* When BT is only client and it is in retention_state */
	BT_IN_RETENTION,
	/* BT is retention mode and UWB powered ON triggered */
	BT_OUT_OF_RETENTION,
	/* When UWB is only client and it is in retention_state */
	UWB_IN_RETENTION,
	/* UWB is retention mode and BT powered ON triggered */
	UWB_OUT_OF_RETENTION,
	/* Both clients are voted for retention */
	BOTH_CLIENTS_IN_RETENTION,
};

enum grant_return_values {
	ACCESS_GRANTED = 0,
	ACCESS_DENIED  = 1,
	ACCESS_RELEASED = 2,
	ACCESS_DISALLOWED = -1,
};

enum grant_states {
	/* Default state */
	NO_GRANT_FOR_ANY_SS = 0,
	NO_OTHER_CLIENT_WAITING_FOR_GRANT,
	BT_HAS_GRANT,
	UWB_HAS_GRANT,
	BT_WAITING_FOR_GRANT,
	UWB_WAITING_FOR_GRANT,
};

static inline char *ConvertGrantRetToString(enum grant_return_values state)
{
	switch (state) {
	case ACCESS_GRANTED:
		return "ACCESS_GRANTED";
	case ACCESS_DENIED:
		return "ACCESS_DENIED";
	case ACCESS_RELEASED:
		return "ACCESS_RELEASED";
	case ACCESS_DISALLOWED:
		return "ACCESS_DISALLOWED";
	default:
		return "INVALID State";
	}
}

static inline char *ConvertGrantToString(enum grant_states state)
{
	switch (state) {
	case NO_GRANT_FOR_ANY_SS:
		return "NO_GRANT_FOR_ANY_SS";
	case NO_OTHER_CLIENT_WAITING_FOR_GRANT:
		return "NO_OTHER_CLIENT_WAITING_FOR_GRANT";
	case BT_HAS_GRANT :
		return "BT_HAS_GRANT";
	case UWB_HAS_GRANT:
		return "UWB_HAS_GRANT";
	case BT_WAITING_FOR_GRANT :
		return "BT_WAITING_FOR_GRANT";
	case UWB_WAITING_FOR_GRANT:
		return "UWB_WAITING_FOR_GRANT";
	default:
		return "INVALID STATE";
	}
}

enum cores {
	BT_CORE = 0,
	UWB_CORE,
	PLATFORM_CORE
};

enum ssr_states {
	SUB_STATE_IDLE = 0,
	SSR_ON_BT,
	BT_SSR_COMPLETED,
	SSR_ON_UWB,
	UWB_SSR_COMPLETED,
	REG_BT_PID,
	REG_UWB_PID,
};

enum plt_pwr_state {
	POWER_ON_BT = 0,
	POWER_OFF_BT,
	POWER_ON_UWB,
	POWER_OFF_UWB,
	POWER_ON_BT_RETENION,
	POWER_ON_UWB_RETENION,
	BT_ACCESS_REQ,
	UWB_ACCESS_REQ,
	BT_RELEASE_ACCESS,
	UWB_RELEASE_ACCESS,
	BT_MAX_PWR_STATE,
};

enum {
	PWR_WAITING_RSP = -2,
	PWR_RSP_RECV = 0,
	PWR_FAIL_RSP_RECV = -1,
	PWR_CLIENT_KILLED,
};

typedef enum {
	BT_HOST_REASON_DEFAULT_NONE  = 0x00,                         //INVALID REASON
	BT_HOST_REASON_SOC_CRASHED = 0x01,                           //SOC WAS CRASHED
	BT_HOST_REASON_SOC_CRASHED_DIAG_SSR = 0x02,                  //SOC CRASHED DIAG INITIATED SSR
	BT_HOST_REASON_INIT_FAILED = 0x03,                           //HOST INITIALIZATION FAILED
	BT_HOST_REASON_CLOSE_RCVD_DURING_INIT = 0x04,                //CLOSE RECEIVED FROM STACK DURING SOC INIT
	BT_HOST_REASON_ERROR_READING_DATA_FROM_UART = 0x05,          //ERROR READING DATA FROM UART
	BT_HOST_REASON_WRITE_FAIL_SPCL_BUFF_CRASH_SOC = 0x06,        //FAILED TO WRITE SPECIAL BYTES TO CRASH SOC
	BT_HOST_REASON_RX_THREAD_STUCK = 0x07,                       //RX THREAD STUCK
	BT_HOST_REASON_SSR_CMD_TIMEDOUT = 0x08,                      //SSR DUE TO CMD TIMED OUT
	BT_HOST_REASON_SSR_SPURIOUS_WAKEUP = 0x09,                   //SSR DUE TO SPURIOUS WAKE UP
	BT_HOST_REASON_SSR_INVALID_BYTES_RCVD = 0x0A,                //INVALID HCI CMD TYPE RECEIVED
	BT_HOST_REASON_SSR_RCVD_LARGE_PKT_FROM_SOC = 0x0B,           //SSR DUE TO LARGE PKT RECVIVED FROM SOC
	BT_HOST_REASON_SSR_UNABLE_TO_WAKEUP_SOC = 0x0C,              //UNABLE TO WAKE UP SOC
	BT_HOST_REASON_CMD_TIMEDOUT_SOC_WAIT_TIMEOUT = 0x0D,         //COMMAND TIMEOUT AND SOC CRASH WAIT TIMEOUT
	BT_HOST_REASON_SPURIOUS_WAKEUP_SOC_WAIT_TIMEOUT = 0x0E,      //SPURIOUS WAKE AND SOC CRASH WAIT TIMEOUT
	BT_HOST_REASON_INV_BYTES_SOC_WAIT_TIMEOUT = 0x0F,            //INVALID BYTES AND SOC CRASH WAIT TIMEOUT
	BT_HOST_REASON_SOC_WAKEUP_FAILED_SOC_WAIT_TIMEOUT = 0x10,    //SOC WAKEUP FAILURE AND SOC CRASH WAIT TIMEOUT
	BT_HOST_REASON_SOC_CRASHED_DIAG_SSR_SOC_WAIT_TIMEOUT = 0x11, //SOC CRASHED DIAG INITIATED SSR CRASH WAIT TIMEOUT
	BT_HOST_REASON_NONE_SOC_WAIT_TIMEOUT = 0x12,                 //INVALID FAILURE AND SOC CRASH WAIT TIMEOUT
	BT_HOST_REASON_SOC_DEINIT_STUCK = 0x13,                      //SOC DEINIT STUCK
	BT_HOST_REASON_SSR_INTERNAL_CMD_TIMEDOUT = 0x14,             //SSR DUE TO CMD INTERNAL TIMED OUT
	BT_HOST_REASON_FAILED_TO_SEND_INTERNAL_CMD = 0x15,           //FAILED TO SEND INTERNAL CMD
	BT_HOST_REASON_SSR_SLEEP_IND_NOT_RCVD = 0x16,                //SOC DID NOT RCVD SLEEP IND DURING CLOSE
	BT_HOST_REASON_DIAG_LOG_API_STUCK = 0x39,                    //DIAG log API stuck.
	BT_HOST_REASON_PERI_SOC_CRASHED_DIAG_SSR = 0x17,             //PERI SOC CRASHED DIAG INITIATED SSR
	BT_HOST_REASON_PERI_SOC_CRASHED = 0X18,                      //PERI SOC WAS CRASHED
	BT_HOST_REASON_PERI_SOC_CRASHED_ON_OTHER_SUB_SYSTEM = 0x3A,  //Peripheral core crash detected in UWB SS
} PrimaryReasonCode;

typedef enum {
	BT_SOC_REASON_DEFAULT        =  0x00,
	BT_SOC_REASON_UNKNOWN        =  0x81,
	BT_SOC_REASON_SW_REQUESTED   =  0x82,
	BT_SOC_REASON_STACK_OVERFLOW =  0x83,
	BT_SOC_REASON_EXCEPTION      =  0x84,
	BT_SOC_REASON_ASSERT         =  0x85,
	BT_SOC_REASON_TRAP           =  0x86,
	BT_SOC_REASON_OS_FATAL       =  0x87,
	BT_SOC_REASON_HCI_RESET      =  0x88,
	BT_SOC_REASON_PATCH_RESET    =  0x89,
	BT_SOC_REASON_ABT            =  0x8A,
	BT_SOC_REASON_RAMMASK        =  0x8B,
	BT_SOC_REASON_PREBARK        =  0x8C,
	BT_SOC_REASON_BUSERROR       =  0x8D,
	BT_SOC_REASON_IO_FATAL       =  0x8E,
	BT_SOC_REASON_SSR_CMD        =  0x8F,
	BT_SOC_REASON_POWERON        =  0x90,
	BT_SOC_REASON_WATCHDOG       =  0x91,
	BT_SOC_REASON_RAMMASK_RGN1   =  0x92,
	BT_SOC_REASON_RAMMASK_RGN0   =  0x93,
	BT_SOC_REASON_Q6_WATCHDOG    =  0x94,
	BT_SOC_REASON_ZEALIS_RAM_MASK_RGN0 =  0x95,
	BT_SOC_REASON_ZEALIS_RAM_MASK_RGN1 =  0x96,
	BT_SOC_REASON_APSS_RESET           =  0x97,
	BT_SOC_REASON_TIME_RESET           =  0x98,
	BT_SOC_REASON_AUDIOSS_RESET        =  0x99,
	BT_SOC_REASON_HOST_WARMRESET       =  0x9A,
	BT_SOC_REASON_HOST_NMI_INIT        =  0x9B,
	BT_SOC_REASON_PANIC_FAULT          =  0x9C,
	BT_SOC_REASON_EARLY_TRAP           =  0x9D,
	BT_SOC_REASON_INSTR_ADDR_MISALIGN  =  0x9E,
	BT_SOC_REASON_INSTR_ACCESS_FAULT   =  0x9F,
	BT_SOC_REASON_ILLEGAL_INSTR        =  0xA0,
	BT_SOC_REASON_BREAKPOINT_EXCEPTION =  0xA1,
	BT_SOC_REASON_LOAD_ADDR_MISALIGN   =  0xA2,
	BT_SOC_REASON_LOAD_ACCESS_FAULT    =  0xA3,
	BT_SOC_REASON_STORE_ADDR_MISALIGN  =  0xA4,
	BT_SOC_REASON_STORE_ACCESS_FAULT   =  0xA5,
	BT_SOC_REASON_ECALL_UMODE          =  0xA6,
	BT_SOC_REASON_ECALL_MMODE          =  0xA7,
	BT_SOC_REASON_STACK_UNDERFLOW      =  0xA8,
	BT_SOC_REASON_MACHINE_EXT_INT      =  0xA9,
	BT_SOC_REASON_PERF_MONITOR_OVERFLOW=  0xAA,
	BT_SOC_REASON_EXT_SUBSYS_RESET     =  0xAB,
	BT_SOC_REASON_IPC_STALL            =  0xAC,
	BT_SOC_REASON_PEER_CPU0_NMI        =  0xAD,
	BT_SOC_REASON_PEER_CPU1_NMI        =  0xAE,
	BT_SOC_REASON_PEER_CPU2_NMI        =  0xAF,
	BT_SOC_REASON_TX_RX_INVALID_PKT_FATAL = 0xC0,
	BT_SOC_REASON_TX_RX_INVALID_LEN_FATAL = 0xC1,
	BT_SOC_REASON_INVALID_STACK          =  0xF0,
	BT_SOC_REASON_INVALID_MCI_MSG_RCVD   =  0xF1,
	BT_HOST_REASON_UARTINIT_STUCK        =  0x17,
	BT_HOST_REASON_GETVER_SEND_STUCK     =  0x18,
	BT_HOST_REASON_GETVER_NO_RSP_RCVD    =  0x19,
	BT_HOST_REASON_SETBAUDRATE_CMD_STUCK =  0x1A,
	BT_HOST_REASON_PATCH_DNLD_STUCK      =  0x1B,
	BT_HOST_REASON_GETBOARDID_CMD_STUCK  =  0x1C,
	BT_HOST_REASON_NVM_DNLD_STUCK        =  0x1D,
	BT_HOST_REASON_HCI_RESET_STUCK       =  0x1E,
	BT_HOST_REASON_GETBLDINFO_CMD_STUCK  =  0x1F,
	BT_HOST_REASON_ADDONFEAT_CMD_STUCK   =  0x20,
	BT_HOST_REASON_ENHLOG_CMD_STUCK      =  0x21,
	BT_HOST_REASON_DIAGINIT_STUCK        =  0x22,
	BT_HOST_REASON_DIAGDEINIT_STUCK      =  0x23,
	BT_HOST_REASON_XMEM_NVM_DNLD_STUCK   =  0x24,
	BT_HOST_REASON_XMEM_PATCH_DNLD_STUCK =  0x25,
	BT_HOST_REASON_SECURE_BRIDGE_CMD_STUCK = 0x26,
	BT_HOST_REASON_FAILED_TO_SEND_CMD              =  0x27,
	BT_HOST_REASON_HCI_RESET_CC_NOT_RCVD           =  0x28,
	BT_HOST_REASON_HCI_PRE_SHUTDOWN_CC_NOT_RCVD    =  0x29,
	BT_HOST_REASON_HCI_SET_BD_ADDRESS_CC_NOT_RCVD  =  0x2A,
	BT_HOST_REASON_FAILED_TO_RECEIVE_SLEEP_IND     =  0x2B,
	BT_HOST_REASON_POWER_ON_REGS_STUCK             =  0x2C,
	BT_HOST_REASON_RX_THREAD_START_STUCK           =  0x2D,
	BT_HOST_REASON_GET_LOCALADDR_STUCK             =  0x2E,
	BT_HOST_REASON_OTP_INFO_GET_CMD_STUCK          =  0x2F,
	BT_HOST_REASON_FILE_SYSTEM_CALL_STUCK          =  0x30,
	BT_HOST_REASON_PROPERTY_GET_STUCK              =  0x31,
	BT_HOST_REASON_PROPERTY_SET_STUCK              =  0x32,
	BT_HOST_REASON_RAM_PATCH_READ_STUCK            =  0x33,
	BT_HOST_REASON_NVM_PATCH_READ_STUCK            =  0x34,
	BT_HOST_REASON_UART_IOCTL_STUCK                =  0x35,
	BT_HOST_REASON_POWER_IOCTL_STUCK               =  0x36,
	BT_HOST_REASON_PATCH_CONFIG_CMD_STUCK          =  0x37,
	BT_HOST_REASON_GET_APP_VER_CMD_STUCK           =  0x38,
	BT_HOST_REASON_SOC_NAME_UNKOWN                 =  0x3A,
	SOC_REASON_START_TX_IOS_SOC_RFR_HIGH_DURING_INIT =  0x3B,
	BT_HOST_REASON_GETVER_CMD_FAILED               = 0x3C,
	BT_HOST_REASON_BAUDRATE_CHANGE_FAILED          = 0x3D,
	BT_HOST_REASON_TLV_DOWNLOAD_FAILED             = 0x3E,
	BT_HOST_REASON_FW_BUILD_INFO_CMD_FAILED        = 0x3F,
	BT_HOST_REASON_HCI_RESET_CMD_FAILED            = 0x40,
	BT_HOST_REASON_UART_INIT_FAILED                = 0x41,
	BT_HOST_REASON_MEMORY_ALLOCATION_FAILED        = 0x42,
	BT_HOST_REASON_READ_THREAD_START_FAILED        = 0x43,
	BT_HOST_REASON_HW_FLOW_ON_FAILED               = 0x44,
	BT_HOST_REASON_NVM_FILE_NOT_FOUND              = 0x45,
	BT_HOST_REASON_UART_BAUDRATE_CHANGE_FAILED     = 0x46,
	BT_HOST_REASON_PATCH_CONFIG_FAILED             = 0x47,
	BT_HOST_REASON_HCI_SET_OFFLOAD_HOST_CONFIG_CC_NOT_RCVD = 0x48,
	BT_HOST_REASON_BT_EN_PIN_LOW                   = 0x49,
	UART_REASON_DEFAULT                            =  0x51,
	UART_REASON_INVALID_FW_LOADED                  =  0x52,
	UART_REASON_CLK_GET_FAIL                       =  0x53,
	UART_REASON_SE_CLK_RATE_FIND_FAIL              =  0x54,
	UART_REASON_SE_RESOURCES_INIT_FAIL             =  0x55,
	UART_REASON_SE_RESOURCES_ON_FAIL               =  0x56,
	UART_REASON_SE_RESOURCES_OFF_FAIL              =  0x57,
	UART_REASON_TX_DMA_MAP_FAIL                    =  0x58,
	UART_REASON_TX_CANCEL_FAIL                     =  0x59,
	UART_REASON_TX_ABORT_FAIL                      =  0x5A,
	UART_REASON_TX_FSM_RESET_FAIL                  =  0x5B,
	UART_REASON_RX_CANCEL_FAIL                     =  0x5C,
	UART_REASON_RX_ABORT_FAIL                      =  0x5D,
	UART_REASON_RX_FSM_RESET_FAIL                  =  0x5E,
	UART_REASON_RX_TTY_INSET_FAIL                  =  0x5F,
	UART_REASON_ILLEGAL_INTERRUPT                  =  0x60,
	UART_REASON_BUFFER_OVERRUN                     =  0x61,
	UART_REASON_RX_PARITY_REASON                   =  0x62,
	UART_REASON_RX_BREAK_REASON                    =  0x63,
	UART_REASON_RX_SBE_REASON                      =  0x64,
	SOC_REASON_START_TX_IOS_SOC_RFR_HIGH           =  0x65,
	UART_REASON_FLOW_OFF                           =  0x66,
	BT_HOST_REASON_PERI_ARBITRATION_CMD_STUCK =  0x67,
	BT_HOST_REASON_PERI_ARBITRATION_NTF_STUCK =  0x68,
	BT_HOST_REASON_BT_ACTIVATE_NTF_STUCK      =  0x69,
	BT_HOST_REASON_PERI_GETVER_SEND_STUCK     =  0x70,
	BT_HOST_REASON_PERI_GETBLDINFO_CMD_STUCK  =  0x71,
	BT_HOST_REASON_PERI_RAM_PATCH_READ_STUCK  =  0x72,
	BT_HOST_REASON_PERI_RESET_STUCK           =  0x73,
	BT_HOST_REASON_PERI_PATCH_CONFIG_CMD_STUCK=  0x74,
	BT_HOST_REASON_PERI_PATCH_DNLD_STUCK      =  0x75,
	BT_HOST_REASON_PERI_RESET_CMD_FAILED      =  0x76,
	BT_HOST_REASON_BT_ACTIVATE_CMD_STUCK      =  0x4F,
	BT_HOST_REASON_BT_ACTIVATE_CMD_FAILED     =  0x77,
	BT_HOST_REASON_PERI_GETVER_CMD_FAILED     =  0x78,
	BT_HOST_REASON_PERI_PATCH_CONFIG_FAILED   =  0x79,
	BT_HOST_REASON_PERI_TLV_DOWNLOAD_FAILED   =  0xB0,
	BT_HOST_REASON_PERI_GETBOARDID_CMD_STUCK  =  0xB1,
	BT_HOST_REASON_PERI_NVM_PATCH_READ_STUCK  =  0xB2,
	BT_HOST_REASON_PERI_NVM_FILE_NOT_FOUND    =  0xB3,
	BT_HOST_REASON_PERI_NVM_DNLD_STUCK        =  0XB4,
	BT_HOST_REASON_PERI_GETBLDINFO_CMD_FAILED =  0XB5,
	BT_HOST_REASON_PERI_GETVER_NO_RSP_RCVD    =  0XB6,
	BT_HOST_REASON_PERI_ARB_NOTIFY_FAILED     =  0xB7,
	BT_HOST_REASON_PERI_SETBAUDRATE_CMD_STUCK =  0xB8,
	BT_HOST_REASON_PERI_SETBAUD_CMD_FAILED    =  0xB9,
	BT_HOST_REASON_MEMORY_ALLOCATION_FAILED_PERI = 0xBA,
	BT_HOST_REASON_HCI_ACTIVATE_CC_NOT_RCVD   =  0xBB,
	BT_HOST_REASON_PERI_FILE_SYSTEM_CALL_STUCK = 0xBC,
	BT_HOST_REASON_PERI_POWER_IOCTL_STUCK     = 0xBD,
	BT_HOST_REASON_PERI_SETBAUD_CC_NOT_RCVD   =  0xBE,
	PERI_SOC_REASON_DEFAULT                   =  0xBF
} SecondaryReasonCode;

typedef struct {
  SecondaryReasonCode reason;
  char reasonstr[50];
} SecondaryReasonMap;

typedef struct {
  PrimaryReasonCode reason;
  char reasonstr[100];
} PrimaryReasonMap;

static PrimaryReasonMap priReasonMap[] = {
	{ BT_HOST_REASON_DEFAULT_NONE,                         "Invalid reason"},
	{ BT_HOST_REASON_SOC_CRASHED,                          "SOC crashed"},
	{ BT_HOST_REASON_SOC_CRASHED_DIAG_SSR,                 "SOC crashed with diag initiated SSR"},
	{ BT_HOST_REASON_INIT_FAILED,                          "Init failed"},
	{ BT_HOST_REASON_CLOSE_RCVD_DURING_INIT,               "Close received from stack during SOC init"},
	{ BT_HOST_REASON_ERROR_READING_DATA_FROM_UART,         "Error reading data from UART"},
	{ BT_HOST_REASON_WRITE_FAIL_SPCL_BUFF_CRASH_SOC,       "Failed to write special bytes to crash SOC"},
	{ BT_HOST_REASON_RX_THREAD_STUCK,                      "Thread Stuck"},
	{ BT_HOST_REASON_SSR_CMD_TIMEDOUT,                     "SSR due to command timed out"},
	{ BT_HOST_REASON_SSR_SPURIOUS_WAKEUP,                  "SSR due to spurious wakeup"},
	{ BT_HOST_REASON_SSR_INVALID_BYTES_RCVD,               "Invalid HCI cmd type received"},
	{ BT_HOST_REASON_SSR_RCVD_LARGE_PKT_FROM_SOC,          "Large packet received from SOC"},
	{ BT_HOST_REASON_SSR_UNABLE_TO_WAKEUP_SOC,             "Unable to wake SOC"},
	{ BT_HOST_REASON_CMD_TIMEDOUT_SOC_WAIT_TIMEOUT,        "Command timedout and SOC crash wait timeout"},
	{ BT_HOST_REASON_SPURIOUS_WAKEUP_SOC_WAIT_TIMEOUT,     "Spurious wake and SOC crash wait timeout"},
	{ BT_HOST_REASON_INV_BYTES_SOC_WAIT_TIMEOUT,           "Invalid bytes received and SOC crash wait timeout"},
	{ BT_HOST_REASON_SOC_WAKEUP_FAILED_SOC_WAIT_TIMEOUT,   "SOC Wakeup failed and SOC crash wait timeout"},
	{ BT_HOST_REASON_SOC_CRASHED_DIAG_SSR_SOC_WAIT_TIMEOUT,"SOC crashed with diag initiated SSR and SOC wait timeout"},
	{ BT_HOST_REASON_NONE_SOC_WAIT_TIMEOUT,                "Invalid Reason and SOC crash wait timeout"},
	{ BT_HOST_REASON_SOC_DEINIT_STUCK,                     "SOC Deinit Stuck"},
	{ BT_HOST_REASON_SSR_INTERNAL_CMD_TIMEDOUT,            "SSR due to internal Command timeout"},
	{ BT_HOST_REASON_FAILED_TO_SEND_INTERNAL_CMD,          "Failed to send internal command"},
	{ BT_HOST_REASON_SSR_SLEEP_IND_NOT_RCVD,               "Failed to receive SLEEP IND during close"},
	{ BT_HOST_REASON_DIAG_LOG_API_STUCK,                   "DIAG log API stuck"},
	{ BT_HOST_REASON_PERI_SOC_CRASHED,                     "Peri SOC crashed"},
	{ BT_HOST_REASON_PERI_SOC_CRASHED_DIAG_SSR,            "Peri SOC crashed with diag initiated SSR"},
	{ BT_HOST_REASON_PERI_SOC_CRASHED_ON_OTHER_SUB_SYSTEM, "Peripheral core crash deteced in UWB SS"},
};

static SecondaryReasonMap secReasonMap[] = {
	{ BT_SOC_REASON_DEFAULT                 ,    "Default"},
	{ BT_SOC_REASON_UNKNOWN                 ,    "Unknown"},
	{ BT_SOC_REASON_TX_RX_INVALID_PKT_FATAL ,    "Tx/Rx invalid packet fatal error"},
	{ BT_SOC_REASON_TX_RX_INVALID_LEN_FATAL ,    "Tx/Rx invalid length fatal error"},
	{ BT_SOC_REASON_SW_REQUESTED            ,    "SW Requested"},
	{ BT_SOC_REASON_STACK_OVERFLOW          ,    "Stack Overflow"},
	{ BT_SOC_REASON_EXCEPTION               ,    "Exception"},
	{ BT_SOC_REASON_ASSERT                  ,    "Assert"},
	{ BT_SOC_REASON_TRAP                    ,    "Trap"},
	{ BT_SOC_REASON_OS_FATAL                ,    "OS Fatal"},
	{ BT_SOC_REASON_HCI_RESET               ,    "HCI Reset"},
	{ BT_SOC_REASON_PATCH_RESET             ,    "Patch Reset"},
	{ BT_SOC_REASON_ABT                     ,    "SoC Abort"},
	{ BT_SOC_REASON_RAMMASK                 ,    "RAM MASK"},
	{ BT_SOC_REASON_PREBARK                 ,    "PREBARK"},
	{ BT_SOC_REASON_BUSERROR                ,    "Bus error"},
	{ BT_SOC_REASON_IO_FATAL                ,    "IO fatal eror"},
	{ BT_SOC_REASON_SSR_CMD                 ,    "SSR CMD"},
	{ BT_SOC_REASON_POWERON                 ,    "Power ON"},
	{ BT_SOC_REASON_WATCHDOG                ,    "Watchdog"},
	{ BT_SOC_REASON_RAMMASK_RGN1            ,    "RAMMASK RGN1"},
	{ BT_SOC_REASON_RAMMASK_RGN0            ,    "RAMMASK RGN0"},
	{ BT_SOC_REASON_Q6_WATCHDOG             ,    "Q6 Watchdog"},
	{ BT_SOC_REASON_ZEALIS_RAM_MASK_RGN0    ,    "ZEALIS RAM MASK RGN0"},
	{ BT_SOC_REASON_ZEALIS_RAM_MASK_RGN1    ,    "ZEALIS RAM MASK RGN1"},
	{ BT_SOC_REASON_APSS_RESET              ,    "APSS reset"},
	{ BT_SOC_REASON_TIME_RESET              ,    "Time reset"},
	{ BT_SOC_REASON_AUDIOSS_RESET           ,    "Audioss reset"},
	{ BT_SOC_REASON_HOST_WARMRESET          ,    "Host warm reset"},
	{ BT_SOC_REASON_HOST_NMI_INIT           ,    "Host NMI init"},
	{ BT_SOC_REASON_INVALID_STACK           ,    "Invalid Stack"},
	{ BT_HOST_REASON_UARTINIT_STUCK              ,    "UartInitStuck"},
	{ BT_HOST_REASON_GETVER_SEND_STUCK           ,    "GetVerSendStuck"},
	{ BT_HOST_REASON_GETVER_NO_RSP_RCVD          ,    "GetVerNoRspRcvd"},
	{ BT_HOST_REASON_SETBAUDRATE_CMD_STUCK       ,    "SetBaudRateStuck"},
	{ BT_HOST_REASON_PATCH_DNLD_STUCK            ,    "PatchDnldStuck"},
	{ BT_HOST_REASON_GETBOARDID_CMD_STUCK        ,    "GetBoardIdStuck"},
	{ BT_HOST_REASON_NVM_DNLD_STUCK              ,    "NvmDnldStuck"},
	{ BT_HOST_REASON_HCI_RESET_STUCK             ,    "HciResetStuck"},
	{ BT_HOST_REASON_GETBLDINFO_CMD_STUCK        ,    "GetBldInfoCmdStuck"},
	{ BT_HOST_REASON_ADDONFEAT_CMD_STUCK         ,    "AddOnFeatCmdStuck"},
	{ BT_HOST_REASON_ENHLOG_CMD_STUCK            ,    "EnhLogCmdStuck"},
	{ BT_HOST_REASON_DIAGINIT_STUCK              ,    "DiagInitStuck"},
	{ BT_HOST_REASON_DIAGDEINIT_STUCK            ,    "DiagDeinitStuck"},
	{ BT_HOST_REASON_XMEM_NVM_DNLD_STUCK         ,    "XMEM NVM Download stuck"},
	{ BT_HOST_REASON_XMEM_PATCH_DNLD_STUCK       ,    "XMEM patch download stuck"},
	{ BT_HOST_REASON_SECURE_BRIDGE_CMD_STUCK     ,    "Secure bridge cmd stuck"},
	{ BT_HOST_REASON_FAILED_TO_SEND_CMD          ,    "Failed to send internal cmd"},
	{ BT_HOST_REASON_HCI_RESET_CC_NOT_RCVD       ,    "HCI Reset Cmd CC Not Rcvd"},
	{ BT_HOST_REASON_HCI_PRE_SHUTDOWN_CC_NOT_RCVD    ,  "HCI Pre shutdown Cmd CC not Rcvd"},
	{ BT_HOST_REASON_HCI_SET_BD_ADDRESS_CC_NOT_RCVD  ,  "HCI BD address CC not Rcvd"},
	{ BT_HOST_REASON_FAILED_TO_RECEIVE_SLEEP_IND     ,  "Failed to receive SLEEP IND from SoC"},
	{ BT_HOST_REASON_POWER_ON_REGS_STUCK             ,  "SoC Power ON Sequence stuck"},
	{ BT_HOST_REASON_POWER_IOCTL_STUCK               ,  "Power driver IOCTL stuck"},
	{ BT_HOST_REASON_RX_THREAD_START_STUCK           ,  "RX thread start stuck"},
	{ BT_HOST_REASON_GET_LOCALADDR_STUCK             ,  "Get local BD address stuck"},
	{ BT_HOST_REASON_OTP_INFO_GET_CMD_STUCK          ,  "Get OTP info. cmd stuck"},
	{ BT_HOST_REASON_FILE_SYSTEM_CALL_STUCK          ,  "FILE system call stuck"},
	{ BT_HOST_REASON_PROPERTY_GET_STUCK              ,  "Property get call stuck"},
	{ BT_HOST_REASON_PROPERTY_SET_STUCK              ,  "Property set call stuck"},
	{ BT_HOST_REASON_RAM_PATCH_READ_STUCK            ,  "RAM patch open/read stuck"},
	{ BT_HOST_REASON_NVM_PATCH_READ_STUCK            ,  "NVM file open/read stuck"},
	{ BT_HOST_REASON_UART_IOCTL_STUCK                ,  "UART IOCTL stuck"},
	{ BT_HOST_REASON_PATCH_CONFIG_CMD_STUCK          ,  "Patch config cmd stuck"},
	{ BT_HOST_REASON_GET_APP_VER_CMD_STUCK           ,  "Get APP version cmd stuck"},
	{ BT_HOST_REASON_SOC_NAME_UNKOWN                 ,  "SoC name unkown"},
	{ SOC_REASON_START_TX_IOS_SOC_RFR_HIGH_DURING_INIT, "SoC RFR high during INIT"},
	{ BT_HOST_REASON_GETVER_CMD_FAILED               ,  "Get Version cmd failed"},
	{ BT_HOST_REASON_BAUDRATE_CHANGE_FAILED          ,  "Baudrate change failed"},
	{ BT_HOST_REASON_TLV_DOWNLOAD_FAILED             ,  "TLV/NVM download failed"},
	{ BT_HOST_REASON_FW_BUILD_INFO_CMD_FAILED        ,  "FW build info. cmd failed"},
	{ BT_HOST_REASON_HCI_RESET_CMD_FAILED            ,  "HCI RESET cmd failed"},
	{ BT_HOST_REASON_UART_INIT_FAILED                ,  "UART INIT failed"},
	{ BT_HOST_REASON_MEMORY_ALLOCATION_FAILED        ,  "Memory allocation failed"},
	{ BT_HOST_REASON_READ_THREAD_START_FAILED        ,  "Read thread start failed"},
	{ BT_HOST_REASON_HW_FLOW_ON_FAILED               ,  "HW Flow ON failed"},
	{ BT_HOST_REASON_NVM_FILE_NOT_FOUND              ,  "NVM file not found"},
	{ BT_HOST_REASON_UART_BAUDRATE_CHANGE_FAILED     ,  "UART baudrate change failed"},
	{ BT_HOST_REASON_PATCH_CONFIG_FAILED             ,  "Patch config cmd failed"},
	{ BT_HOST_REASON_HCI_SET_OFFLOAD_HOST_CONFIG_CC_NOT_RCVD ,  "offload host config CC not Rcvd"},
	{ BT_HOST_REASON_BT_EN_PIN_LOW                   ,  "BT EN pin pulled low"},
	{ UART_REASON_DEFAULT                            ,  "UART reason Default"},
	{ UART_REASON_INVALID_FW_LOADED                  ,  "UART invalid FW loaded"},
	{ UART_REASON_CLK_GET_FAIL                       ,  "UART CLK get failed"},
	{ UART_REASON_SE_CLK_RATE_FIND_FAIL              ,  "UART CLK rate find failed"},
	{ UART_REASON_SE_RESOURCES_INIT_FAIL             ,  "UART resources init failed"},
	{ UART_REASON_SE_RESOURCES_ON_FAIL               ,  "UART resources ON failed"},
	{ UART_REASON_SE_RESOURCES_OFF_FAIL              ,  "UART resources OFF failed"},
	{ UART_REASON_TX_DMA_MAP_FAIL                    ,  "UART DMA map failed"},
	{ UART_REASON_TX_CANCEL_FAIL                     ,  "UART tx cancel failed"},
	{ UART_REASON_TX_ABORT_FAIL                      ,  "UART tx abort failed"},
	{ UART_REASON_TX_FSM_RESET_FAIL                  ,  "UART tx FSM reset failed"},
	{ UART_REASON_RX_CANCEL_FAIL                     ,  "UART rx cancel failed"},
	{ UART_REASON_RX_ABORT_FAIL                      ,  "UART rx abort failed"},
	{ UART_REASON_RX_FSM_RESET_FAIL                  ,  "UART rx FSM reset failed"},
	{ UART_REASON_RX_TTY_INSET_FAIL                  ,  "UART rx tty inset failed"},
	{ UART_REASON_ILLEGAL_INTERRUPT                  ,  "UART illegal interrupt"},
	{ UART_REASON_BUFFER_OVERRUN                     ,  "UART buffer overrun"},
	{ UART_REASON_RX_PARITY_REASON                   ,  "UART rx parity error"},
	{ UART_REASON_RX_BREAK_REASON                    ,  "UART rx break error"},
	{ UART_REASON_RX_SBE_REASON                      ,  "UART rx SBE error"},
	{ SOC_REASON_START_TX_IOS_SOC_RFR_HIGH           ,  "SoC RFR high"},
	{ UART_REASON_FLOW_OFF                           ,  "UART flow is OFF"},
	{ BT_HOST_REASON_PERI_ARBITRATION_CMD_STUCK      ,  "Peri arbitration cmd stuck"},
	{ BT_HOST_REASON_PERI_GETBLDINFO_CMD_STUCK       ,  "Peri Get Bld Info CmdStuck"},
	{ BT_HOST_REASON_BT_ACTIVATE_CMD_STUCK           ,  "Subsystem Activate cmd stuck"},
	{ BT_HOST_REASON_BT_ACTIVATE_NTF_STUCK           ,  "Subsystem Activate ntf stuck"},
	{ BT_HOST_REASON_PERI_GETVER_SEND_STUCK          ,  "Peri Get Ver Send Stuck"},
	{ BT_HOST_REASON_PERI_GETBLDINFO_CMD_STUCK       ,  "Peri Get Bld Info Cmd Stuck"},
	{ BT_HOST_REASON_PERI_RAM_PATCH_READ_STUCK       ,  "Peri RAM patch open/read stuck"},
	{ BT_HOST_REASON_PERI_RESET_STUCK                ,  "Peri Reset Stuck"},
	{ BT_HOST_REASON_PERI_PATCH_CONFIG_CMD_STUCK     ,  "Peri Patch config cmd stuck"},
	{ BT_HOST_REASON_PERI_PATCH_DNLD_STUCK           ,  "Peri Patch Dnld Stuck"},
	{ BT_HOST_REASON_PERI_RESET_CMD_FAILED           ,  "HCI Peri RESET cmd failed"},
	{ BT_HOST_REASON_BT_ACTIVATE_CMD_STUCK           ,  "BT activate cmd stuck"},
	{ BT_HOST_REASON_BT_ACTIVATE_CMD_FAILED          ,  "BT activate cmd failed"},
	{ BT_HOST_REASON_PERI_GETVER_CMD_FAILED          ,  "Peri Get Ver cmd failed"},
	{ BT_HOST_REASON_PERI_PATCH_CONFIG_FAILED        ,  "Peri Patch config cmd failed"},
	{ BT_HOST_REASON_PERI_TLV_DOWNLOAD_FAILED        ,  "Peri TLV/NVM download failed"},
	{ BT_HOST_REASON_PERI_GETBOARDID_CMD_STUCK       ,  "Peri Get BoardId Stuck"},
	{ BT_HOST_REASON_PERI_NVM_PATCH_READ_STUCK       ,  "Peri NVM file open/read stuck"},
	{ BT_HOST_REASON_PERI_NVM_DNLD_STUCK             ,  "Peri Nvm Dnld Stuck"},
	{ BT_HOST_REASON_PERI_GETBLDINFO_CMD_FAILED      ,  "Peri FW build info. cmd failed"},
	{ BT_HOST_REASON_PERI_GETVER_NO_RSP_RCVD         ,  "Peri Get Ver No Rsp Rcvd"},
	{ BT_HOST_REASON_PERI_SETBAUDRATE_CMD_STUCK      ,  "Peri Set baud rate stuck"},
	{ BT_HOST_REASON_PERI_SETBAUD_CC_NOT_RCVD        ,  "Peri set baudrate cc nor rcvd"},
	{ PERI_SOC_REASON_DEFAULT                        ,  "Default"},
};

struct log_index {
	int init;
	int crash;
};

struct vreg_data {
	struct regulator *reg;  /* voltage regulator handle */
	const char *name;       /* regulator name */
	u32 min_vol;            /* min voltage level */
	u32 max_vol;            /* max voltage level */
	u32 load_curr;          /* current */
	bool is_enabled;        /* is this regulator enabled? */
	bool is_retention_supp; /* does this regulator support retention mode */
	struct log_index indx;  /* Index for reg. w.r.t init & crash */
};

struct pwr_data {
	char compatible[32];
	struct vreg_data *bt_vregs;
	int bt_num_vregs;
	struct vreg_data *uwb_vregs;
	int uwb_num_vregs;
	struct vreg_data *platform_vregs;
	int platform_num_vregs;
};

struct bt_power_clk_data {
	struct clk *clk;  /* clock regulator handle */
	const char *name; /* clock name */
	bool is_enabled;  /* is this clock enabled? */
};

struct btpower_state_machine {
	struct mutex state_machine_lock;
	enum power_states power_state;
	enum retention_states retention_mode;
	enum grant_states grant_state;
	enum grant_states grant_pending;
};

#define BTPWR_MAX_REQ         BT_MAX_PWR_STATE

/*
 * Platform data for the bluetooth power driver.
 */
struct platform_pwr_data {
	struct platform_device *pdev;
	int bt_gpio_sys_rst;                   /* Bluetooth reset gpio */
	int wl_gpio_sys_rst;                   /* Wlan reset gpio */
	int bt_gpio_sw_ctrl;                   /* Bluetooth sw_ctrl gpio */
	int bt_gpio_debug;                     /* Bluetooth debug gpio */
	unsigned int wlan_sw_ctrl_gpio;        /* Wlan switch control gpio*/
#ifdef CONFIG_MSM_BT_OOBS
	int bt_gpio_dev_wake;                  /* Bluetooth bt_wake */
	int bt_gpio_host_wake;                 /* Bluetooth bt_host_wake */
	int irq;                               /* Bluetooth host_wake IRQ */
#endif
	int sw_cntrl_gpio;
	int xo_gpio_clk;                       /* XO clock gpio*/
	struct device *slim_dev;
	struct vreg_data *bt_vregs;
	struct vreg_data *uwb_vregs;
	struct vreg_data *platform_vregs;
	struct bt_power_clk_data *bt_chip_clk; /* bluetooth reference clock */
	int (*power_setup)(int core, int id); /* Bluetooth power setup function */
	char compatible[32]; /*Bluetooth SoC name */
	int bt_num_vregs;
	int uwb_num_vregs;
	int platform_num_vregs;
	struct mbox_client mbox_client_data;
	struct mbox_chan *mbox_chan;
	const char *vreg_ipa;
	bool is_ganges_dt;
	int pdc_init_table_len;
	const char **pdc_init_table;
	int bt_device_type;
	bool sec_peri_feature_disable;
	int bt_sec_hw_disable;
#ifdef CONFIG_MSM_BT_OOBS
	struct file *reffilp_obs;
	struct task_struct *reftask_obs;
#endif
	struct task_struct *reftask;
	struct task_struct *reftask_bt;
	struct task_struct *reftask_uwb;
	struct btpower_state_machine btpower_state;
	enum ssr_states sub_state;
	enum ssr_states wrkq_signal_state;
	struct workqueue_struct *workq;
	struct device_node *bt_of_node;
	struct device_node *uwb_of_node;
	struct work_struct bt_wq;
	struct work_struct uwb_wq;
	wait_queue_head_t rsp_wait_q[BTPWR_MAX_REQ];
	int wait_status[BTPWR_MAX_REQ];
	struct work_struct wq_pwr_voting;
	struct sk_buff_head rxq;
	struct mutex pwr_mtx;
};

int btpower_register_slimdev(struct device *dev);
int btpower_get_chipset_version(void);
int btpower_aop_mbox_init(struct platform_pwr_data *pdata);
int bt_aop_pdc_reconfig(struct platform_pwr_data *pdata);

static const char *pwr_req[] = {"POWER_ON_BT", "POWER_OFF_BT",
								"POWER_ON_UWB", "POWER_OFF_UWB",
								"POWER_ON_BT_RETENION",
								"POWER_ON_UWB_RETENION",
								"BT_ACCESS_REQ", "UWB_ACCESS_REQ",
								"BT_RELEASE_ACCESS", "UWB_RELEASE_ACCESS"};

#define WLAN_SW_CTRL_GPIO       "qcom,wlan-sw-ctrl-gpio"
#define BT_CMD_SLIM_TEST            0xbfac
#define BT_CMD_PWR_CTRL             0xbfad
#define BT_CMD_CHIPSET_VERS         0xbfae
#define BT_CMD_GET_CHIPSET_ID       0xbfaf
#define BT_CMD_CHECK_SW_CTRL        0xbfb0
#define BT_CMD_GETVAL_POWER_SRCS    0xbfb1
#define BT_CMD_SET_IPA_TCS_INFO     0xbfc0
#define BT_CMD_KERNEL_PANIC         0xbfc1
#define UWB_CMD_KERNEL_PANIC        0xbfc2
#define UWB_CMD_PWR_CTRL            0xbfe1
#define BT_CMD_REGISTRATION	        0xbfe2
#define UWB_CMD_REGISTRATION        0xbfe3
#define BT_CMD_ACCESS_CTRL          0xbfe4
#define UWB_CMD_ACCESS_CTRL         0xbfe5

#ifdef CONFIG_MSM_BT_OOBS
#define BT_CMD_OBS_VOTE_CLOCK		0xbfd1


/**
 * enum btpower_obs_param: OOBS low power param
 * @BTPOWER_OBS_CLK_OFF: Transport bus is no longer acquired
 * @BTPOWER_OBS_CLK_ON: Acquire transport bus for either transmitting or receiving
 * @BTPOWER_OBS_DEV_OFF: Bluetooth is released because of no more transmission
 * @BTPOWER_OBS_DEV_ON: Wake up the Bluetooth controller for transmission
 */
enum btpower_obs_param {
	BTPOWER_OBS_CLK_OFF = 0,
	BTPOWER_OBS_CLK_ON,
	BTPOWER_OBS_DEV_OFF,
	BTPOWER_OBS_DEV_ON,
};
#endif

static const char * const bt_arg[]= {"power off BT", "power on BT",
									"BT power retention"};
static const char * const uwb_arg[]= {"power off UWB", "power on UWB",
									"UWB power retention"};
static const char * const pwr_states[]= {"Both Sub-System powered OFF", "BT powered ON",
									"UWB powered ON",
					  "Both Sub-System powered ON"};
static const char * const ssr_state[]= {"No SSR on Sub-Sytem", "SSR on BT",
									"SSR Completed on BT", "SSR on UWB",
									"SSR Completed on UWB"};
static const char * const reg_mode[]= {"vote off", "vote on", "vote for retention", "vote off retention"};
static const char * const retention_mode[]= {"IDLE", "BT_IN_RETENTION", "BT_OUT_OF_RETENTION",
									"UWB_IN_RETENTION", "UWB_OUT_OF_RETENTION",
									"BOTH_CLIENT_IN_RETENTION"};
#endif /* __LINUX_BLUETOOTH_POWER_H */
