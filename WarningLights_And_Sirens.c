#include <winsock.h>
#include <advanlys.h>
#include <formatio.h>
#include <ansi_c.h>
#include <utility.h>
#include <cvirte.h>
#include <userint.h>
#include <adamtcp.h>
#include "inifile.h"
#include "WarningLights_And_Sirens.h"

#define		ADAM_UDP_RECEIVE_TIMEOUT 	300 
#define		ADAM_UDP_SEND_TIMEOUT 		100
#define		BW_AND &

#define 	NUMBER_OF_ADAM_UNITS		4
#define		ON 							1
#define		OFF							0
#define 	TRUE 						1
#define 	FALSE 						0

#define     GREEN 						0X66FF33               
#define 	WHITE 						0XFFFFFF

struct ADAM_Struct
{
		int id;
		char *ipaddr;
		char *name;
		char digitalString[16];
		BOOL siren;
		BOOL light;
		BOOL auxiliary;
		BOOL newTypeSiren;
		int  lightDisplay;
		int	 sirenDisplay;
		int	 auxiliaryDisplay;
		BOOL sirenstatus;
		BOOL ipstatus;
		BOOL lightstatus;
		BOOL compressorstatus;
};	

struct 		ADAM_Struct AdamUnit[NUMBER_OF_ADAM_UNITS];

static int 		panelHandle;

unsigned int	i = 0,
				toggle_bit = 0;
				year,
				month,
				day,
				hh,
				mm,
				ss,
				unit_index = 0;
				
				LED[8],
				STAT[8],
				longDurationTimer[5],
				shortDurationTimer[5];
			


int				gQuit 	= 0,
				stat 	= 0,
				stat2	= 0,
				stat3	= 0;
				
		

char			timeMsg[64],
				dateMsg[64],
				errorMessage[255],
				Message[255],
				returnValue[255],
				testPatternString[8],
				*leftAuxTitle,
				*middleAuxTitle,
				*rightAuxTitle;

BOOL			status_on = FALSE;
                firsttime_message[NUMBER_OF_ADAM_UNITS] = {TRUE,TRUE,TRUE,TRUE},
				TyphoneSound[NUMBER_OF_ADAM_UNITS] = {OFF,OFF,OFF,OFF};



//----  .ini file variables  ----

static			IniText 	lista;



//---- Function Prototypes ----

void Adam_udp_startup(void);
void SystemInit(void);
void getTimeAndDateMsg(void);
void catchStatusData(char*,int);
void displayStatus(int);
void ActivateDangerSound(int);
void ActivateAllClear(int);
void SirenOff(int);
void WarningLight(int, BOOL);
void Auxiliary(int, BOOL);
void toggleSiren(int);
void stopTimersAndDisableSiren(int);
void shortDurationHandlerForUnit(int);
void displaySystemMessage(char*);
void SetIPStatus(BOOL, int, int, char*);
void SetStatusFieldText(int,int, char*);



//************************************************************************************************
//                          MAIN PROGRAM 
//************************************************************************************************

int main (int argc, char *argv[])
{
	
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "WarningLights_And_Sirens.uir", PANEL)) < 0)
		return -1;
	   	
	SystemInit();  // Set up the program according to the configuration file
		
	DisplayPanel (panelHandle); // Show main panel
	
	// Add titles, from the conf file,  to the different sections on the GUI
	SetCtrlVal(panelHandle,  PANEL_MODULE_0_TITLE,  AdamUnit[0].name);
	SetCtrlVal(panelHandle,  PANEL_MODULE_1_TITLE,  AdamUnit[1].name);
	SetCtrlVal(panelHandle,  PANEL_MODULE_2_TITLE,  AdamUnit[2].name);
	SetCtrlVal(panelHandle,  PANEL_MODULE_3_TITLE,  AdamUnit[3].name);
	
	// Add names, from conf file, to the ip address fields
	SetCtrlVal(panelHandle,  PANEL_MODULE_0_CONN_TITLE,  AdamUnit[0].name);
	SetCtrlVal(panelHandle,  PANEL_MODULE_1_CONN_TITLE,  AdamUnit[1].name);
	SetCtrlVal(panelHandle,  PANEL_MODULE_2_CONN_TITLE,  AdamUnit[2].name);
	SetCtrlVal(panelHandle,  PANEL_MODULE_3_CONN_TITLE,  AdamUnit[3].name);
	
	// Add ip addresses, from conf file,  to the address fields
	SetCtrlVal(panelHandle,PANEL_MODULE_0_IP,AdamUnit[0].ipaddr);
	SetCtrlVal(panelHandle,PANEL_MODULE_1_IP,AdamUnit[1].ipaddr);
	SetCtrlVal(panelHandle,PANEL_MODULE_2_IP,AdamUnit[2].ipaddr);
	SetCtrlVal(panelHandle,PANEL_MODULE_3_IP,AdamUnit[3].ipaddr);
	
	SetCtrlVal(panelHandle,PANEL_LEFT_AUX_TITLE,leftAuxTitle);
	SetCtrlVal(panelHandle,PANEL_MIDDLE_AUX_TITLE,middleAuxTitle);
	SetCtrlVal(panelHandle,PANEL_RIGHT_AUX_TITLE,rightAuxTitle);
	
	
	for(i = 0; i < NUMBER_OF_ADAM_UNITS; i++) // for all Adam units
	{
		strcpy(AdamUnit[i].digitalString, "");
		
		if (i > 0) // timer counts from number 1 and up
		{
		   	stat = SetCtrlAttribute (panelHandle, shortDurationTimer[i], ATTR_ENABLED, 0); // disable 2 seconds timer for the actual ADAM unit
			stat2 = SetCtrlAttribute (panelHandle, longDurationTimer[i], ATTR_ENABLED, 0); // disable 30 seconds timer for the actual ADAM unit
		}
	}
		
			
	// ADAM UDP COMMUNICATION STARTUP
	Adam_udp_startup ();
 
