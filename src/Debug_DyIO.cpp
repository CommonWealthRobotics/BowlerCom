
#include <BowlerCom.h>
//int maxNumCharsInModes=0;
//char *  printModeStrings[]={
//		"NO_CHANGE",
//		"HIGH_IMPEDANCE",
//		"IS_DI",
//		"IS_DO",
//		"IS_ANALOG_IN",
//		"IS_ANALOG_OUT",
//		"PWM",
//		"SERVO",
//		"UART_TX",
//		"UART_RX",
//		"SPI_MOSI",
//		"SPI_MISO",
//		"SPI_SCK",
//		"SPI_SS",
//		"COUNTER_INPUT_INT",
//		"COUNTER_INPUT_DIR",
//		"COUNTER_INPUT_HOME",
//		"COUNTER_OUTPUT_INT",
//		"COUNTER_OUTPUT_DIR",
//		"COUNTER_OUTPUT_HOME",
//		"IS_DC_MOTOR_DIR",
//		"IS_DC_MOTOR_VEL",
//		"IS_PPM_IN"
//};

int mystrlen(const char * s){
	int i=0;
	while(s[i++]);
	return i;
}
void printModeLocal(uint8_t mode,  Print_Level l){
	//

	switch(mode){
case NO_CHANGE	 	 		:
		print_nnl(" NO_CHANGE",l);
		break;

case HIGH_IMPEDANCE 	 	:
		print_nnl(" HIGH_IMPEDANCE",l);
		break;
case IS_DI	 		 		:
		print_nnl(" IS_DI",l);
		break;
case IS_DO		 			:
		print_nnl(" IS_DO",l);
		break;
case IS_ANALOG_IN 			:
		print_nnl(" IS_ANALOG_IN",l);
		break;
case IS_ANALOG_OUT			:
		print_nnl(" IS_ANALOG_OUT",l);
		break;
case IS_PWM	 				:
		print_nnl(" IS_PWM",l);
		break;
case IS_SERVO 				:
		print_nnl(" IS_SERVO",l);
		break;
case IS_UART_TX		 		:
		print_nnl(" IS_UART_TX",l);
		break;
case IS_UART_RX	 			:
		print_nnl(" IS_UART_RX",l);
		break;
case IS_SPI_MOSI			:
		print_nnl(" IS_SPI_MOSI",l);
		break;
case IS_SPI_MISO			:
		print_nnl(" IS_SPI_MISO",l);
		break;
case IS_SPI_SCK	 			:
		print_nnl(" IS_SPI_SCK",l);
		break;
case IS_COUNTER_INPUT_INT	:
		print_nnl(" IS_COUNTER_INPUT_INT",l);
		break;
case IS_COUNTER_INPUT_DIR	:
		print_nnl(" IS_COUNTER_INPUT_DIR",l);
		break;
case IS_COUNTER_INPUT_HOME	:
		print_nnl(" IS_COUNTER_INPUT_HOME",l);
		break;
case IS_COUNTER_OUTPUT_INT	:
		print_nnl(" IS_COUNTER_OUTPUT_INT",l);
		break;
case IS_COUNTER_OUTPUT_DIR	:
		print_nnl(" IS_COUNTER_OUTPUT_DIR",l);
		break;
case IS_COUNTER_OUTPUT_HOME	:
		print_nnl(" IS_COUNTER_OUTPUT_HOME",l);
		break;
case IS_DC_MOTOR_VEL		:
		print_nnl(" IS_DC_MOTOR_VEL",l);
		break;
case IS_DC_MOTOR_DIR		:
		print_nnl(" IS_DC_MOTOR_DIR",l);
		break;
case IS_PPM_IN				:
		print_nnl(" IS_PPM_IN",l);
		break;
case IS_DEBUG_RX				:
		print_nnl(" IS_DEBUG_RX",l);
		break;
case IS_DEBUG_TX				:
		print_nnl(" IS_DEBUG_TX",l);
		break;
default:
	print_nnl(" Unknown Mode 0x",l);prHEX8(mode,l);
	}

}


//char * unknown = "UNKNOWN";
void printMode(uint8_t pin,uint8_t newMode,  Print_Level l){
	uint8_t mode =GetChannelMode(pin);
	printModeLocal( mode,   l);
	print_nnl(" ,",l);
	printModeLocal( newMode,   l);

}
