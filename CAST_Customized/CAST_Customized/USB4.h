// USB4.h : USB4 header file.

#ifndef USB4
#define USB4

/*************************************************************************
|    Definitions                                                         |
*************************************************************************/

#define MAX_ERROR_MSG_LENGTH		512
#define MAX_INC_COUNTS				0x1000000
#define FAILURE						FALSE		// must be zero
#define SUCCESS						TRUE
#define USB4_SUCCESS				0
#define USB4_MAX_ENCODERS			4		// max number of encoders per device.
#define USB4_MAX_DEVICES			255		// max devices on USB bus
#define USB4_MAX_ADDRESSES			255		// address can be any valid single byte value
#define USB4_MAX_ADC_CHANNELS		4		// max number of a/d channels.
#define USB4_FIFO_MAX_COUNT			196607  // max number of ChannelBufferRecords in FIFO.
#define USB4_RS232_BAUDBYTE_9600	0x00	// baud byte code used to set RS232 baud rate to 9600.
#define USB4_RS232_BAUDBYTE_19200	0x01	// baud byte code used to set RS232 baud rate to 19200.
#define USB4_RS232_BAUDBYTE_115200	0xFF	// baud byte code used to set RS232 baud rate to 115200.


//****** Error Codes ***************************/
#define DEVICE_NOT_OPEN					-1
#define FAILED_TO_AQUIRE_MUTEX			-2
#define FAILED_TO_DOWNLOAD_FIRMWARE		-3
#define FATAL_ERROR						-4
#define FIFO_BUFFER_EMPTY				-5
#define INVALID_A2D_CHANNEL				-6
#define INVALID_COUNTER_MODE			-7
#define INVALID_D2A_CHANNEL				-8
#define INVALID_D2A_MODE				-9
#define INVALID_DEVICE_NUMBER			-10
#define INVALID_ENCODER_NUMBER			-11
#define INVALID_MODULE_NUMBER			-12
#define INVALID_PARAMETER				-13
#define INVALID_QUADRATURE_MODE			-14
#define INVALID_REGISTER_NUMBER			-15
#define INVALID_SIGNAL_LENGTH_CODE		-16
#define MODULE_NUMBER_ALREADY_ASSIGNED	-17
#define MODULE_NUMBER_NOT_FOUND			-18
#define NO_AVAILABLE_MODULE_ADDRESSES	-19
#define USB4_INVALID_D2A_VALUE			-20
#define RX_232_FAILURE					-30
#define TX_232_FAILURE					-31
#define NO_DEVICES_FOUND				-32
#define OLD_FIRMWARE_DETECTED			-33