//  Start the GUI loop and check for user interactions
	while(!gQuit)   // This is a fix to reduce cpu-load   (used instead of RunUserInterface)
	{
		ProcessSystemEvents();  
		Sleep(20);
	}
	
	// Disconnect UDP channel at program termination
	ADAMTCP_UDPClose();
	DiscardPanel (panelHandle);
	return 0;
}


//-----------END OF MAIN PROGRAM  --------------------------------------------------------------------




void  Adam_udp_startup(void)
{
    if(ADAMTCP_Open() != 0)
		{ displaySystemMessage("ADAMTCP_Open failed!\r\n");}
	else
	{ 
		stat = ADAMTCP_UDPOpen(ADAM_UDP_SEND_TIMEOUT, ADAM_UDP_RECEIVE_TIMEOUT); // API function call to open ADAM connection
		if (stat != 0)
		{ 
		  displaySystemMessage( "ADAMTCP_UDPOpen failed, ADAM error: %d IP:Not available \r\n");
		  displaySystemMessage ("Warning system program stopped!! \r\n");
		  stat = MessagePopup ("Communication setup fault", "ADAMTCP_UDPOpen failed, IP:Not available\r\n Warning system program stopped");
		  gQuit = 1; // set program termination flag
		}
	}
}
//***********************************************************************************************************

void SystemInit(void) 
{	
	
	for(i = 0; i < NUMBER_OF_ADAM_UNITS; i++)
	{
		AdamUnit[i].light 		 = FALSE;
		AdamUnit[i].siren		 = FALSE;
		AdamUnit[i].auxiliary 	 = FALSE;
		TyphoneSound[i] 		 = OFF;
	}
		
	//******** Connect status info to user interface indicators ***************************
   
	AdamUnit[0].lightDisplay = PANEL_MODULE_0_LIGHT_TEXT;
	AdamUnit[1].lightDisplay = PANEL_STAT_7;
	AdamUnit[2].lightDisplay = PANEL_MODULE_2_LIGHT_TEXT;
	AdamUnit[3].lightDisplay = PANEL_MODULE_3_LIGHT_TEXT;
			
	AdamUnit[0].sirenDisplay = PANEL_STAT_8;                         
	AdamUnit[1].sirenDisplay = PANEL_MODULE_1_SIREN_DISPL;
	AdamUnit[2].sirenDisplay = PANEL_MODULE_2_SIREN_DISPL;
	AdamUnit[3].sirenDisplay = PANEL_MODULE_3_SIREN_DISPL;                            
		
	AdamUnit[0].auxiliaryDisplay = PANEL_STAT_9;
	AdamUnit[1].auxiliaryDisplay = PANEL_STAT_11;
	AdamUnit[2].auxiliaryDisplay = PANEL_MODULE_2_AUX_TEXT;   
	AdamUnit[3].auxiliaryDisplay = PANEL_MODULE_3_AUX_TEXT;  
	
	AdamUnit[0].ipstatus = PANEL_MODULE_0_IP_OK_DISPL;
	AdamUnit[1].ipstatus = PANEL_MODULE_1_IP_OK_DISPL;
	AdamUnit[2].ipstatus = PANEL_MODULE_2_IP_OK_DISPL;
	AdamUnit[3].ipstatus = PANEL_MODULE_3_IP_OK_DISPL;
			
	
	longDurationTimer[1] = PANEL_MODULE_1_LONG_TIMER;
	longDurationTimer[2] = PANEL_MODULE_2_LONG_TIMER;
	longDurationTimer[3] = PANEL_MODULE_3_LONG_TIMER;
	
	
	shortDurationTimer[1] = PANEL_MODULE_1_SHORT_TIMER;
	shortDurationTimer[2] = PANEL_MODULE_2_SHORT_TIMER;
	shortDurationTimer[3] = PANEL_MODULE_3_SHORT_TIMER;
	
	
		
	
			
	// Load system parameters from configuration file
	
	lista = Ini_New (1);
	if (Ini_ReadFromFile (lista, "WarningLights_And_Sirens.ini") != 0)
	 { 
		  displaySystemMessage("Error when reading from configuration file!\r\n");
	 }
	  
	else
	 {   
		Ini_GetStringCopy (lista, "ADAM Unit 0", "AdamUnit_0_name",	&AdamUnit[0].name);
		Ini_GetStringCopy (lista, "ADAM Unit 1", "AdamUnit_1_name",	&AdamUnit[1].name);
		Ini_GetStringCopy (lista, "ADAM Unit 2", "AdamUnit_2_name",	&AdamUnit[2].name);
		Ini_GetStringCopy (lista, "ADAM Unit 3", "AdamUnit_3_name",	&AdamUnit[3].name);
		
		Ini_GetStringCopy (lista, "ADAM Unit 0", "AdamUnit_0_ip_address", &AdamUnit[0].ipaddr);
		Ini_GetStringCopy (lista, "ADAM Unit 1", "AdamUnit_1_ip_address", &AdamUnit[1].ipaddr);
		Ini_GetStringCopy (lista, "ADAM Unit 2", "AdamUnit_2_ip_address", &AdamUnit[2].ipaddr);
		Ini_GetStringCopy (lista, "Adam Unit 3", "AdamUnit_3_ip_address", &AdamUnit[3].ipaddr);

		Ini_GetInt (lista, "Adam Unit 1", "AdamUnit_1_newTypeSiren", &AdamUnit[1].newTypeSiren);
		Ini_GetInt (lista, "Adam Unit 2", "AdamUnit_2_newTypeSiren", &AdamUnit[2].newTypeSiren);
		Ini_GetInt (lista, "Adam Unit 3", "AdamUnit_3_newTypeSiren", &AdamUnit[3].newTypeSiren);
	
		Ini_GetStringCopy(lista,"AUX Titles", "Left_Aux_Title",	&leftAuxTitle);
		Ini_GetStringCopy(lista,"AUX Titles", "Middle_Aux_Title", &middleAuxTitle);
		Ini_GetStringCopy(lista,"AUX Titles", "Right_Aux_Title", &rightAuxTitle);
	}
	Ini_Dispose (lista);

} // End SystemInit()



