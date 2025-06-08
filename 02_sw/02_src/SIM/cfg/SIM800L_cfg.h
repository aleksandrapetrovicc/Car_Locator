/// @file SIM800L_cfg.h
/// @brief Contains configuration data used for SIM800L module
/// @author Aleksandra Petrovic

#ifndef SIM800L_CFG_H_
#define SIM800L_CFG_H_

#include"SIM.h"

/// Used to define number of words in a dictionary
#define SIM800L_DICTIONARY_LENGTH 20u
/// Used to define maximum length of SIM800L response
#define SIM800L_RESPONSE_LENGTH 50u
/// Used to define the length of phone number
#define SIM800L_NUMBER_LENGTH 12
/// Used to define number of states SIM800L module can be in
#define SIM800L_STATES 5u
/// Used to define SIM800L message buffer length
#define SIM800L_MESSAGE_BUFFER_LENGTH 300u
/// Carriage Return in ASCII
#define CARRIAGE_RETURN 13
/// Line Feed in ASCII
#define LINE_FEED 10
/// ESC in ASCII
#define ESC 27

// SIM800L states for different functions
t_SIM_Function SIM800L_t_Functions[SIM800L_STATES] = {
		{ IdleFunction,		ReadMessage	 },
		{ MakeCall,  	 	IdleFunction },
		{ EndCall,     		MakeCall	 },
		{ SendMessage,		EndCall		 },
		{ ReadMessage, 		SendMessage	 }
};

/// SIM800L dictionary used for SIM800L commands
t_SIM_Command SIM800L_t_Dictionary[SIM800L_DICTIONARY_LENGTH] = {
		{ (uint8_t*)"AT", 					SIM, 		AT   },
		{ (uint8_t*)"AT+CSQ", 				SIM, 		CSQ  },
		{ (uint8_t*)"AT+CCID", 				SIM, 		CCID },
		{ (uint8_t*)"AT+CREG?", 			SIM,		CREG },
		{ (uint8_t*)"AT+CMGF=1",			SIM,		CMGF },
		{ (uint8_t*)"AT+CNMI=1,2,0,0,0",	SIM, 		CNMI },
		{ (uint8_t*)"ATH", 					SIM, 		ATH  },
		{ (uint8_t*)"ATD+ ", 				SIM, 		ATD  },
		{ (uint8_t*)"AT+CMGS=+", 			SIM, 		CMGS }
};
/// Used to determine the length of SIM800L_t_Dictionary array
uint16_t SIM800L_u_DictionaryLength = sizeof(SIM800L_t_Dictionary) / sizeof(SIM800L_t_Dictionary[0]);

/// SIM800L dictionary for SIM800L responses for AT commands, calls and messages
t_SIM_Response SIM800L_t_ResponseDictionary[SIM800L_DICTIONARY_LENGTH] = {
		{ (uint8_t*)"OK", 			2,		SIM, 		OK 		   },   			//b_FALSE	},
		{ (uint8_t*)"RING", 		4,		SIM, 		RING 	   }, 				//b_FALSE	},
		{ (uint8_t*)"NO CARRIER", 	10,		SIM, 		NO_CARRIER },				//b_FALSE	},
		{ (uint8_t*)"+CLIP: ",      7,		SIM,		CLIP	   }				//b_FALSE	}
};
/// Used to determine the length of SIM800L_t_ResponseDictionary array
uint16_t SIM800L_u_ResponseDictionaryLength = sizeof(SIM800L_t_ResponseDictionary) / sizeof(SIM800L_t_ResponseDictionary[0]);

/// SIM800L dictionary of known callers
t_SIM_KnownCaller SIM800L_t_CallerDictionary[SIM800L_DICTIONARY_LENGTH] = {
		{ (uint8_t*)"+381605074705", 	Aleksandra, 	b_FALSE },
		{ (uint8_t*)"+381611753295", 	SIM_module, 	b_FALSE }
};
/// USed to determine the length of SIM800L_t_CallerDictionary array
uint16_t SIM800L_u_CallerDictionaryLength = sizeof(SIM800L_t_CallerDictionary) / sizeof(SIM800L_t_CallerDictionary[0]);

#endif /* SIM800L_CFG_H_ */