//****** REGISTERS *****************************/
#define PRESET_REGISTER						0
#define OUTPUT_LATCH_REGISTER				1
#define MATCH_REGISTER						2
#define CONTROL_REGISTER					3
#define STATUS_REGISTER						4
#define COUNTER_REGISTER					5  // Renamed from RESET_CHANNEL_REGISTER
#define RESET_CHANNEL_REGISTER				5  // Not currently used.
#define TRANSFER_PRESET_REGISTER			6
#define CMD_REGISTER						7
#define TIMESTAMP_OUTPUT_LATCH_REGISTER		15
#define TIMESTAMP_REGISTER					23
#define ADC01_TRIGGER_CONTROL_REGISTER		24
#define ADC23_TRIGGER_CONTROL_REGISTER		25
#define ENCODER_TYPE_REGISTER				26
#define INPUT_TRIGGER_CONTROL_REGISTER		27
#define INPUT_TRIGGER_STATUS_REGISTER		28
#define SAMPLING_RATE_MULTIPLIER_REGISTER   30
#define SAMPLING_RATE_COUNTER_REGISTER		31
#define USB_PWM0_TRIGGER_CONTROL_REGISTER	32
#define USB_PWM1_TRIGGER_CONTROL_REGISTER	33
#define USB_PWM2_TRIGGER_CONTROL_REGISTER	34
#define USB_PWM3_TRIGGER_CONTROL_REGISTER	35
#define FIFO_ON_OFF_REGISTER				37
#define FIFO_STATUS_CONTROL_REGISTER		38
#define FIFO_BUFFER_COUNT					39
#define INPUT_PORT_REGISTER					40
#define INPUT_TRIGGER1_SETUP_REGISTER		41
#define INPUT_TRIGGER2_SETUP_REGISTER		42
#define SAMPLES_TO_COLLECT_REGISTER			43
#define SAMPLES_REMAINING_TO_COLLECT_REGISTER	44
#define AQUISITION_CONTROL_REGISTER			45
#define OUTPUT_PORT_REGISTER				46
#define OUTPUT_PORT_SETUP_REGISTER			47
#define USB4_ENCODER3_BASE					48
#define USB4_A2D_BASE_REGISTER				55
#define USB4_A2D_CHANNEL0_REGISTER			55
#define USB4_A2D_CHANNEL1_REGISTER			56
#define USB4_A2D_CHANNEL2_REGISTER			57
#define USB4_A2D_CHANNEL3_REGISTER			58
#define USB4_D2A_CONTROL_REGISTER			59
#define USB4_PW0_REGISTER					60
#define USB4_PER0_REGISTER					61
#define USB4_PW1_REGISTER					62
#define USB4_PER1_REGISTER					63
#define USB4_PW2_REGISTER					64
#define USB4_PER2_REGISTER					65
#define USB4_PW3_REGISTER					66
#define USB4_PER3_REGISTER					67


//****** Status Register Bits ******************/		// bit  6-0 reserved
#define STATUS_BIT_LATCHED_ZERO_DETECTED	0x00000080	// bit  7
#define STATUS_BIT_LATCHED_MATCH_DETECTED	0x00000100	// bit  8
#define STATUS_BIT_LATCHED_CARRY_DETECTED	0x00000200	// bit  9
#define STATUS_BIT_LATCHED_BORROW_DETECTED	0x00000400	// bit 10
#define STATUS_BIT_LATCHED_INDEX_DETECTED	0x00000800	// bit 11
#define STATUS_BIT_LATCHED_ADVANCE_DETECTED	0x00001000	// bit 12
#define STATUS_BIT_LATCHED_RETARD_DETECTED	0x00002000	// bit 13
#define STATUS_BIT_ZERO_DETECTED			0x00004000	// bit 14
#define STATUS_BIT_MATCH_DETECTED			0x00008000	// bit 15
#define STATUS_BIT_CARRY_DETECTED			0x00010000	// bit 16
#define STATUS_BIT_BORROW_DETECTED			0x00020000	// bit 17
#define STATUS_BIT_INDEX_DETECTED			0x00040000	// bit 18
#define STATUS_BIT_ADVANCE_DETECTED			0x00080000	// bit 19
#define STATUS_BIT_RETARD_DETECTED			0x00100000	// bit 20
														// bit 22-21 reserved
#define STATUS_BIT_LAST_DIRECTION_INDICATOR	0x08000000	// bit 23			
														// bit 31-24 reserved

//****** Control Register Bits ******************/			// bit  6-0
#define CONTROL_BIT_INDEX_ENABLE_ON_MATCH		0x00000010	// bit  4
#define CONTROL_BIT_TRIGGER_ON_ZERO				0x00000080	// bit  7
#define CONTROL_BIT_TRIGGER_ON_MATCH			0x00000100	// bit  8
#define CONTROL_BIT_TRIGGER_ON_ROLLOVER			0x00000200	// bit  9
#define CONTROL_BIT_TRIGGER_ON_ROLLUNDER		0x00000400	// bit 10
#define CONTROL_BIT_TRIGGER_ON_INDEX			0x00000800	// bit 11
#define CONTROL_BIT_TRIGGER_ON_INCREASE			0x00001000	// bit 12
#define CONTROL_BIT_TRIGGER_ON_DECREASE			0x00002000	// bit 13
#define CONTROL_BITS_MULTIPLIER					0x0000C000	// bits 14 & 15
#define CONTROL_BITS_COUNTER_MODE				0x00030000	// bits 16 & 17
#define CONTROL_BIT_COUNTER_ENABLE				0x00040000	// bit 18
#define CONTROL_BIT_COUNT_DIRECTION				0x00080000	// bit 19
#define CONTROL_BIT_INDEX_ENABLE				0x00100000	// bit 20
#define CONTROL_BIT_INVERT_INDEX				0x00200000	// bit 21
#define CONTROL_BIT_COUNTER_RESET_OR_PRESET		0x00400000	// bit 22
#define CONTROL_BIT_ENABLE_CAPTURE				0x00800000	// bit 23
															// bit 31-24 reserved
				