// **************  Timer function to read status from all ADAM-6066 units  ******************************************

int CVICALLBACK digitalStatusTimer (int panel, int control, int event,
									void *callbackData, int eventData1, int eventData2)
{
	char 			errorNumberString[255] = "",
					nameString[64];
	                	
	int				string_size = 0;
	
	switch (event)
	{
		case EVENT_TIMER_TICK:
			
		   for (unit_index = 0; unit_index < NUMBER_OF_ADAM_UNITS; unit_index++)  // For all ADAM-6066 units used in the system 
			{					
				stat = ADAMTCP_SendReceive6KUDPCmd(AdamUnit[unit_index].ipaddr, "$016\r", AdamUnit[unit_index].digitalString);  //Ask for status
							
				if (stat < 0 ) 	// Error								
				{
					if (firsttime_message[unit_index] == TRUE)   // Display the error message only once, not repeatably
					{
						sprintf(errorMessage,"Error when reading from ADAM module, ADAM error: %d  IP =  %s\r\n",stat, AdamUnit[unit_index].ipaddr);	
						displaySystemMessage(errorMessage);
						SetIPStatus(AdamUnit[unit_index].ipstatus, VAL_WHITE, VAL_RED,"NOK");
						firsttime_message[unit_index] = FALSE;	
					}
				}
				else
				{
					string_size = strlen(AdamUnit[unit_index].digitalString); 
				    AdamUnit[unit_index].digitalString[string_size] = 0;     // Marks the end of the string
					
					if (firsttime_message[unit_index] == FALSE)
					{
					    sprintf(Message,"Error Cleared..IP = %s\r\n",AdamUnit[unit_index].ipaddr);
						displaySystemMessage(Message);
						firsttime_message[unit_index] = TRUE;		
					}
			   		SetIPStatus(AdamUnit[unit_index].ipstatus, VAL_BLACK, VAL_GREEN,"OK");
					
					
				}	 
				
			}//end for
	     			
		}	
	return 0;
}


//********************************************************************************************************

void catchStatusData(char *digStatus, int idx)