//******* Input Port Register Bits ***************/
#define INPUT_PORT_ESTOP						0x00000100	// bit 8

//******* Output Port Configuration Bits ***************/
#define INVERT_OUTPUT_POLARITY					0x00000020	// bit 5


#ifdef __cplusplus 
extern "C"{ 
#endif 

	struct USB4_FIFOBufferRecord
	{
		unsigned char Header[6];
		unsigned char Input;
		unsigned char EStop;
		unsigned long Time;
		unsigned long Count[4];
		unsigned char Status[4];
		unsigned short ADC[4];
	};

	int _stdcall USB4_CaptureTimeAndCounts(short iDeviceNo, unsigned long *pulCounts, unsigned long *pulTimeStamp);
	int _stdcall USB4_ClearCapturedStatus(short iDeviceNo, short iEncoder);
	int _stdcall USB4_ClearDigitalInputTriggerStatus(short iDeviceNo);
	int _stdcall USB4_ClearFIFOBuffer(short iDeviceNo);
	int _stdcall USB4_DeviceCount();
	int _stdcall USB4_DisableFIFOBuffer(short iDeviceNo);
	int _stdcall USB4_EnableFIFOBuffer(short iDeviceNo);
	int _stdcall USB4_GetA2D(short iDeviceNo, short iA2DChannel, unsigned short * puiA2DValue);
	int _stdcall USB4_GetA2DSamplingFrequency(short iDeviceNo, unsigned short * puiVal);
	int _stdcall USB4_GetCaptureEnabled(short iDeviceNo, short iEncoder, BOOL *pbVal);
	int _stdcall USB4_GetControlMode(short iDeviceNo, short iEncoder, unsigned long *pulVal);
	int _stdcall USB4_GetCount(short iDeviceNo, short iEncoder, unsigned long *pulVal);
	int _stdcall USB4_GetCounterMode(short iDeviceNo, short iEncoder, short *piVal);
	int _stdcall USB4_GetDeviceNo(short iModuleAddress, short *piDeviceNo);
	int _stdcall USB4_GetDigitalInputTriggerConfig(short iDeviceNo, BOOL *pbEnableTrigger, BOOL *pbTriggerOnRisingEdge);
	int _stdcall USB4_GetDigitalInputTriggerStatus(short iDeviceNo, BOOL *pbVal);
	int _stdcall USB4_GetDriverBuildNumber(short iDeviceNo, unsigned char *pucVersion);
	int _stdcall USB4_GetCounterEnabled(short iDeviceNo, short iEncoder, BOOL *pbVal);
	int _stdcall USB4_GetEnableIndex(short iDeviceNo, short iEncoder, BOOL *pbVal);
	int _stdcall USB4_GetEnableIndexOnMatch(short iDeviceNo, short iEncoder, BOOL *pbVal);
	int _stdcall USB4_GetEStopBit(short iDeviceNo, unsigned char *pucVal);
	int _stdcall USB4_GetFactoryInfo(short iDeviceNo, unsigned short *puiModel, unsigned short *puiVersion, unsigned long *pulSN, unsigned char *pucMonth, unsigned char *pucDay, unsigned short *pusYear);
	int _stdcall USB4_GetFIFOBufferCount(short iDeviceNo, unsigned long *plVal);
	int _stdcall USB4_GetForward(short iDeviceNo, short iEncoder, BOOL *pbVal);
	int _stdcall USB4_GetInterruptControl(short iDeviceNo, BOOL *pbEnableFIFOHalfFullInterrupt, BOOL *pbEnableTriggerOutInterrupt);
	int _stdcall USB4_GetInvertIndex(short iDeviceNo, short iEncoder, BOOL *pbVal);
	int _stdcall USB4_GetInvertOutput(short iDeviceNo, BOOL *pbVal);
	int _stdcall USB4_GetMatch(short iDeviceNo, short iEncoder, unsigned long *pulVal);
	int _stdcall USB4_GetModuleAddress(short iDeviceNo, unsigned char *pucModuleAddress);
	int _stdcall USB4_GetMultiplier(short iDeviceNo, short iEncoder, short *piVal);
	int _stdcall USB4_GetOutputPortConfig(short iDeviceNo, BOOL *pbTriggerOutSignalDrivesOutputPin, unsigned char *pucTriggerSignalLengthCode);
	int _stdcall USB4_GetPresetOnIndex(short iDeviceNo, short iEncoder, BOOL *pbVal);
	int _stdcall USB4_GetPresetValue(short iDeviceNo, short iEncoder, unsigned long *pulVal);
	int _stdcall USB4_GetPWM(short iDeviceNo, short iPWMChannel, unsigned long *pulPulseWidth, unsigned long *pulPulsePeriod);
	int _stdcall USB4_GetPWMConfig(short iDeviceNo, unsigned char *pucDivisor, unsigned char *pucCaptureToFIFOFlags);
	int _stdcall USB4_GetROM_ID(short iDeviceNo, unsigned char *pucVal);
	int _stdcall USB4_GetRPM(short iDeviceNo, short iEncoder, float *pufRPM);
	int _stdcall USB4_GetSamplesRemaining(short iDeviceNo, unsigned long *pulVal);
	int _stdcall USB4_GetSamplesToCollect(short iDeviceNo, unsigned long *pulVal);
	int _stdcall USB4_GetSamplingRateCounter(short iDeviceNo, unsigned long *pulVal);
	int _stdcall USB4_GetSamplingRateMultiplier(short iDeviceNo, unsigned long *pulVal);
	int _stdcall USB4_GetStatus(short iDeviceNo, short iEncoder, unsigned long *pulVal);
	int _stdcall USB4_GetStatusEx(short iDeviceNo, short iEncoder, BOOL *pbDecreaseDetected, BOOL *pbIncreaseDetected, BOOL *pbIndexDetected, BOOL *pbRollunderDetected, BOOL *pbRolloverDetected, BOOL *pbMatchDetected, BOOL *pbZeroDetected);
	int _stdcall USB4_GetTimeBasedLogSettings(short iDeviceNo, unsigned char * pucInputTrigger1Array, unsigned char * pucInputTrig1And,
											  unsigned char * pucInputTrigger2Array, unsigned char * pucInputTrig2And,
											  unsigned char * pucADCTrigger, unsigned short * puiADCThreshold,
											  unsigned char * pucPWMTrigger, unsigned long  * pulPWMThreshold,
											  unsigned char * pucEncoderChannels, unsigned long * pulNumberOfSamples);
	int _stdcall USB4_GetTimeStamp(short iDeviceNo, unsigned long *pulVal);
	int _stdcall USB4_GetTriggerOnDecrease(short iDeviceNo, short iEncoder, BOOL *pbVal);
	int _stdcall USB4_GetTriggerOnIncrease(short iDeviceNo, short iEncoder, BOOL *pbVal);
	int _stdcall USB4_GetTriggerOnIndex(short iDeviceNo, short iEncoder, BOOL *pbVal);
	int _stdcall USB4_GetTriggerOnMatch(short iDeviceNo, short iEncoder, BOOL *pbVal);
	int _stdcall USB4_GetTriggerOnRollover(short iDeviceNo, short iEncoder, BOOL *pbVal);
	int _stdcall USB4_GetTriggerOnRollunder(short iDeviceNo, short iEncoder, BOOL *pbVal);
	int _stdcall USB4_GetTriggerOnZero(short iDeviceNo, short iEncoder, BOOL *pbVal);
	int _stdcall USB4_GetVersion(short iDeviceNo, unsigned short *puiVersion);
	int _stdcall USB4_Initialize(short *pDeviceCount);
	BOOL _stdcall USB4_IsFIFOBufferEmpty(short iDeviceNo, int *piResult);
	BOOL _stdcall USB4_IsFIFOBufferFull(short iDeviceNo, int * piResult);
	int _stdcall USB4_PresetCount(short iDeviceNo, short iEncoder);
	int _stdcall USB4_ReadFIFOBuffer(short iDeviceNo, long *plSize, unsigned long * pTime, unsigned long * pCount0, unsigned long * pCount1, unsigned long * pCount2, unsigned long * pCount3, unsigned char * pStatus0, unsigned char * pStatus1, unsigned char * pStatus2, unsigned char * pStatus3, unsigned char * pInput, unsigned char * pEStop, unsigned long * pADC0, unsigned long * pADC1, unsigned long * pADC2, unsigned long * pADC3, unsigned long ulReadTimeout);
	int _stdcall USB4_ReadFIFOBufferStruct(short iDeviceNo, long *plSize, USB4_FIFOBufferRecord *pCBR, unsigned long ulReadTimeout);
	int _stdcall USB4_ReadInputPortRegister(short iDeviceNo, unsigned char *pucVal);
	int _stdcall USB4_ReadOutputLatch(short iDeviceNo, short iEncoder, unsigned long *pulVal);
	int _stdcall USB4_ReadOutputPortRegister(short iDeviceNo, unsigned char *pucVal);
	int _stdcall USB4_ReadRegister(short iDeviceNo, short iRegister, unsigned long *pulVal);
	int _stdcall USB4_ReadSavedParameters(short iDeviceNo);
	int _stdcall USB4_ReadTimeAndCounts(short iDeviceNo, unsigned long *pulCounts, unsigned long *pulTimeStamp);
	int _stdcall USB4_ReadTimeStamp(short iDeviceNo, unsigned long *pulVal);
	int _stdcall USB4_ReadUnlatchedTimeAndCounts(short iDeviceNo, unsigned long *pulCounts, unsigned long *pulTimeStamp);
	int _stdcall USB4_ReadUserEEPROM(short iDeviceNo, unsigned char ucStartAddress, unsigned char ucBytesToRead, unsigned char * pucDataArray);
	int _stdcall USB4_ResetCount(short iDeviceNo, short iEncoder);
	int _stdcall USB4_ResetTimeStamp(short iDeviceNo);
	int _stdcall USB4_RestoreFactoryParameters(short iDeviceNo);
	int _stdcall USB4_SaveParameters(short iDeviceNo);
	int _stdcall USB4_SetA2DSamplingFrequency(short iDeviceNo, unsigned short uiVal);
	int _stdcall USB4_SetCaptureEnabled(short iDeviceNo, short iEncoder, BOOL bVal);
	int _stdcall USB4_SetControlMode(short iDeviceNo, short iEncoder, unsigned long ulVal);
	int _stdcall USB4_SetCount(short iDeviceNo, short iEncoder, unsigned long ulVal);
	int _stdcall USB4_SetCounterMode(short iDeviceNo, short iEncoder, short iVal);
	int _stdcall USB4_SetD2A(short iDeviceNo, short iChannel, unsigned short iD2AValue, BOOL bUpdateD2AChannelsNow);
	int _stdcall USB4_SetD2AControlMode(short iDeviceNo, unsigned char ucMode);
	int _stdcall USB4_SetDigitalInputTriggerConfig(short iDeviceNo, BOOL *pbEnableTrigger, BOOL *pbTriggerOnRisingEdge);
	int _stdcall USB4_SetCounterEnabled(short iDeviceNo, short iEncoder, BOOL bVal);
	int _stdcall USB4_SetEnableIndex(short iDeviceNo, short iEncoder, BOOL bVal);
	int _stdcall USB4_SetEnableIndexOnMatch(short iDeviceNo, short iEncoder, BOOL bVal);
	int _stdcall USB4_SetEStopBit(short iDeviceNo, unsigned char bVal);
	int _stdcall USB4_SetForward(short iDeviceNo, short iEncoder, BOOL bVal);
	int _stdcall USB4_SetInvertIndex(short iDeviceNo, short iEncoder, BOOL bVal);
	int _stdcall USB4_SetInvertOutput(short iDeviceNo, BOOL bVal);
	int _stdcall USB4_SetMatch(short iDeviceNo, short iEncoder, unsigned long ulVal);
	int _stdcall USB4_SetModuleAddress(short iDeviceNo, unsigned char ucModuleAddress);
	int _stdcall USB4_SetMultiplier(short iDeviceNo, short iEncoder, short iVal);
	int _stdcall USB4_SetOutputPortConfig(short iDeviceNo, BOOL *pbTriggerOutSignalDrivesOutputPin, unsigned char ucTriggerSignalLengthCode);
	int _stdcall USB4_SetPresetOnIndex(short iDeviceNo, short iEncoder, BOOL bVal);
	int _stdcall USB4_SetPresetValue(short iDeviceNo, short iEncoder, unsigned long ulVal);
	int _stdcall USB4_SetPWMConfig(short iDeviceNo, unsigned char pucDivisor, unsigned char ucCaptureToFIFOFlags);
	int _stdcall USB4_SetSamplesToCollect(short iDeviceNo, unsigned long ulVal);
	int _stdcall USB4_SetSamplingRateMultiplier(short iDeviceNo, unsigned long ulVal);
	int _stdcall USB4_SetTimeBasedLogSettings(short iDeviceNo, unsigned char * pucInputTrigger1Array, unsigned char ucInputTrig1And, 
											  unsigned char * pucInputTrigger2Array, unsigned char ucInputTrig2And, 
											  unsigned char * pucADCTrigger, unsigned short * puiADCThreshold, 
											  unsigned char * pucPWMTrigger, unsigned long  * pulPWMThreshold, 
											  unsigned char ucEncoderChannels, unsigned long ulNumberOfSamples);
	int _stdcall USB4_SetTriggerOnDecrease(short iDeviceNo, short iEncoder, BOOL bVal);
	int _stdcall USB4_SetTriggerOnIncrease(short iDeviceNo, short iEncoder, BOOL bVal);
	int _stdcall USB4_SetTriggerOnIndex(short iDeviceNo, short iEncoder, BOOL bVal);
	int _stdcall USB4_SetTriggerOnMatch(short iDeviceNo, short iEncoder, BOOL bVal);
	int _stdcall USB4_SetTriggerOnRollover(short iDeviceNo, short iEncoder, BOOL bVal);
	int _stdcall USB4_SetTriggerOnRollunder(short iDeviceNo, short iEncoder, BOOL bVal);
	int _stdcall USB4_SetTriggerOnZero(short iDeviceNo, short iEncoder, BOOL bVal);
	void _stdcall USB4_Shutdown();
	int _stdcall USB4_StartAcquisition(short iDeviceNo);
	int _stdcall USB4_StopAcquisition(short iDeviceNo);
	int _stdcall USB4_TriggerSoftwareCapture(short iDeviceNo);
	int _stdcall USB4_WriteOutputPortRegister(short iDeviceNo, unsigned char ucVal);
	int _stdcall USB4_WriteRegister(short iDeviceNo, short iRegister, unsigned long ulVal);
	int _stdcall USB4_WriteUserEEPROM(short iDeviceNo, unsigned char startAddress, unsigned char bytesToWrite, unsigned char * pucData);


	#ifdef _DEBUG
	#define PRINT(x)	OutputDebugString(x)
	#else
	#define PRINT(x)
	#endif
	
#ifdef __cplusplus 
} 
#endif 

#endif