// Check which bits in the digStatus are set
{
  char 			bitPatternString[8] = "",
				message[64],
				*binaryString = "";
				
				 
  unsigned int 	result 		= 0, 
				power_value = 0,
				count 		= 0,
				integer_value = 0;
    
  
  CopyBytes (bitPatternString, 0, digStatus, 5, 2);   				// Get digital input bitpattern
  bitPatternString[2] = 0;											// Set NULL character at the end of string
 
  
  integer_value = strtol (bitPatternString, 0, 16);
  
   
  for (count = 1; count <= 128; count *= 2)
	
	{
		result = (integer_value) BW_AND (count);   			        // check bit-pattern to find which bits are set  (2^0, 2^1, 2^2, 2^3.. and so on)
		
		switch (count)  
		{
			case 1:		if (result == count)
							 AdamUnit[idx].light = TRUE;   		   	// LSB - (rightmost), relay 0, warning light  (2^0)
						else
						     AdamUnit[idx].light  = FALSE;     
						break;
						
						
			case 2:		if (result == count)						// second bit from the right, relay 1, warning siren (2^1)						
						{	 AdamUnit[idx].siren = TRUE; 
			            	 SetCtrlAttribute(panelHandle,AdamUnit[idx].sirenDisplay,ATTR_TEXT_BGCOLOR,GREEN);// Special case to get a stable on/off display 
						     SetCtrlVal(panelHandle,AdamUnit[idx].sirenDisplay,"On"); 
						}
						else
						     AdamUnit[idx].siren  = FALSE;     
						break;
						
						
			case 4:		if (result == count)
							 AdamUnit[idx].auxiliary = TRUE;   		// third bit from the right, relay 2, auxiliary (2^2)
						else
						     AdamUnit[idx].auxiliary = FALSE; 
						break;
						
						
			case 8:	    break;
			
			case 16:	break;
			
			case 32:	if (result == count)
						{
							SetCtrlAttribute(panelHandle,AdamUnit[idx].sirenDisplay,ATTR_TEXT_BGCOLOR,GREEN);// Special case to get a stable on/off display 
						    SetCtrlVal(panelHandle,AdamUnit[idx].sirenDisplay,"On"); 
						}
						else
						{
						    SetCtrlAttribute(panelHandle,AdamUnit[idx].sirenDisplay,ATTR_TEXT_BGCOLOR,WHITE);// Special case to get a stable on/off display 
						    SetCtrlVal(panelHandle,AdamUnit[idx].sirenDisplay,"Off"); 
						}
						break;
		} 
	}// end for	

}


//************************************************************************************************

// Creates a timestamp message
void getTimeAndDateMsg(void)
{
	GetSystemDate (&month, &day, &year); 
	sprintf(dateMsg,"%04d-%02d-%02d, ",year,month,day);
	GetSystemTime (&hh, &mm, &ss);
	sprintf(timeMsg,"%02d:%02d:%02d ",hh,mm,ss);
	SetCtrlVal(panelHandle,PANEL_SYS_MESS, dateMsg);
	SetCtrlVal(panelHandle,PANEL_SYS_MESS, timeMsg);
}

//************************************************************************************************


//************************************************************************************************

void displayStatus(int Unit_index)
{
	
			if (AdamUnit[Unit_index].light == TRUE)
				{  SetStatusFieldText(AdamUnit[Unit_index].lightDisplay, GREEN, "On"); }
			else
				{ SetStatusFieldText(AdamUnit[Unit_index].lightDisplay, WHITE, "Off"); }
				
	
			if (AdamUnit[Unit_index].siren == TRUE) // Status text and color is handled differently for siren, this is to avoid a toggeling indicator with the siren signal
	           { SetCtrlVal(panelHandle, AdamUnit[Unit_index].sirenstatus, AdamUnit[Unit_index].siren); } 
				
	
			if (AdamUnit[Unit_index].auxiliary == TRUE)
				{ SetStatusFieldText(AdamUnit[Unit_index].auxiliaryDisplay, GREEN, "On"); }
			else
				{ SetStatusFieldText(AdamUnit[Unit_index].auxiliaryDisplay, WHITE, "Off"); }

}

//************************************************************************************************

void displaySystemMessage(char* message)
{
	getTimeAndDateMsg();
	SetCtrlVal(panelHandle, PANEL_SYS_MESS, message);
}


//************************************************************************************************

void SetIPStatus(BOOL field, int textcolor, int bgcolor, char* text)
{
   SetCtrlAttribute(panelHandle, field, ATTR_TEXT_COLOR, textcolor);
   SetCtrlAttribute (panelHandle, field, ATTR_TEXT_BGCOLOR, bgcolor);
   SetCtrlVal(panelHandle,field,text);
}


//************************************************************************************************
void SetStatusFieldText(int textfield, int textBgcolor, char* text)
{
   SetCtrlAttribute(panelHandle, textfield, ATTR_TEXT_BGCOLOR, textBgcolor);
   SetCtrlVal(panelHandle,textfield, text);
}



//************************************************************************************************
//                    Command buttons callback routines
//************************************************************************************************

int CVICALLBACK Quit (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)  // Terminate program callback
{  
	int user_selection;


	switch (event)
	{
		case EVENT_COMMIT:
			
			user_selection = GenericMessagePopup ("Program Termination", "Quit Program?", "OK", "Cancel", 
	      											 NULL, NULL, 0, 0, VAL_GENERIC_POPUP_BTN1, VAL_GENERIC_POPUP_BTN1, 
													 VAL_GENERIC_POPUP_BTN2);
					 
			if(user_selection == 1)
			{   
				gQuit = 1;  // Quit program 
			}
			break;
	}
	return 0;
}

//************************************************************************************************

int CVICALLBACK AuxOn (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)      
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			switch (control)
			{
				case PANEL_MODULE_2_AUX_ON:		Auxiliary(2,ON);   // Auxiliary relay, (relay #2 for module 2 -> ON)
												break;
					
				case PANEL_MODULE_3_AUX_ON:		Auxiliary(3,ON);   // Auxiliary relay, (relay #2 for module 3 -> ON)
												break;
			}
			
			break;
	}
	return 0;
}

//************************************************************************************************

int CVICALLBACK AuxOff (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)      
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			switch (control)
			{
				case PANEL_MODULE_2_AUX_OFF:	Auxiliary(2,OFF);   // Auxiliary relay, (relay #2 for module 2 -> OFF)
												break;
					
				case PANEL_MODULE_3_AUX_OFF:	Auxiliary(3,OFF);   // Auxiliary relay, (relay #2 for module 3 -> OFF)
												break;
			}
			
			break;
	}
	return 0;
}


//************************************************************************************************

int CVICALLBACK LampOn (int panel, int control, int event,
						void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			switch (control)  
			{
				case PANEL_MODULE_0_LAMP_ON:	WarningLight(0,ON);    // warning light for module 0 ->  ON
											    break;
												
				// Module 1 is Main Building ADAM unit for siren control and has no warning light

				case PANEL_MODULE_2_LAMP_ON:	WarningLight(2,ON);    // warning light for module 2 ->  ON
												break;
												
				case PANEL_MODULE_3_LAMP_ON:	WarningLight(3,ON);    // warning light for module 3 ->  ON
												break;
			}
	}
	return 0;
}


//************************************************************************************************

int CVICALLBACK LampOff (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			switch (control)  
			{
				case PANEL_MODULE_0_LAMP_OFF:	WarningLight(0,OFF);    // warning light for module 0 ->  OFF
											    break;
				// Module 1 has no warning light
				
				case PANEL_MODULE_2_LAMP_OFF:	WarningLight(2,OFF);    // warning light for module 2 ->  OFF
												break;
												
				case PANEL_MODULE_3_LAMP_OFF:	WarningLight(3,OFF);    // warning light for module 3 ->  OFF
												break;
			}
	}
	return 0;
}

//************************************************************************************************

int CVICALLBACK SirenDanger (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	int userResponseValue;
	
	switch (event)
	{
		case EVENT_COMMIT:
				userResponseValue = GenericMessagePopup ("Operator Command", "Confirm - Activate Danger Signal ?", "OK", "Cancel", NULL, NULL, 0, 0, VAL_GENERIC_POPUP_BTN1, VAL_GENERIC_POPUP_BTN1, VAL_GENERIC_POPUP_BTN2);
			    
				if(userResponseValue == 1)
				{
					
					switch (control) 
					{
						case PANEL_MODULE_1_SIREN_DANGER:	ActivateDangerSound(1);  //  Danger siren for module 1 ->  ON
															break;	
															
						case PANEL_MODULE_2_SIREN_DANGER:	ActivateDangerSound(2);  //  Danger siren for module 2 -> ON
															break;
															
						case PANEL_MODULE_3_SIREN_DANGER:	ActivateDangerSound(3);  //  Danger siren for module 3 ->  ON
															break;
					}// end switch (control)
				}
			break;
	}
	return 0;
}

//************************************************************************************************

int CVICALLBACK SirenAllClear (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	int		userResponseValue;
	
	switch (event)
	{
		case EVENT_COMMIT:
			userResponseValue = GenericMessagePopup ("Operator Command", "Confirm - Activate Danger Over Signal ?", "OK", "Cancel", NULL, NULL, 0, 0, VAL_GENERIC_POPUP_BTN1, VAL_GENERIC_POPUP_BTN1, VAL_GENERIC_POPUP_BTN2);
			
			if(userResponseValue == 1)
			{
				switch (control)  
				{
					//      Module 0 is for wind tower warning light only, has no siren
					
					case 	PANEL_MODULE_1_SIREN_ALL_CL:	ActivateAllClear(1);  //  Danger Over siren for module 1 ->  ON
															break;
							
					case 	PANEL_MODULE_2_SIREN_ALL_CL:	ActivateAllClear(2);  //  Danger Over siren for module 2 ->  ON
															break;
							
					case 	PANEL_MODULE_3_SIREN_ALL_CL:	ActivateAllClear(3);  //  Danger Over siren for module 3 ->  ON
															break;	
				}		
						
			}
			break;
	}
	return 0;
}

//************************************************************************************************

int CVICALLBACK SirenTurnOff (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			switch(control)  
			{
				case PANEL_MODULE_1_SIREN_OFF:	SirenOff(1);      //  Siren for module 1 ->  Off
												break;
												
				case PANEL_MODULE_2_SIREN_OFF:	SirenOff(2);      //  Siren for module 2 ->  Off
												break;
												
				case PANEL_MODULE_3_SIREN_OFF:	SirenOff(3);      //  Siren for module 2 ->  Off
												break;
			}// end switch

		break;
	}
	return 0;
}

//************************************************************************************************

int CVICALLBACK clearSystemMessageWindow (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			ResetTextBox (panelHandle, PANEL_SYS_MESS, " \0");
			break;
			
	}
	return 0;
}

//-------------- END OF CALLBACK ROUTINES --------------------------------------------------------

void ActivateDangerSound(int UnitNumber)
{
	char *returnString = "";
	
	if(AdamUnit[UnitNumber].newTypeSiren == 1)  // new type siren
	{
		stat = ADAMTCP_SendReceive6KUDPCmd(AdamUnit[UnitNumber].ipaddr, "#011101", returnString);
		returnString[strlen(returnString)] = 0;
		
		if (stat < 0)
		{
			sprintf(errorMessage, " Problem activating Danger - Adam error %d IP:%s !\r\n", stat, AdamUnit[UnitNumber].ipaddr);
			displaySystemMessage(errorMessage);	
		}
		else
		{	
			stat = ADAMTCP_SendReceive6KUDPCmd(AdamUnit[UnitNumber].ipaddr, "#011501", returnString);
			returnString[strlen(returnString)] = 0;
			
				
			ResetTimer(panelHandle, shortDurationTimer[UnitNumber]);
			ResetTimer(panelHandle, longDurationTimer[UnitNumber]);

			stat = SetCtrlAttribute (panelHandle, shortDurationTimer[UnitNumber], ATTR_ENABLED, 1);
			stat = SetCtrlAttribute (panelHandle, longDurationTimer[UnitNumber], ATTR_ENABLED, 1);
		}
		
	}
	else
	{
		TyphoneSound[UnitNumber] = OFF;  // Start toggling with start value = off
		
		stat = ADAMTCP_SendReceive6KUDPCmd(AdamUnit[UnitNumber].ipaddr, "#011501", returnString);
		returnString[strlen(returnString)] = 0;
		
		
		ResetTimer(panelHandle, shortDurationTimer[UnitNumber]);
		ResetTimer(panelHandle, longDurationTimer[UnitNumber]);
		
		stat = SetCtrlAttribute (panelHandle, shortDurationTimer[UnitNumber], ATTR_ENABLED, 1);
		stat = SetCtrlAttribute (panelHandle, longDurationTimer[UnitNumber], ATTR_ENABLED, 1);
		
		
	}
	
}

//************************************************************************************************


void ActivateAllClear(int UnitNumber)
{
	char *returnValue = "";
	
	if (AdamUnit[UnitNumber].newTypeSiren == 1)  // new type siren
	  {	
		stat = ADAMTCP_SendReceive6KUDPCmd(AdamUnit[UnitNumber].ipaddr, "#011301", returnValue);  //   activate danger over relay (relay #3) 
		if (stat < 0)
			{ 
			  sprintf(errorMessage, " Problem activating Danger over - Adam error %d IP:%s !\r\n", stat, AdamUnit[UnitNumber].ipaddr);
			  displaySystemMessage(errorMessage);	
			  SetStatusFieldText(AdamUnit[UnitNumber].sirenDisplay, WHITE, "Off");
			}
		else
			{
		   		stat = ADAMTCP_SendReceive6KUDPCmd(AdamUnit[UnitNumber].ipaddr, "#011501", returnValue);
				returnValue[strlen(returnValue)] = 0;
								
				ResetTimer(panelHandle, shortDurationTimer[UnitNumber]);
				ResetTimer(panelHandle, longDurationTimer[UnitNumber]);
				
		   		stat = SetCtrlAttribute (panelHandle, shortDurationTimer[UnitNumber], ATTR_ENABLED, 1);
				stat = SetCtrlAttribute (panelHandle, longDurationTimer[UnitNumber], ATTR_ENABLED, 1);
			  
			}
      }
	else // old type siren
	  {
		stat = ADAMTCP_SendReceive6KUDPCmd(AdamUnit[UnitNumber].ipaddr, "#011101", returnValue); 			// Turn RL1(siren)on
		if (stat <0)
			{ 
			  sprintf(errorMessage, "Problem activating Danger over - Adam error %d IP:%s !\r\n", stat, AdamUnit[UnitNumber].ipaddr);
			  displaySystemMessage(errorMessage);
			  SetStatusFieldText(AdamUnit[UnitNumber].sirenDisplay, WHITE, "Off");
			}
		else  
		    { 
			  stat = ADAMTCP_SendReceive6KUDPCmd(AdamUnit[UnitNumber].ipaddr, "#011501", returnValue);
			  returnValue[strlen(returnValue)] = 0;
			  
			  stat = SetCtrlAttribute(panelHandle,shortDurationTimer[UnitNumber],ATTR_ENABLED, 0);
			  stat = SetCtrlAttribute (panelHandle, longDurationTimer[UnitNumber], ATTR_ENABLED, 1);
			  
			}
	  }
}	

//************************************************************************************************


void SirenOff(int UnitNumber)
{
	if(AdamUnit[UnitNumber].newTypeSiren == 1)  // new type siren
	{
		stat = ADAMTCP_SendReceive6KUDPCmd(AdamUnit[UnitNumber].ipaddr, "#011401", returnValue); //   activate stop signal relay (relay #4) 
		if (stat < 0)	
			{ 
			  sprintf(errorMessage, " Problem activating SirenOff - Adam error %d IP:%s !\r\n", stat, AdamUnit[UnitNumber].ipaddr);
			  displaySystemMessage(errorMessage);		
			}
		else	  
		    { 
			  ResetTimer(panelHandle, shortDurationTimer[UnitNumber]);
			  stat = SetCtrlAttribute (panelHandle, shortDurationTimer[UnitNumber], ATTR_ENABLED, 1);
			  
	    	  stat = ADAMTCP_SendReceive6KUDPCmd(AdamUnit[UnitNumber].ipaddr, "#011500", returnValue);
			  returnValue[strlen(returnValue)] = 0;
			}
	}
	else  // Old type siren
	{
		stat = ADAMTCP_SendReceive6KUDPCmd(AdamUnit[UnitNumber].ipaddr, "#011100", returnValue);			// Turn Relay 1(siren) for actual ADAM unit off
		if (stat < 0)
			{ 
			  sprintf(errorMessage, " Problem activating SirenOff - Adam error %d IP:%s !\r\n", stat, AdamUnit[UnitNumber].ipaddr);
              displaySystemMessage(errorMessage); 			
			}
		else
			{ 
			  AdamUnit[UnitNumber].siren = OFF; 
			  
			  stat = ADAMTCP_SendReceive6KUDPCmd(AdamUnit[UnitNumber].ipaddr, "#011500", returnValue);
			  returnValue[strlen(returnValue)] = 0;
			  
			  stat = SetCtrlAttribute (panelHandle, shortDurationTimer[UnitNumber], ATTR_ENABLED, 0);
			  stat = SetCtrlAttribute (panelHandle, longDurationTimer[UnitNumber], ATTR_ENABLED, 0);
			 
		    }
	}
}	

//************************************************************************************************

void WarningLight(int UnitNumber,BOOL selection)
{
	
    if (selection == ON)
	{ 
		stat = ADAMTCP_SendReceive6KUDPCmd(AdamUnit[UnitNumber].ipaddr, "#011001", returnValue); 	// Turn WT relay RL0 on
		if (stat < 0)
			{ 
			  sprintf(errorMessage, " Problem turning warning light On - Adam error %d IP:%s !\r\n", stat, AdamUnit[UnitNumber].ipaddr);
			  displaySystemMessage(errorMessage);
			}
	}
 	else
	{ 
		stat = ADAMTCP_SendReceive6KUDPCmd(AdamUnit[UnitNumber].ipaddr, "#011000", returnValue); 	// Turn WT relay RL0 off
		if (stat < 0)
			{ 
			  sprintf(errorMessage, " Problem turning warning light off - Adam error %d IP:%s !\r\n", stat, AdamUnit[UnitNumber].ipaddr);
			  displaySystemMessage(errorMessage);
			}
	}
}

//************************************************************************************************

void Auxiliary(int UnitNumber,BOOL selection)
{
  	//returnValue[0] = 0;
   	if (selection == ON)
		{
			stat = ADAMTCP_SendReceive6KUDPCmd(AdamUnit[UnitNumber].ipaddr, "#011201", returnValue);  	// Turn relay RL2 on
			
		 	if (stat < 0)
				{ 
			  	  sprintf(errorMessage, " Problem turning auxiliary relay on - Adam error %d IP:%s !\r\n", stat, AdamUnit[UnitNumber].ipaddr);
				  displaySystemMessage(errorMessage);
				}
		}
	else
		{
			stat = ADAMTCP_SendReceive6KUDPCmd(AdamUnit[UnitNumber].ipaddr, "#011200", returnValue);  	// Turn relay RL2 off
			returnValue[strlen(returnValue)] = 0;
		 	if (stat < 0)
				{ 
			  	  sprintf(errorMessage, " Problem turning auxiliary relay off - Adam error %d IP:%s !\r\n", stat, AdamUnit[UnitNumber].ipaddr);
				  displaySystemMessage(errorMessage);
				}
		}
}

//************************************************************************************************

void toggleSiren(int UnitNumber)
{
	
  		if (TyphoneSound[UnitNumber] == OFF) // if siren is "off"
		{	
			stat = ADAMTCP_SendReceive6KUDPCmd(AdamUnit[UnitNumber].ipaddr, "#011101", returnValue);  // Turn relay RL1 on, (meaning: turn siren on)
		    if (stat < 0)
			{
				sprintf(errorMessage, " Problem turning siren relay off - Adam error %d IP:%s !\r\n", stat, AdamUnit[UnitNumber].ipaddr);
				displaySystemMessage(errorMessage);
			}
		    else
			{ 
				TyphoneSound[UnitNumber] = ON;
			}
		}
		else if (TyphoneSound[UnitNumber] == ON) // if siren is "on"
		{
			stat = ADAMTCP_SendReceive6KUDPCmd(AdamUnit[UnitNumber].ipaddr, "#011100", returnValue);  // Turn relay RL1 off, (meaning: turn siren off)
			returnValue[strlen(returnValue)] = 0;
			TyphoneSound[UnitNumber] =  OFF;
		}
			
}   

//************************************************************************************************
//                         Timer Callback Functions
//************************************************************************************************

int CVICALLBACK LongDurationTimer (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)  // 30 sec timer
{
   
	char *returnString = "";
	int module_number = 0;
   
	switch (event)
	{
		case EVENT_TIMER_TICK:
			switch (control) // check which 30 s timer sent the event
			{
				case PANEL_MODULE_1_LONG_TIMER:	module_number = 1;
					                            break;
				case PANEL_MODULE_2_LONG_TIMER:	module_number = 2;
												break;
				case PANEL_MODULE_3_LONG_TIMER:	module_number = 3;
												break;
			}									
			stopTimersAndDisableSiren(module_number); // deactivate both timers  (short and long)
			SetStatusFieldText(AdamUnit[module_number].sirenDisplay, WHITE, "Off");
			
			
		    stat = ADAMTCP_SendReceive6KUDPCmd(AdamUnit[module_number].ipaddr, "#011500", returnString);
			
			
		break;
	}
	return 0;
}


//************************************************************************************************

int CVICALLBACK ShortDurationTimer (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)  // 2 sec timer
{
	switch (event)
	{
		case EVENT_TIMER_TICK:
			switch (control) // check which 2s timer sent the event
			{
				case PANEL_MODULE_1_SHORT_TIMER:	shortDurationHandlerForUnit(1);  // call handler for 2s timer #1
													break;
				case PANEL_MODULE_2_SHORT_TIMER:	shortDurationHandlerForUnit(2);	 // call handler for 2s timer #2
													break;
				case PANEL_MODULE_3_SHORT_TIMER:	shortDurationHandlerForUnit(3);  // call handler for 2s timer #3
													break;
			}
		break;
	}
	return 0;
}


//-------------------- End of timer callback routines --------------------------------------------  


//************************************************************************************************

void shortDurationHandlerForUnit(int UnitNumber)
{  
	if(AdamUnit[UnitNumber].newTypeSiren == TRUE)
	{
		stat = ADAMTCP_SendReceive6KUDPCmd(AdamUnit[UnitNumber].ipaddr, "#011100", returnValue);   // disable OLRT type siren "danger" activation signal by 2s timer
		stat = ADAMTCP_SendReceive6KUDPCmd(AdamUnit[UnitNumber].ipaddr, "#011300", returnValue);   // disable OLRT type siren "all clear" activation signal by 2 s timer
		stat = ADAMTCP_SendReceive6KUDPCmd(AdamUnit[UnitNumber].ipaddr, "#011400", returnValue);   // disable OLRT type siren "signal off" activation signal by 2s timer
	}
	else
	{ 	
		toggleSiren(UnitNumber);  // toggle siren on unit 
	}
}

//************************************************************************************************

void stopTimersAndDisableSiren(int UnitNumber) 
{
	stat = SetCtrlAttribute (panelHandle, shortDurationTimer[UnitNumber], ATTR_ENABLED, 0); 
	stat = SetCtrlAttribute (panelHandle, longDurationTimer[UnitNumber], ATTR_ENABLED, 0); 
	
	SirenOff(UnitNumber);
	TyphoneSound[UnitNumber] = OFF;
	
}


//************************************************************************************************

int CVICALLBACK displayTimer (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	int i = 0;
	switch (event)
	{
		case EVENT_TIMER_TICK:  // Update GUI  with actual status from all ADAM units
			  			  
              for(i = 0; i < NUMBER_OF_ADAM_UNITS; i++)
			  { 
			  	 catchStatusData(AdamUnit[i].digitalString,i);   	// Set status variables	
			     displayStatus(i);									// Display Status on GUI
				
			  }
			 
			break;
	}
	return 0;
	
}






