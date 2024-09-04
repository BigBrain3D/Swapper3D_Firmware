//Swapper3D FULL octoprint compatible firmware, Author: BigBrain3D, License: AGPLv3
#include <Adafruit_PWMServoDriver.h> 
#include <LiquidCrystal.h>
#include <stdarg.h>
#include <EEPROM.h>


const char firmwareVersion[] PROGMEM = "1.1.3";
const char msg_RetrieveCurrentFirmwareVersion[] PROGMEM = "RetrieveCurrentFirmwareVersion";

const double msPerStepAdjustment PROGMEM = 0.50; //this is divided by 100 and used to adjust the msPerStep for each step. make negative for faster than 7v servos, positive for slower 7v servos. 6V servos should be about 30% extra delay for each step.

const char msg_OCTOPRINT[] PROGMEM = "octoprint";
const char msg_OK[] PROGMEM = "ok";
const char msg_SWAPPER[] PROGMEM = "swapper";
const char msg_LOAD_INSERT[] PROGMEM = "load_insert";
const char msg_unload_CONNECT[] PROGMEM = "unload_connect";
const char msg_unload_PULLDOWN_LockingHeight[] PROGMEM = "unload_pulldown_lockingheight";
const char msg_unload_PULLDOWN_CuttingHeight[] PROGMEM = "unload_pulldown_cuttingheight";
const char msg_unload_DEPLOYCUTTER[] PROGMEM = "unload_deploycutter"; //unload_deploycutter1
const char msg_unload_DEPLOYCUTTER_CONNECT_WITH_FILAMENT_GUIDE[] PROGMEM = "unload_deploycutter_guide";
const char msg_cutter_CUT[] PROGMEM = "cutter_cut";
const char msg_cutter_OPEN[] PROGMEM = "cutter_open";
const char msg_unload_AVOIDBIN[] PROGMEM = "unload_AvoidBin";
const char msg_unload_stowCutter[] PROGMEM = "unload_stowCutter"; //unload_stowCutter0
const char msg_unload_stowInsert[] PROGMEM = "unload_stowInsert";
const char msg_unload_dumpWaste[] PROGMEM = "unload_dumpWaste";
const char msg_unloadED_MESSAGE[] PROGMEM = "unloaded_message";
const char msg_SWAP_MESSAGE[] PROGMEM = "swap_message";
const char msg_WIPER_DEPLOY[] PROGMEM = "wiper_deploy";
const char msg_WIPER_STOW[] PROGMEM = "wiper_stow";
const char msg_COMMAND_NOT_FOUND[] PROGMEM = "Command not found";
const char msg_READY_TO_SWAP[] PROGMEM = "Ready to Swap!";
const char msg_INSERT_NUMBER[] PROGMEM = "InsertNumber"; //Added Aug 23th 2024
const char msg_INSERT_FORMAT[] PROGMEM = "Insert: %d";
const char msg_INSERT_EMPTY[] PROGMEM = "Insert: Empty";
const char msg_CONNECT[] PROGMEM = "Connect";
const char msg_PULLDOWN[] PROGMEM = "Pulldown";
const char msg_DEPLOY_CUTTER[] PROGMEM = "Deploy cutter";
const char msg_LCD_cutter_cut[] PROGMEM = "Cut";
const char msg_LCD_cutter_open[] PROGMEM = "Open";
const char msg_AVOID_WASTE_BIN[] PROGMEM = "Avoid waste bin";
const char msg_STOW_CUTTER[] PROGMEM = "Stow cutter";
const char msg_STOW_INSERT[] PROGMEM = "Stow Insert";
const char msg_DUMP_WASTE[] PROGMEM = "Dump waste";
const char msg_DEPLOY_WIPER[] PROGMEM = "Deploy wiper";
const char msg_STOW_WIPER[] PROGMEM = "Stow wiper";
const char msg_WIPER_DEPLOYED[] PROGMEM = "Wiper deployed";
const char msg_SWAPPING_FORMAT_1[] PROGMEM = "Swapping -> %d";
const char msg_SWAPPING_FORMAT_2[] PROGMEM = "Swapping %d -> %d";
const char msg_ParityCheckFailed[] PROGMEM = "Parity check failed";
const char msg_LOADING[] PROGMEM = "Loading";
const char msg_EMPTY_LINE[] PROGMEM = "                ";
const char msg_ERROR_NOT_EMPTY[] PROGMEM = "ERROR: Not Empty!";
const char msg_S_TO_RETRY[] PROGMEM = "S to retry";
const char msg_ERROR_EMPTY[] PROGMEM = "ERROR: Empty!";
const char msg_ERROR_HAS_TOOL_BUT_SHOULD_BE_EMPTY[] PROGMEM = "ERROR 1";
const char msg_ERROR_IS_EMPTY_BUT_SHOULD_HAVE_TOOL[] PROGMEM = "ERROR 2";
const char msg_BUTTON_PRESSED[] PROGMEM = "Button pressed.";
const char msg_BUTTON_NOT_PRESSED[] PROGMEM = "Button NOT PRESSED.";
const char msg_ERROR_WAS_RESET[] PROGMEM = "Error was reset";
// const char msg_HeatNozzle[] PROGMEM = "heatnozzle"; //probably not going to use as the printer will display it's own message "heating..."
// const char msg_Heating[] PROGMEM = "Heating Nozzle...";
const char msg_WriteToEeprom_MajorVersion[] PROGMEM = "writemajor";
const char msg_WriteToEeprom_MinorVersion[] PROGMEM = "writeminor";
const char msg_WriteToEeprom_PatchVersion[] PROGMEM = "writepatch";
const char msg_ReadFromEeprom_MajorVersion[] PROGMEM = "readmajor";
const char msg_ReadFromEeprom_MinorVersion[] PROGMEM = "readminor";
const char msg_ReadFromEeprom_PatchVersion[] PROGMEM = "readpatch";
const char msg_BoreAlignOn[] PROGMEM = "borealignon";
const char msg_BoreAlignOff[] PROGMEM = "borealignoff";
const char msg_HomeToolRotate[] PROGMEM = "hometoolrotate"; //hometoolrotate0

const char servoInitial_0[] PROGMEM = "TR";
const char servoInitial_1[] PROGMEM = "TH";
const char servoInitial_2[] PROGMEM = "TL";
const char servoInitial_3[] PROGMEM = "QL";
const char servoInitial_4[] PROGMEM = "HR";
const char servoInitial_5[] PROGMEM = "CR";
const char servoInitial_6[] PROGMEM = "CA";
const char servoInitial_7[] PROGMEM = "WA";

const char* const servoInitials_table[] PROGMEM = {
  servoInitial_0,
  servoInitial_1,
  servoInitial_2,
  servoInitial_3,
  servoInitial_4,
  servoInitial_5,
  servoInitial_6,
  servoInitial_7
};
// ... add more phrases as needed ...




const int inputStringSize = 35;
char inputString[inputStringSize];
bool stringComplete = false;
int inputStringIndex = 0;
byte insertNumber = 0;

const Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

const int servo_pwm_max = 2900;// 2376;
const int servo_pwm_min = 600; //484;


//servos enums
const byte eePinNum = 0;
const byte eeMaxAngle = 1;
const byte eeCurrentAngle = 2;

//Servos
const byte numOfServos = 8;
const byte s_Tool_Rotate = 0; //360d //TR
const byte s_Tool_Height = 1; //TH
const byte s_Tool_Lock = 2; //TL
const byte s_QuickSwapHotend_Lock = 3; //QL
const byte s_ToolHolder_Rotate = 4; //360d //HR
const byte s_Cutter_Rotate = 5; //CR
const byte s_Cutter_Action = 6; //CA
const byte s_WasteCup_Action = 7; //WA


//_pin
//_maxAngle
//_currentAngle
const byte servos_pin[8] = {15,14,13,12,11,10,9,8}; //pin #, max angle, current angle //note: splitting this cannot lower memory since it cannot be a const since all elements are written at runtime
const int servos_maxAngle[8] = {360,180,280,180,360,180,180,280}; //pin #, max angle, current angle //note: splitting this cannot lower memory since it cannot be a const since all elements are written at runtime
int servos_currentAngle[8]; //pin #, max angle, current angle //note: splitting this cannot lower memory since it cannot be a const since all elements are written at runtime


//LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//swap tools process
// const byte eeps_servoNumber = 0;
// const byte eeps_degrees = 1;
// const byte eeps_MsDelayPerDegreeMoved = 2;
// const byte eeps_MsDelayAfterCommandSent = 3;
// const byte eeps_StepType = 4;



//step types:
const byte eeRegularStep = 0;
const byte eeButtonCheck_Empty = 1;
const byte eeButtonCheck_HoldingTool = 2;
//const byte eeExtrude = 3;
//const byte eeRetract = 4;
const byte eeToolHolderPrepRotate = 5; 
const byte eeAddHalfDegreePrecision = 6;
const byte eeToolHolderPrepUNrotate = 7; 

//
const byte eeToolHolderPrepRotate_degrees = 3;
const byte eeToolHolderPrepUNrotate_degrees = 3; //4; //1; //2; //3; //3; //1; //3; //6; //3;

//_servoNumber
//_degrees
//_msDelayPerDegreeMoved
//_msDelayAfterCommandSent
//_stepType
const byte numOfProcessSteps_LoadTool = 21; //***change this if adding or removing process steps
byte ProcessSteps_LoadTool_servoNumber[numOfProcessSteps_LoadTool]={0};
int ProcessSteps_LoadTool_degrees[numOfProcessSteps_LoadTool]={0}; //degrees
byte ProcessSteps_LoadTool_msDelayPerDegreeMoved[numOfProcessSteps_LoadTool]={0}; 
byte ProcessSteps_LoadTool_msDelayAfterCommandSent[numOfProcessSteps_LoadTool]={0}; 
byte ProcessSteps_LoadTool_stepType[numOfProcessSteps_LoadTool]={0};


//30 total steps to Unload
const byte numOfProcessSteps_unload_connect = 6; //***change this if adding or removing process steps
const byte numOfProcessSteps_unload_pulldown_LockingHeight = 2; //***change this if adding or removing process steps
const byte numOfProcessSteps_unload_pulldown_CuttingHeight = 2; //***change this if adding or removing process steps
const byte numOfProcessSteps_unload_deployCutter = 1; //***change this if adding or removing process steps
const byte numOfProcessSteps_unload_deployCutter_ConnectWithFilamentGuide = 1; //Palette ONLY //***change this if adding or removing process steps
const byte numOfProcessSteps_cutter_cut = 1; //***change this if adding or removing process steps
const byte numOfProcessSteps_cutter_open = 1; //***change this if adding or removing process steps
const byte numOfProcessSteps_unload_avoidBin = 2; //***change this if adding or removing process steps
const byte numOfProcessSteps_unload_stowCutter = 1; //***change this if adding or removing process steps
const byte numOfProcessSteps_unload_stowInsert = 12; //***change this if adding or removing process steps
const byte numOfProcessSteps_unload_dumpWaste = 2; //***change this if adding or removing process steps

//_servoNumber
byte ProcessSteps_unload_connect_servoNumber[numOfProcessSteps_unload_connect]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_pulldown_LockingHeight_servoNumber[numOfProcessSteps_unload_pulldown_LockingHeight]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_pulldown_CuttingHeight_servoNumber[numOfProcessSteps_unload_pulldown_CuttingHeight]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_deployCutter_servoNumber[numOfProcessSteps_unload_deployCutter]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_deployCutter_ConnectWithFilamentGuide_servoNumber[numOfProcessSteps_unload_deployCutter_ConnectWithFilamentGuide]={0}; //Palette ONLY //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_cutter_cut_servoNumber[numOfProcessSteps_cutter_cut]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_cutter_open_servoNumber[numOfProcessSteps_cutter_cut]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_avoidBin_servoNumber[numOfProcessSteps_unload_avoidBin]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_stowCutter_servoNumber[numOfProcessSteps_unload_stowCutter]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_stowInsert_servoNumber[numOfProcessSteps_unload_stowInsert]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_dumpWaste_servoNumber[numOfProcessSteps_unload_dumpWaste]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType

//_degrees
int ProcessSteps_unload_connect_degrees[numOfProcessSteps_unload_connect]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
int ProcessSteps_unload_pulldown_LockingHeight_degrees[numOfProcessSteps_unload_pulldown_LockingHeight]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
int ProcessSteps_unload_pulldown_CuttingHeight_degrees[numOfProcessSteps_unload_pulldown_CuttingHeight]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
int ProcessSteps_unload_deployCutter_degrees[numOfProcessSteps_unload_deployCutter]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
int ProcessSteps_unload_deployCutter_ConnectWithFilamentGuide_degrees[numOfProcessSteps_unload_deployCutter_ConnectWithFilamentGuide]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
int ProcessSteps_cutter_cut_degrees[numOfProcessSteps_cutter_cut]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
int ProcessSteps_cutter_open_degrees[numOfProcessSteps_cutter_cut]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
int ProcessSteps_unload_avoidBin_degrees[numOfProcessSteps_unload_avoidBin]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
int ProcessSteps_unload_stowCutter_degrees[numOfProcessSteps_unload_stowCutter]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
int ProcessSteps_unload_stowInsert_degrees[numOfProcessSteps_unload_stowInsert]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
int ProcessSteps_unload_dumpWaste_degrees[numOfProcessSteps_unload_dumpWaste]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType

//_msDelayPerDegree
byte ProcessSteps_unload_connect_msDelayPerDegreeMoved[numOfProcessSteps_unload_connect]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_pulldown_LockingHeight_msDelayPerDegreeMoved[numOfProcessSteps_unload_pulldown_LockingHeight]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_pulldown_CuttingHeight_msDelayPerDegreeMoved[numOfProcessSteps_unload_pulldown_CuttingHeight]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_deployCutter_msDelayPerDegreeMoved[numOfProcessSteps_unload_deployCutter]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_deployCutter_ConnectWithFilamentGuide_msDelayPerDegreeMoved[numOfProcessSteps_unload_deployCutter_ConnectWithFilamentGuide]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_cutter_cut_msDelayPerDegreeMoved[numOfProcessSteps_cutter_cut]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_cutter_open_msDelayPerDegreeMoved[numOfProcessSteps_cutter_cut]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_avoidBin_msDelayPerDegreeMoved[numOfProcessSteps_unload_avoidBin]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_stowCutter_msDelayPerDegreeMoved[numOfProcessSteps_unload_stowCutter]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_stowInsert_msDelayPerDegreeMoved[numOfProcessSteps_unload_stowInsert]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_dumpWaste_msDelayPerDegreeMoved[numOfProcessSteps_unload_dumpWaste]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType

//_msDelayAfterCommandSent
byte ProcessSteps_unload_connect_msDelayAfterCommandSent[numOfProcessSteps_unload_connect]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_pulldown_LockingHeight_msDelayAfterCommandSent[numOfProcessSteps_unload_pulldown_LockingHeight]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_pulldown_CuttingHeight_msDelayAfterCommandSent[numOfProcessSteps_unload_pulldown_CuttingHeight]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_deployCutter_msDelayAfterCommandSent[numOfProcessSteps_unload_deployCutter]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_deployCutter_ConnectWithFilamentGuide_msDelayAfterCommandSent[numOfProcessSteps_unload_deployCutter_ConnectWithFilamentGuide]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_cutter_cut_msDelayAfterCommandSent[numOfProcessSteps_cutter_cut]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_cutter_open_msDelayAfterCommandSent[numOfProcessSteps_cutter_cut]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_avoidBin_msDelayAfterCommandSent[numOfProcessSteps_unload_avoidBin]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_stowCutter_msDelayAfterCommandSent[numOfProcessSteps_unload_stowCutter]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_stowInsert_msDelayAfterCommandSent[numOfProcessSteps_unload_stowInsert]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_dumpWaste_msDelayAfterCommandSent[numOfProcessSteps_unload_dumpWaste]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType

//_stepType
byte ProcessSteps_unload_connect_stepType[numOfProcessSteps_unload_connect]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_pulldown_LockingHeight_stepType[numOfProcessSteps_unload_pulldown_LockingHeight]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_pulldown_CuttingHeight_stepType[numOfProcessSteps_unload_pulldown_CuttingHeight]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_deployCutter_stepType[numOfProcessSteps_unload_deployCutter]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_deployCutter_ConnectWithFilamentGuide_stepType[numOfProcessSteps_unload_deployCutter_ConnectWithFilamentGuide]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_cutter_cut_stepType[numOfProcessSteps_cutter_cut]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_cutter_open_stepType[numOfProcessSteps_cutter_cut]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_avoidBin_stepType[numOfProcessSteps_unload_avoidBin]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_stowCutter_stepType[numOfProcessSteps_unload_stowCutter]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_stowInsert_stepType[numOfProcessSteps_unload_stowInsert]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType
byte ProcessSteps_unload_dumpWaste_stepType[numOfProcessSteps_unload_dumpWaste]={0}; //servo number, msDelayPerDegree, msDelayAfterCommandSent, stepType


const byte msDelayAfterCommandSent_Buffer = 0; //100; was 100 before increasing all msDelayAfterCommandSent for each step by 100ms to enable fine grain control over delays at the step level //50 //in milliseconds //extra ms delay 


//menu
//menus enums
const byte eeOnce = 0;
const byte eeEndless = 1;
const byte eeLoadTool = 0;
const byte eeunload = 1;
const byte eeAutomaticProcess = 0;
const byte eeManualProcess = 1;

//error state button press
const bool ErrorCheckingEnabed = true; //set to false for testing purposes so that the Swap can be run without inserts
const byte CheckButton_Pin = 13; //digital pin thirteen(13)
bool EmptyButtonState = 0; //remember the initial button state on boot-up; that is now the 'Empty Insert' state
bool InErrorState = false;
//int CurrentProcessType = eeLoadTool;//load/unload

//tool holder rotation and selection
bool firstPositionCommandGiven = false;
const int servoMinAngle = 0;
float pos_Tool_Holder_FirstTool = 0;
bool toolIsLoaded = false;
int CurrentTool = 0;

int pos_Tool_Lock_Unlocked = 0;
int pos_Tool_Lock_Locked = 0;
int pos_Cutter_Rotate_Stowed = 0;
int pos_QuickSwapHotend_Lock_Locked = 0;
int pos_QuickSwapHotend_Lock_Unlocked = 0;
bool LockToolPartWayThru = false;
const int numMsUntilLock = 50; //100; //200; //10ms per degree currently

int pos_Tool_Rotate_UnderExtruder_ConnectWithNozzleCollar = 0;
int pos_Tool_Rotate_ButtingTheToolToTheLeftOfNext = 0;
int Homing_offset_ToolRotate = 0;


const char* getServoInitials(byte whichServo) {
  static char buffer[3];
  strcpy_P(buffer, (char*)pgm_read_word(&(servoInitials_table[whichServo])));
  return buffer;
}

int checkParity(char* message) {
  int count = 0;
  for (int i = 0; i < strlen(message); i++) {
    int value = message[i];
    while (value) {
      count++;
      value = value & (value - 1);
    }
  }
  return ~count & 1; // returns 1 for odd parity, 0 for even parity
}


void printWithParity_Combined(char inputMessage_TextPart[], const char* PROGMEM message) {
  char buffer[50];
  
  // Copy the inputMessage_TextPart to the buffer
  strncpy(buffer, inputMessage_TextPart, sizeof(buffer) - 1);
  buffer[sizeof(buffer) - 1] = '\0';  // Ensure null-termination
  
  // Concatenate the underscore and the message (e.g., "_ok")
  strncat(buffer, "_", sizeof(buffer) - strlen(buffer) - 1);
  strncat_P(buffer, message, sizeof(buffer) - strlen(buffer) - 1);
  
  // Calculate parity of the combined string (excluding the parity itself)
  int parity = checkParity(buffer);
  
  // Prepare the final output string in the format "text_okParity"
  char output[60];
  snprintf(output, sizeof(output), "%s%d", buffer, parity);
  
  // Print the final output
  Serial.println(output);
}

void printWithParity_P(const char* PROGMEM message) {
  char buffer[50];
  strcpy_P(buffer, message);
  int parity = checkParity(buffer);
  char output[60];
  snprintf(output, sizeof(output), "%s%d", buffer, parity);
  
  Serial.println(output);
}

void printWithParity(char* message) {
	int parity = checkParity(message);
	char output[60];
	snprintf(output, sizeof(output), "%s%d", message, parity);

	Serial.println(output);
  }

void PrintIntWithParityAsChar(int number) {
  char numberStr[10];
  itoa(number, numberStr, 10); // Convert the integer to a string
  int parity = checkParity(numberStr); // Calculate the parity of the string
  char output[60];
  snprintf(output, sizeof(output), "%s%d", numberStr, parity);
  Serial.println(output);
}

void updateLCD(const char* PROGMEM message1, const char* PROGMEM message2, ...) {
  char buffer1[50];
  char buffer2[50];
  strcpy_P(buffer1, message1);
  va_list args1;
  va_start(args1, message2);
  vsprintf_P(buffer1, message1, args1);
  va_end(args1);

  strcpy_P(buffer2, message2);
  va_list args2;
  va_start(args2, message2);
  vsprintf_P(buffer2, message2, args2);
  va_end(args2);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(buffer1);
  lcd.setCursor(0, 1);
  lcd.print(buffer2);
}

void updateLCD_line1(const char* PROGMEM message, ...) {
  char buffer[50];
  strcpy_P(buffer, message);

  va_list args;
  va_start(args, message);
  vsprintf_P(buffer, message, args);
  va_end(args);
  
  lcd.setCursor(0, 0);
  for (int i = 0; i < 16; i++) {
    lcd.write(' ');  // Clear the line by writing spaces
  }
  lcd.setCursor(0, 0);
  lcd.print(buffer);
}

void updateLCD_line2(byte servo, int angle) {
  const char* servo_name = getServoInitials(servo); // Assuming getServoName(0) returns "Tool_Rotate"

  char angle_str[10];
  itoa(angle, angle_str, 10);  // Convert angle to string

  char formatted_message[50];
  sprintf(formatted_message, "S: %s, A: %s", servo_name, angle_str);

  // Now print the message on the LCD
  lcd.setCursor(0, 1);
  for (int i = 0; i < 16; i++) {
    lcd.write(' ');  // Clear the line by writing spaces
  }
  lcd.setCursor(0, 1);
  lcd.print(formatted_message);
}

void updateLCD_line2_with_step(byte servo, int angle, int step) {
  char step_str[10];
  itoa(step, step_str, 10);  // Convert step to string, adding 1 to start from 1

  const char* servo_name = getServoInitials(servo); 

  char angle_str[10];
  itoa(angle, angle_str, 10);  // Convert angle to string

  char formatted_message[50];
  sprintf(formatted_message, "%s, S: %s, A: %s", step_str, servo_name, angle_str);

  // Now print the message on the LCD
  lcd.setCursor(0, 1);
  for (int i = 0; i < 16; i++) {
    lcd.write(' ');  // Clear the line by writing spaces
  }
  lcd.print(formatted_message);

  // Print the same message to the serial port
  printWithParity(formatted_message);
}

char* generateMessage(byte stepNumber, byte servo, int angle) {
  const char* servo_name = getServoInitials(servo); // Assuming getServoName(0) returns "Tool_Rotate"

  char angle_str[10];
  itoa(angle, angle_str, 10);  // Convert angle to string



  static char formatted_message[50];
 sprintf(formatted_message, "ST:%d,  S:%s,  A:%s", stepNumber, servo_name, angle_str);

//sprintf(formatted_message, "%s", servo_name);
  return formatted_message;
}

int fMap(float desiredAngle, int MinAngle, int MaxAngle, int minPWM, int maxPWM){  
  int angleRange = MaxAngle - MinAngle;
  int pwmRange = maxPWM - minPWM;
  
  float desiredAnglePercentOfRange = desiredAngle / angleRange;
  float pwmByDesiredAngle = float(pwmRange) * float(desiredAnglePercentOfRange) + float(minPWM);

  return pwmByDesiredAngle;
}


void ToolHolder_AlignToThisTool(int SelectThisTool){
	int localPulseLength = 0;
	int msDelayPerToolPostionToCompleteMovement = 75; //50; 
	int msDelayPadding = 50;
	int msDelayUntilRotationComplete = 0; //total ms to delay for the current tool holder rotation
	float degreesPerTool = 14.4; //14.72; //computed angle is 14.4d per spline calced as 25T splines 360/25=14.4 //14.72; //last 14.80 //this works for 25T's but barely: 14.85; //19:14.95 slightly too much;
	float degreesPositionOfSelectedTool = (float)SelectThisTool * degreesPerTool;

	
	servos_currentAngle[s_ToolHolder_Rotate] = degreesPositionOfSelectedTool + pos_Tool_Holder_FirstTool;

	localPulseLength = fMap(servos_currentAngle[s_ToolHolder_Rotate], servoMinAngle, servos_maxAngle[s_ToolHolder_Rotate], servo_pwm_min, servo_pwm_max);

	pwm.setPWM(servos_pin[s_ToolHolder_Rotate], 0, localPulseLength);  
	
	msDelayUntilRotationComplete = abs(CurrentTool - SelectThisTool) * msDelayPerToolPostionToCompleteMovement + msDelayPadding;

	delay(msDelayUntilRotationComplete);

	CurrentTool = SelectThisTool;  
}

void CheckToolRotateHomePosition()
{		
	int Adjustment_Tool_Rotate = map(EEPROM.read(0), 100, 140, -20, 20);
	int Adjustment_Tool_Height = map(EEPROM.read(1), 100, 140, -20, 20);
	int pos_Tool_Rotate_HomeStart = pos_Tool_Rotate_ButtingTheToolToTheLeftOfNext - 10; // this is the position of TR at the start of the homing routine
	//the expectedDegreesFromHomeStart is the number to adjust if rotate isn't working correctly.
	//because then it will automatically adjust to this number on homing it and startup.
	int expectedDegreesFromHomeStart = 10; //9; //6; //this is the number of degrees that we expect to move from the home start postion to the homing switch (re: check button) being pressed
	int pos_Tool_Height_Homing = 105 + Adjustment_Tool_Height;
	int AngleToTest = pos_Tool_Rotate_HomeStart;
	int maxHomingAngle = AngleToTest - 21;
	bool homeFound = false;
	int suggested_Homing_offset_ToolRotate = 0;

	//set TR to Start Position
	int pulselength = map(pos_Tool_Rotate_HomeStart, 0, servos_maxAngle[s_Tool_Rotate], servo_pwm_min, servo_pwm_max);
	pwm.setPWM(servos_pin[s_Tool_Rotate], 0, pulselength);	

	
	//set TH to homing position
	pulselength = map(pos_Tool_Height_Homing, 0, servos_maxAngle[s_Tool_Height], servo_pwm_min, servo_pwm_max);
	pwm.setPWM(servos_pin[s_Tool_Height], 0, pulselength);	
	delay(100);
	
	// Serial.print("Homing_offset_ToolRotate Initial Value:");
	// Serial.println(Homing_offset_ToolRotate);
	
	// Serial.print("AngleToTest:");
	// Serial.println(AngleToTest);
	
	// Serial.print("maxHomingAngle:");
	// Serial.println(maxHomingAngle); 
	
	//increment the TR angle by 1 at a time
	//check that the button is pressed
	//if the button is press exit loop
	//output the home position serial
	while(AngleToTest > maxHomingAngle)
	{	
		// Serial.print("AngleToTest:");
		// Serial.println(AngleToTest);
		
		pulselength = map(AngleToTest, 0, servos_maxAngle[s_Tool_Rotate], servo_pwm_min, servo_pwm_max);
		pwm.setPWM(servos_pin[s_Tool_Rotate], 0, pulselength);	
		delay(500);
		
		//if the check button is pressed then the TR is homed
		if(CheckButton_Pressed())
		{
			// Serial.print("button pressed");

			homeFound = true;
			break;
		}
		
		 AngleToTest--;
	}
	
	
	//record the degrees moved
	//compare to expected
	//adjust as necessary
	if (homeFound)
	{
		// Serial.println("Home found");
		// Serial.print("Home TR position:");
		// Serial.println(AngleToTest);
		
		// Serial.print("Degrees from start:");
		// Serial.println(pos_Tool_Rotate_HomeStart - AngleToTest);
		
		// Serial.print("Offset from expected:");
		// Serial.println((pos_Tool_Rotate_HomeStart - AngleToTest - expectedDegreesFromHomeStart));
		
		
		// Serial.print("Homing_offset_ToolRotate Adjusted:");
		// Serial.println(Homing_offset_ToolRotate);
		
		suggested_Homing_offset_ToolRotate -= (pos_Tool_Rotate_HomeStart - AngleToTest - expectedDegreesFromHomeStart);
		
		if(suggested_Homing_offset_ToolRotate != 0)
		{
			printWithParity_P(PSTR("HomeAdjusted:"));
			PrintIntWithParityAsChar(suggested_Homing_offset_ToolRotate);
			// Serial.println(suggested_Homing_offset_ToolRotate);

			Homing_offset_ToolRotate = suggested_Homing_offset_ToolRotate;
		}
		else
		{			
			printWithParity_P(PSTR("No Adjust"));
			// Serial.print("Homing_offset_ToolRotate Adjusted:");
			// Serial.println(Homing_offset_ToolRotate);
			// Serial.println("No Adjustment needed; Current offset is correct.");
		}			
		
		
	}
	//Error because home was not found 
	else
	{		
		printWithParity_P(PSTR("TRHomeNotFound"));
		// Serial.println("ERROR, Home NOT found");
		// Serial.print("Last angle checked:");
		// Serial.println(AngleToTest);
	}

		
	//set TR to Start Position
	pulselength = map(pos_Tool_Rotate_HomeStart, 0, servos_maxAngle[s_Tool_Rotate], servo_pwm_min, servo_pwm_max);
	pwm.setPWM(servos_pin[s_Tool_Rotate], 0, pulselength);	
	delay(50);
	
	//refresh the tool rotate positions from eeprom
	SetSwapStepLocations();

	//reset the 
	SetServosToStartPositions();
}

void SetServosToStartPositions()
{	
	int pulselength = 0;

	for(int i; i < 8; i++)
	{
		pulselength = map(servos_currentAngle[i], 0, servos_maxAngle[i], servo_pwm_min, servo_pwm_max);
		pwm.setPWM(servos_pin[i], 0, pulselength);	
		delay(100);
	}

	delay(300);
}

void SetSwapStepLocations(){
	int pulselength = 0;
	
	//changed out servo all should be off by same angle over last servo	
	int Adjustment_Tool_Rotate = map(EEPROM.read(0), 100, 140, -20, 20);
	int Adjustment_Tool_Height = map(EEPROM.read(1), 100, 140, -20, 20);
	int Adjustment_Tool_Lock = map(EEPROM.read(2), 100, 140, -20, 20);
	int Adjustment_QuickSwapHotend_Lock = map(EEPROM.read(3), 100, 140, -20, 20);
	int Adjustment_Holder_Rotate = map(EEPROM.read(4), 100, 140, -20, 20); 
	int Adjustment_Cutter_Rotate = map(EEPROM.read(5), 100, 140, -20, 20);
	int Adjustment_Cutter_Action = map(EEPROM.read(6), 100, 140, -20, 20);
	int Adjustment_WasteCup_Action = map(EEPROM.read(7), 100, 140, -20, 20);


	//apply adjustment from EEPROM
	// pos_Tool_Holder_FirstTool = pos_Tool_Holder_FirstTool + Adjustment_Holder_Rotate; //this was causeing a stacking error
	pos_Tool_Holder_FirstTool = 15 + Adjustment_Holder_Rotate; //11.7; //11; //12; //2; //note: eeprom offset is added at runtime so cannot be const
	
	
//position variables
	//Servo 0
	//**** Tool Rotate (TR) ****
	//next line is starting first 1st position
	pos_Tool_Rotate_ButtingTheToolToTheLeftOfNext = 105 + Adjustment_Tool_Rotate + Homing_offset_ToolRotate; //106 //105 //103 //102; //104; //103;
	int pos_Tool_Rotate_LeftOfToolInHolder = 100 + Adjustment_Tool_Rotate + Homing_offset_ToolRotate; //100 //98 //97 //98; //99; //101;//101 to try and deal with the single nozzle load failure //100; //102; //101;//103;//95; //SetupMode
	int pos_Tool_Rotate_UnderToolHolder_ConnectWithNozzleCollar = 93 + Adjustment_Tool_Rotate + Homing_offset_ToolRotate; //97 //96 //95 //80, 85; //90; //83;//76 = 11
	int pos_Tool_Rotate_UnderToolHolder_CenteredUnderCurrentTool = 97 + Adjustment_Tool_Rotate + Homing_offset_ToolRotate; //99 //97 //98, 97; //95;
	int pos_Tool_Rotate_UnderToolHolder_ConnectWithNozzleCollar_NoPressure = pos_Tool_Rotate_UnderToolHolder_CenteredUnderCurrentTool; //95; //97; //98; //96; //91; //86 = 5
	int pos_Tool_Rotate_ReleaseFromHotendUnderToolHolder = 109 + Adjustment_Tool_Rotate + Homing_offset_ToolRotate; //108 //106 //104;
	int pos_Tool_Rotate_BetweenBothNozzles = 104 + Adjustment_Tool_Rotate + Homing_offset_ToolRotate; //was 103
	int pos_Tool_Rotate_ButtonToolCheck = 72 + Adjustment_Tool_Rotate + Homing_offset_ToolRotate; //74, 75, 74, 72, 75, 70 //72; //74; //75; //68;
	int pos_Tool_Rotate_UnderExtruder_JerkConnectWithNozzleCollar = 281 + Adjustment_Tool_Rotate + Homing_offset_ToolRotate; //280 //275, 282; //283; //284; //265; //270; //274;
	pos_Tool_Rotate_UnderExtruder_ConnectWithNozzleCollar = 283 + Adjustment_Tool_Rotate + Homing_offset_ToolRotate; //285, 286, 285 //284; 286; //Why did this change???!?!??? 285; //283; //284; //283; // 282; //283; //284; //285; //283; //287; //285; //278;
	int pos_Tool_Rotate_UnderExtruder_JerkReleaseFromNozzleCollar = 293 + Adjustment_Tool_Rotate + Homing_offset_ToolRotate; //292 //293 291; // 310; //305; //297;
	int pos_Tool_Rotate_UnderExtruder_ReleasedFromNozzleCollar = 291 + Adjustment_Tool_Rotate + Homing_offset_ToolRotate; //290 //291, 293 291; //285;
	int pos_Tool_Rotate_UnderExtruder_UnderTaperButPastNozzle = 291 + Adjustment_Tool_Rotate + Homing_offset_ToolRotate; //had to increase a little because of the TR homing //289 //new
	int pos_Tool_Rotate_UnderExtruder_TouchSideOfNozzle = 285 + Adjustment_Tool_Rotate + Homing_offset_ToolRotate; //new
	
	int pos_Tool_Rotate_WaitingForunloadCommand = 148 + Adjustment_Tool_Rotate + Homing_offset_ToolRotate; //147 //140;
	int pos_Tool_Rotate_PastWasteCup = 259 + Adjustment_Tool_Rotate + Homing_offset_ToolRotate; //258 //251;

	//**** Tool Height (TH) ****
	//Servo 1
	//next line is starting first 1st position
	//increasing this will lower all up/down moves. decreasing this will raise all up/down moves
	int pos_Tool_Height_LowestLevel = 129 + Adjustment_Tool_Height; //125 //121 //Servo change 126; // below 126 it causes the servo to stall.  127; //126; //119; //117
	int pos_Tool_Height_ButtingTheToolToTheLeftOfNext = 50 + Adjustment_Tool_Height; //44 //40 //37, Servo change 42;//we want to be at the height of the hex on the right nozzle //39; //40; //41;
	int pos_Tool_Height_NozzleCollarLevel = 42 + Adjustment_Tool_Height; //33 //37 //33 //30, Servo change 35; //38; //36; //29;
	int pos_Tool_Height_ToolLoweredButStillInHolder = 59 + Adjustment_Tool_Height; //54 //Servo change 49; //42;
	int pos_Tool_Height_ToolFullyInsertedInHolder = 43 + Adjustment_Tool_Height; //31 //27 //25, 22, Servo change 27; //29; //20; //13;
	int pos_Tool_Height_ToolFullyInsertedInHolder_NoPressure = 47 + Adjustment_Tool_Height; //38 //34 //35, 32, Servo change 37; //36; //40; //29; //SetupMode
	int pos_Tool_Height_ToolFullyInsertedIntoExtruder = 42 + Adjustment_Tool_Height; //41 //37 //39, 40, 39, 33, Servo change 38; //40; //42; //35; //28;
	int pos_Tool_Height_ToolFullyInsertedIntoExtruder_ScrappingHotendMildPressure = 45 + Adjustment_Tool_Height;//44 //40 //Servo change 43; //42; //41; //42; //40; //42; //clunking? maybe 41?
	int pos_Tool_Height_ToolFullyInsertedIntoExtruder_NoPressure = 51 + Adjustment_Tool_Height; //43 //42 //38 //39; 38; //Servo change 43; //44; //42; // 45; // 42; //35;
	int pos_Tool_Height_ToolLoweredButStillInExtruder = 72 + Adjustment_Tool_Height;//Aug8:52 //48 //Servo change 53; //53 moves up until the hotend tapper is past the edge of the inner bore of the heater block//56; //49;
	int pos_Tool_Height_ToolLowered_CuttingHeight = 122 + Adjustment_Tool_Height; //122 //118 //117 //125 //113;//111, 108, 109; 107; //higher number moves down away from cutter leaving longer filament strand sticking out. //108; //Servo change 112;//works with new cutting sequence //110; //108; at 108 there was a single instance of cutting the heatbreak copper and it was ruined. //107; //99;
	// int pos_Tool_Height_ToolLowered_BelowCutterJaws = 116 + Adjustment_Tool_Height; //116 //112 //Servo change 117; //110;
	int pos_Tool_Height_SideOfNozzleInExtruder = 54 + Adjustment_Tool_Height; //new
	int pos_Tool_Height_CollarOfNozzleInExtruder = 50 + Adjustment_Tool_Height; //new

	//**** Tool Lock (TL) (micro 280d servo) ****
	//Servo 2
	//next line is starting first 1st position
	pos_Tool_Lock_Unlocked = 193 + Adjustment_Tool_Lock; //195, 180
	pos_Tool_Lock_Locked = 102 + Adjustment_Tool_Lock; //112 //8 //7; //8; //standard move172degrees. 8;//now precision move +.5 //9;//8;//9; //8; //9; 13; 

	//**** QuickSwap- Hotend Lock (QL) ****
	//Servo 3
	//next line is 1st starting first 1st position
	pos_QuickSwapHotend_Lock_Locked = 70 + Adjustment_QuickSwapHotend_Lock; //70, 0
	pos_QuickSwapHotend_Lock_Unlocked = 77; + Adjustment_QuickSwapHotend_Lock; //100 This changed a TON on Sep 3rd 2024, maybe the end effector was bent? //104, 34 //32; //33; //34; //35; //29;

	//**** Cutter Rotate (CR) ****
	//Servo 5
	//next line is starting first 1st position
	pos_Cutter_Rotate_Stowed = 15 + Adjustment_Cutter_Rotate; //27 //25;//1;//must be greater than 0. 0 causes major jittering. Something about 25 works better than 26. 26 had lots of jitter.
	//pos_Cutter_Rotate_Cutting: 120 rotates to the cross. 
  //pos_Cutter_Rotate_Cutting: 128 is max rotation before guard strikes the frame. 
  //pos_Cutter_Rotate_Cutting: 123 rotates past the cross to the straight part of the blade
  // Sep 2nd 2024: this value isn't used anymore, look in void unload_deployCutter //int pos_Cutter_Rotate_Cutting = 123 + Adjustment_Cutter_Rotate; //123 //122; //125; //126, 124, 122; //121; //122; //121; //122; //120; //124; //126; //127; //128;//get closer but lower the cutting height a little //126; //127; 127 is too close //126; //or maybe 127? //121; //99;
	int pos_Cutter_Rotate_ConnectWithFilamentGuide = 123 + Adjustment_Cutter_Rotate; //127 //122; //125; //126, 124, 122; //121; //122; //121; //122; //120; //124; //126; //127; //128;//get closer but lower the cutting height a little //126; //127; 127 is too close //126; //or maybe 127? //121; //99;

	//**** Cutter Action (CA) ****
	//Servo 6
	//next line is starting first 1st position
	int pos_Cutter_Action_Open = 175 + Adjustment_Cutter_Action; //160
  //pos_Cutter_Action_Cut: 15 is the max if the guard is cut back out of the way, at that poing the cutter handle will impinge on the cutter arm
  //pos_Cutter_Action_Cut: 33 is the max with the guard and right most screw in place
  //pos_Cutter_Action_Cut: 28 is the max if the right most screw is removed
	int pos_Cutter_Action_Cut = 33 + Adjustment_Cutter_Action; //33 #on Aug 19th 2024 changed from 33 to 30, if the guard screw is removed it could be reduced to 28-29, and if the gaurd was cut it could be reduced to 15.  //15 //0 //20; //40; //6; //7; //21;

	//**** Waste Cup Action (WA) (micro 280d servo) ****
	//Servo 7
	//next line is starting first 1st position
	int pos_WasteCup_Action_Fill = 173 + Adjustment_WasteCup_Action; //171, 110, 0
	int pos_WasteCup_Action_Dump = 164 + Adjustment_WasteCup_Action; //99, 107


	//void SetServoStartingPositions(){
	//pin #, max angle, start angle, current angle
	servos_currentAngle[s_Tool_Rotate] = pos_Tool_Rotate_ButtingTheToolToTheLeftOfNext;
	servos_currentAngle[s_Tool_Height] = pos_Tool_Height_LowestLevel;
	servos_currentAngle[s_Tool_Lock] = pos_Tool_Lock_Unlocked;
	servos_currentAngle[s_QuickSwapHotend_Lock] = pos_QuickSwapHotend_Lock_Locked;//
	servos_currentAngle[s_ToolHolder_Rotate] = pos_Tool_Holder_FirstTool;//
	servos_currentAngle[s_Cutter_Rotate] = pos_Cutter_Rotate_Stowed;//
	servos_currentAngle[s_Cutter_Action] = pos_Cutter_Action_Open;//
	servos_currentAngle[s_WasteCup_Action] = pos_WasteCup_Action_Fill;//
	
	//added Sep3rd 2024 for TR homing
	SetServosToStartPositions();


	//void SetProcessSteps_Load(){
	//store process steps
	ProcessSteps_LoadTool_servoNumber[0] = s_Tool_Height;
	ProcessSteps_LoadTool_servoNumber[1] = s_Tool_Rotate;
	ProcessSteps_LoadTool_servoNumber[2] = s_Tool_Height;
	ProcessSteps_LoadTool_servoNumber[3] = s_Tool_Rotate;
	ProcessSteps_LoadTool_servoNumber[4] = s_Tool_Rotate;
	ProcessSteps_LoadTool_servoNumber[5] = s_Tool_Height;
	ProcessSteps_LoadTool_servoNumber[6] = s_Tool_Lock;
	ProcessSteps_LoadTool_servoNumber[7] = s_Tool_Height;
	ProcessSteps_LoadTool_servoNumber[8] = s_Tool_Rotate;
	ProcessSteps_LoadTool_servoNumber[9] = s_Tool_Rotate;
	ProcessSteps_LoadTool_servoNumber[10] = s_QuickSwapHotend_Lock;
	ProcessSteps_LoadTool_servoNumber[11] = s_Tool_Height;
	ProcessSteps_LoadTool_servoNumber[12] = s_Tool_Lock;
	ProcessSteps_LoadTool_servoNumber[13] = s_Tool_Height;
	ProcessSteps_LoadTool_servoNumber[14] = s_QuickSwapHotend_Lock;
	ProcessSteps_LoadTool_servoNumber[15] = s_Tool_Height;
	ProcessSteps_LoadTool_servoNumber[16] = s_Tool_Rotate;
	ProcessSteps_LoadTool_servoNumber[17] = s_Tool_Rotate;
	ProcessSteps_LoadTool_servoNumber[18] = s_Tool_Height;
	ProcessSteps_LoadTool_servoNumber[19] = s_Tool_Rotate;
	ProcessSteps_LoadTool_servoNumber[20] = s_Tool_Rotate;

	ProcessSteps_LoadTool_degrees[0] = pos_Tool_Height_ButtingTheToolToTheLeftOfNext;
	ProcessSteps_LoadTool_degrees[1] = pos_Tool_Rotate_LeftOfToolInHolder;
	ProcessSteps_LoadTool_degrees[2] = pos_Tool_Height_NozzleCollarLevel;
	ProcessSteps_LoadTool_degrees[3] = pos_Tool_Rotate_UnderToolHolder_ConnectWithNozzleCollar;
	ProcessSteps_LoadTool_degrees[4] = pos_Tool_Rotate_UnderToolHolder_ConnectWithNozzleCollar_NoPressure;
	ProcessSteps_LoadTool_degrees[5] = pos_Tool_Height_ToolLoweredButStillInHolder;
	ProcessSteps_LoadTool_degrees[6] = pos_Tool_Lock_Locked; 
	ProcessSteps_LoadTool_degrees[7] = pos_Tool_Height_LowestLevel;
	ProcessSteps_LoadTool_degrees[8] = pos_Tool_Rotate_ButtonToolCheck;
	ProcessSteps_LoadTool_degrees[9] = pos_Tool_Rotate_UnderExtruder_ConnectWithNozzleCollar;
	ProcessSteps_LoadTool_degrees[10] = pos_QuickSwapHotend_Lock_Unlocked;
	ProcessSteps_LoadTool_degrees[11] = pos_Tool_Height_ToolLoweredButStillInExtruder;
	ProcessSteps_LoadTool_degrees[12] = pos_Tool_Lock_Unlocked;
	ProcessSteps_LoadTool_degrees[13] = pos_Tool_Height_ToolFullyInsertedIntoExtruder;
	ProcessSteps_LoadTool_degrees[14] = pos_QuickSwapHotend_Lock_Locked;
	ProcessSteps_LoadTool_degrees[15] = pos_Tool_Height_ToolFullyInsertedIntoExtruder_NoPressure;
	ProcessSteps_LoadTool_degrees[16] = pos_Tool_Rotate_UnderExtruder_JerkReleaseFromNozzleCollar;
	ProcessSteps_LoadTool_degrees[17] = pos_Tool_Rotate_UnderExtruder_ReleasedFromNozzleCollar;
	ProcessSteps_LoadTool_degrees[18] = pos_Tool_Height_LowestLevel;
	ProcessSteps_LoadTool_degrees[19] = pos_Tool_Rotate_ButtonToolCheck;
	ProcessSteps_LoadTool_degrees[20] = pos_Tool_Rotate_WaitingForunloadCommand;


	ProcessSteps_LoadTool_msDelayPerDegreeMoved[0] = 0;
	ProcessSteps_LoadTool_msDelayPerDegreeMoved[1] = 0;
	ProcessSteps_LoadTool_msDelayPerDegreeMoved[2] = 0;
	ProcessSteps_LoadTool_msDelayPerDegreeMoved[3] = 0;
	ProcessSteps_LoadTool_msDelayPerDegreeMoved[4] = 0;
	ProcessSteps_LoadTool_msDelayPerDegreeMoved[5] = 0;
	ProcessSteps_LoadTool_msDelayPerDegreeMoved[6] = 0;
	ProcessSteps_LoadTool_msDelayPerDegreeMoved[7] = 0;
	ProcessSteps_LoadTool_msDelayPerDegreeMoved[8] = 0;
	ProcessSteps_LoadTool_msDelayPerDegreeMoved[9] = 0;
	ProcessSteps_LoadTool_msDelayPerDegreeMoved[10] = 0;
	ProcessSteps_LoadTool_msDelayPerDegreeMoved[11] = 20; //60;//6;
	ProcessSteps_LoadTool_msDelayPerDegreeMoved[12] = 0;
	ProcessSteps_LoadTool_msDelayPerDegreeMoved[13] = 0;
	ProcessSteps_LoadTool_msDelayPerDegreeMoved[14] = 0;
	ProcessSteps_LoadTool_msDelayPerDegreeMoved[15] = 0;
	ProcessSteps_LoadTool_msDelayPerDegreeMoved[16] = 0;
	ProcessSteps_LoadTool_msDelayPerDegreeMoved[17] = 0;
	ProcessSteps_LoadTool_msDelayPerDegreeMoved[18] = 0;
	ProcessSteps_LoadTool_msDelayPerDegreeMoved[19] = 0;
	ProcessSteps_LoadTool_msDelayPerDegreeMoved[20] = 0;

	ProcessSteps_LoadTool_msDelayAfterCommandSent[0] = 75; //160;
	ProcessSteps_LoadTool_msDelayAfterCommandSent[1] = 50; //1145 //350 //150; //160;
	ProcessSteps_LoadTool_msDelayAfterCommandSent[2] = 50; //350 //150; //160;
	ProcessSteps_LoadTool_msDelayAfterCommandSent[3] = 50; //450 //150; //80;
	ProcessSteps_LoadTool_msDelayAfterCommandSent[4] = 20;
	ProcessSteps_LoadTool_msDelayAfterCommandSent[5] = 25;
	ProcessSteps_LoadTool_msDelayAfterCommandSent[6] = 50; //3; //130;
	ProcessSteps_LoadTool_msDelayAfterCommandSent[7] = 22;
	ProcessSteps_LoadTool_msDelayAfterCommandSent[8] = 30; //200; //90;
	ProcessSteps_LoadTool_msDelayAfterCommandSent[9] = 60; //260;//rotate to under extruder
	ProcessSteps_LoadTool_msDelayAfterCommandSent[10] = 40; //110;//give the hotend time to stabilize before moving up to heaterblock bore
	ProcessSteps_LoadTool_msDelayAfterCommandSent[11] = 40; //220;
	ProcessSteps_LoadTool_msDelayAfterCommandSent[12] = 23;//cannot unlock without hitting the cooling shround. Must have delay. //30; //130;
	ProcessSteps_LoadTool_msDelayAfterCommandSent[13] = 33; //130; //fully inserted into extruder
	ProcessSteps_LoadTool_msDelayAfterCommandSent[14] = 40; //200; //110; //Lock the hotend into extruder
	ProcessSteps_LoadTool_msDelayAfterCommandSent[15] = 19;
	ProcessSteps_LoadTool_msDelayAfterCommandSent[16] = 30; //110; //jerk nozzle release from hotend collar
	ProcessSteps_LoadTool_msDelayAfterCommandSent[17] = 19;
	ProcessSteps_LoadTool_msDelayAfterCommandSent[18] = 28;
	ProcessSteps_LoadTool_msDelayAfterCommandSent[19] = 110; //700;
	ProcessSteps_LoadTool_msDelayAfterCommandSent[20] = 26;

	ProcessSteps_LoadTool_stepType[0] = eeRegularStep;
	ProcessSteps_LoadTool_stepType[1] = eeRegularStep;
	ProcessSteps_LoadTool_stepType[2] = eeRegularStep;
	ProcessSteps_LoadTool_stepType[3] = eeToolHolderPrepUNrotate; //eeRegularStep; //rotate connect with nozzle collar
	ProcessSteps_LoadTool_stepType[4] = eeRegularStep;
	ProcessSteps_LoadTool_stepType[5] = eeRegularStep;
	ProcessSteps_LoadTool_stepType[6] = eeRegularStep; //eeAddHalfDegreePrecision; //eeRegularStep;
	ProcessSteps_LoadTool_stepType[7] = eeRegularStep;
	ProcessSteps_LoadTool_stepType[8] = eeButtonCheck_HoldingTool;
	ProcessSteps_LoadTool_stepType[9] = eeRegularStep; //eeAddHalfDegreePrecision; //eeRegularStep; //rotate centered under heater block bore
	ProcessSteps_LoadTool_stepType[10] = eeRegularStep;
	ProcessSteps_LoadTool_stepType[11] = eeRegularStep;
	ProcessSteps_LoadTool_stepType[12] = eeRegularStep;
	ProcessSteps_LoadTool_stepType[13] = eeRegularStep;
	ProcessSteps_LoadTool_stepType[14] = eeRegularStep;
	ProcessSteps_LoadTool_stepType[15] = eeRegularStep;
	ProcessSteps_LoadTool_stepType[16] = eeRegularStep;
	ProcessSteps_LoadTool_stepType[17] = eeRegularStep;
	ProcessSteps_LoadTool_stepType[18] = eeRegularStep;
	ProcessSteps_LoadTool_stepType[19] = eeButtonCheck_Empty;
	ProcessSteps_LoadTool_stepType[20] = eeRegularStep;


//							****UNLOAD****
	ProcessSteps_unload_connect_servoNumber[0] = s_Tool_Rotate;// TR 286 + 3 pos_Tool_Rotate_UnderExtruder_UnderTaperButPastNozzle
	ProcessSteps_unload_connect_servoNumber[1] = s_Tool_Height;// TH 43 + 11 pos_Tool_Height_SideOfNozzleInExtruder
	ProcessSteps_unload_connect_servoNumber[2] = s_Tool_Rotate;// TR 282 + 3 pos_Tool_Rotate_UnderExtruder_TouchSideOfNozzle
	ProcessSteps_unload_connect_servoNumber[3] = s_Tool_Height;// TH 39 + 11 pos_Tool_Height_CollarOfNozzleInExtruder
	ProcessSteps_unload_connect_servoNumber[4] = s_Tool_Rotate;
	ProcessSteps_unload_connect_servoNumber[5] = s_QuickSwapHotend_Lock;

	ProcessSteps_unload_connect_degrees[0] = pos_Tool_Rotate_UnderExtruder_UnderTaperButPastNozzle; //pos_Tool_Rotate_UnderExtruder_ReleasedFromNozzleCollar; //update to position past nozzle but under collar
	ProcessSteps_unload_connect_degrees[1] = pos_Tool_Height_SideOfNozzleInExtruder; //pos_Tool_Rotate_UnderExtruder_ReleasedFromNozzleCollar; //update to position past nozzle but under collar
	ProcessSteps_unload_connect_degrees[2] = pos_Tool_Rotate_UnderExtruder_TouchSideOfNozzle; //pos_Tool_Rotate_UnderExtruder_ReleasedFromNozzleCollar; //update to position past nozzle but under collar
	ProcessSteps_unload_connect_degrees[3] = pos_Tool_Height_CollarOfNozzleInExtruder; //pos_Tool_Rotate_UnderExtruder_ReleasedFromNozzleCollar; //update to position past nozzle but under collar
	ProcessSteps_unload_connect_degrees[4] = pos_Tool_Rotate_UnderExtruder_ConnectWithNozzleCollar;
	ProcessSteps_unload_connect_degrees[5] = pos_QuickSwapHotend_Lock_Unlocked;

	ProcessSteps_unload_connect_msDelayPerDegreeMoved[0] = 0;
	ProcessSteps_unload_connect_msDelayPerDegreeMoved[1] = 0;
	ProcessSteps_unload_connect_msDelayPerDegreeMoved[2] = 0;
	ProcessSteps_unload_connect_msDelayPerDegreeMoved[3] = 0;
	ProcessSteps_unload_connect_msDelayPerDegreeMoved[4] = 0;
	ProcessSteps_unload_connect_msDelayPerDegreeMoved[5] = 0;
	
	ProcessSteps_unload_connect_msDelayAfterCommandSent[0] = 360;
	ProcessSteps_unload_connect_msDelayAfterCommandSent[1] = 360; //10; //55; //350;
	ProcessSteps_unload_connect_msDelayAfterCommandSent[2] = 20; //10; //63; //430; //330; //230; 
	ProcessSteps_unload_connect_msDelayAfterCommandSent[3] = 40; //10; //up to nozzle collar level
	ProcessSteps_unload_connect_msDelayAfterCommandSent[4] = 20; //10; //50;
	ProcessSteps_unload_connect_msDelayAfterCommandSent[5] = 20; //11; //10; //11;
	
	ProcessSteps_unload_connect_stepType[0] = eeRegularStep;
	ProcessSteps_unload_connect_stepType[1] = eeRegularStep;
	ProcessSteps_unload_connect_stepType[2] = eeRegularStep;
	ProcessSteps_unload_connect_stepType[3] = eeRegularStep;
	ProcessSteps_unload_connect_stepType[4] = eeRegularStep;  
	ProcessSteps_unload_connect_stepType[5] = eeRegularStep;  


	//pulldown to locking height
	ProcessSteps_unload_pulldown_LockingHeight_servoNumber[0] = s_Tool_Height; //down to cutting height
	ProcessSteps_unload_pulldown_LockingHeight_servoNumber[1] = s_Tool_Lock; 
	
	ProcessSteps_unload_pulldown_LockingHeight_degrees[0] = pos_Tool_Height_ToolLoweredButStillInExtruder;
	ProcessSteps_unload_pulldown_LockingHeight_degrees[1] = pos_Tool_Lock_Locked;
	
	ProcessSteps_unload_pulldown_LockingHeight_msDelayPerDegreeMoved[0] = 0; //1; //lower to cutting height //This must be zero(0) so that the Zero delay can allow the lock to lock during the downward move. If it was set to a number >0 then the slow move would not allow the locl to activate until the move was complete regardless of the msDelayAfterCommandSent being zero. To sync the extrude and pulldown we must adjust the extrusion speed only.
	ProcessSteps_unload_pulldown_LockingHeight_msDelayPerDegreeMoved[1] = 0;
	
	ProcessSteps_unload_pulldown_LockingHeight_msDelayAfterCommandSent[0] = 10; //100;
	ProcessSteps_unload_pulldown_LockingHeight_msDelayAfterCommandSent[1] = 30; //110; //lock tool. lock moved to SetServoPosition()
	
	ProcessSteps_unload_pulldown_LockingHeight_stepType[0] = eeRegularStep; //was eeExtrude; but the extrude part is now handled by octoprint //lower to cutting height. extrude stage 2
	ProcessSteps_unload_pulldown_LockingHeight_stepType[1] = eeRegularStep; //Must be regular step so that the current angle of the tool lock servo is set. There may be a bug int he half degree type//eeAddHalfDegreePrecision; //eeRegularStep; //eeAddHalfDegreePrecision; //eeRegularStep; //locking the nozzle-hotend into the end effector

	//pulldown to cutting height
	ProcessSteps_unload_pulldown_CuttingHeight_servoNumber[0] = s_Tool_Height; //down to cutting height
	ProcessSteps_unload_pulldown_CuttingHeight_servoNumber[1] = s_QuickSwapHotend_Lock; //release the QuickSwap-lock after insert is removed from the bore
	
	ProcessSteps_unload_pulldown_CuttingHeight_degrees[0] = pos_Tool_Height_ToolLowered_CuttingHeight;
	ProcessSteps_unload_pulldown_CuttingHeight_degrees[1] = pos_QuickSwapHotend_Lock_Locked;
	
	ProcessSteps_unload_pulldown_CuttingHeight_msDelayPerDegreeMoved[0] = 0; //1; //lower to cutting height //This must be zero(0) so that the Zero delay can allow the lock to lock during the downward move. If it was set to a number >0 then the slow move would not allow the locl to activate until the move was complete regardless of the msDelayAfterCommandSent being zero. To sync the extrude and pulldown we must adjust the extrusion speed only.
	ProcessSteps_unload_pulldown_CuttingHeight_msDelayPerDegreeMoved[1] = 0;
	
	ProcessSteps_unload_pulldown_CuttingHeight_msDelayAfterCommandSent[0] = 10;
	ProcessSteps_unload_pulldown_CuttingHeight_msDelayAfterCommandSent[1] = 0;
	
	ProcessSteps_unload_pulldown_CuttingHeight_stepType[0] = eeRegularStep; //was eeExtrude; but the extrude part is now handled by octoprint //lower to cutting height. extrude stage 2
	ProcessSteps_unload_pulldown_CuttingHeight_stepType[1] = eeRegularStep;


	//void SetProcessSteps_unload_deployCutter(){
	ProcessSteps_unload_deployCutter_servoNumber[0] = s_Cutter_Rotate;
	// ProcessSteps_unload_deployCutter_degrees[0] = pos_Cutter_Rotate_Cutting; //sEP 2ND 2024: this value isn't used anymore, look in void unload_deployCutter, the angle is now set in octoprint code (should be a setting!)
	ProcessSteps_unload_deployCutter_msDelayPerDegreeMoved[0] = 6;//this makes the end position more repeatable than allowing the servo to control it's deceleration //6; //0; //cutter rotate
	ProcessSteps_unload_deployCutter_msDelayAfterCommandSent[0] = 65; //650; //550; //500; //190; //cutter rotate
	ProcessSteps_unload_deployCutter_stepType[0] = eeRegularStep; //eeAddHalfDegreePrecision; //eeRegularStep; //rotate to cutting position
	
	
	//void SetProcessSteps_unload_deployCutter_ConnectWithFilamentGuide
	ProcessSteps_unload_deployCutter_ConnectWithFilamentGuide_servoNumber[0] = s_Cutter_Rotate;
	ProcessSteps_unload_deployCutter_ConnectWithFilamentGuide_degrees[0] = pos_Cutter_Rotate_ConnectWithFilamentGuide;
	ProcessSteps_unload_deployCutter_ConnectWithFilamentGuide_msDelayPerDegreeMoved[0] = 16;//this makes the end position more repeatable than allowing the servo to control it's deceleration //6; //0; //cutter rotate
	ProcessSteps_unload_deployCutter_ConnectWithFilamentGuide_msDelayAfterCommandSent[0] = 65; //650; //550; //500; //190; //cutter rotate
	ProcessSteps_unload_deployCutter_ConnectWithFilamentGuide_stepType[0] = eeRegularStep; //eeAddHalfDegreePrecision; //eeRegularStep; //rotate to cutting position
	

	//void ProcessSteps_cutter_cut
	ProcessSteps_cutter_cut_servoNumber[0] = s_Cutter_Action;
	
	ProcessSteps_cutter_cut_degrees[0] = pos_Cutter_Action_Cut;
	
	ProcessSteps_cutter_cut_msDelayPerDegreeMoved[0] = 0;
	
	ProcessSteps_cutter_cut_msDelayAfterCommandSent[0] = 100;  //70;
	
	ProcessSteps_cutter_cut_stepType[0] = eeRegularStep;
	
	
	//void ProcessSteps_cutter_open
	ProcessSteps_cutter_open_servoNumber[0] = s_Cutter_Action;
	
	ProcessSteps_cutter_open_degrees[0] = pos_Cutter_Action_Open;
	
	ProcessSteps_cutter_open_msDelayPerDegreeMoved[0] = 0;
	
	ProcessSteps_cutter_open_msDelayAfterCommandSent[0] = 70;
	
	ProcessSteps_cutter_open_stepType[0] = eeRegularStep;
	

	//void SetProcessSteps_unload_avoidBin(){
	ProcessSteps_unload_avoidBin_servoNumber[0] = s_Tool_Height;
	ProcessSteps_unload_avoidBin_servoNumber[1] = s_Tool_Rotate;
	
	ProcessSteps_unload_avoidBin_degrees[0] = pos_Tool_Height_LowestLevel;
	ProcessSteps_unload_avoidBin_degrees[1] = pos_Tool_Rotate_PastWasteCup;
	
	ProcessSteps_unload_avoidBin_msDelayPerDegreeMoved[0] = 0;
	ProcessSteps_unload_avoidBin_msDelayPerDegreeMoved[1] = 0;
	
	ProcessSteps_unload_avoidBin_msDelayAfterCommandSent[0] = 0; //10
	ProcessSteps_unload_avoidBin_msDelayAfterCommandSent[1] = 0; //10
	
    ProcessSteps_unload_avoidBin_stepType[0] = eeRegularStep; //eeRetract;Not anymore that these steps are only for the palette //after cutters are open. retract
	ProcessSteps_unload_avoidBin_stepType[1] = eeRegularStep;
	
	


	//void SetProcessSteps_unload_stowCutter(){
	ProcessSteps_unload_stowCutter_servoNumber[0] = s_Cutter_Rotate;
	
	ProcessSteps_unload_stowCutter_degrees[0] = pos_Cutter_Rotate_Stowed;
	
	ProcessSteps_unload_stowCutter_msDelayPerDegreeMoved[0] = 0;//go full speed so that the tool can be stowed symultaneously //6 slow to keep the servo from dying //0; //cutter rotate
	
	ProcessSteps_unload_stowCutter_msDelayAfterCommandSent[0] = 0; //cutter rotate stowed //75; //50; //100; //200; //100; //50;//need slight delay just for the cutter to rotate a little away from the filament and break the strand //130;s_Cutter_Rotate no delay needed when stowing the cutter
	
	ProcessSteps_unload_stowCutter_stepType[0] = eeRegularStep; //was  eeRetract; but that is done by octoprint now //eeRegularStep;


	//void SetProcessSteps_unload_stowInsert(){
	ProcessSteps_unload_stowInsert_servoNumber[0] = s_Tool_Height; //lowest height
	ProcessSteps_unload_stowInsert_servoNumber[1] = s_Tool_Rotate; //to check button
	ProcessSteps_unload_stowInsert_servoNumber[2] = s_Tool_Rotate;
	ProcessSteps_unload_stowInsert_servoNumber[3] = s_Tool_Height;
	ProcessSteps_unload_stowInsert_servoNumber[4] = s_Tool_Lock;
	ProcessSteps_unload_stowInsert_servoNumber[5] = s_Tool_Height;
	ProcessSteps_unload_stowInsert_servoNumber[6] = s_Tool_Height;
	ProcessSteps_unload_stowInsert_servoNumber[7] = s_Tool_Rotate;
	ProcessSteps_unload_stowInsert_servoNumber[8] = s_Tool_Rotate;
	ProcessSteps_unload_stowInsert_servoNumber[9] = s_Tool_Height;
	ProcessSteps_unload_stowInsert_servoNumber[10] = s_Tool_Rotate;
	ProcessSteps_unload_stowInsert_servoNumber[11] = s_Tool_Rotate;
	
	ProcessSteps_unload_stowInsert_degrees[0] = pos_Tool_Height_LowestLevel;
	ProcessSteps_unload_stowInsert_degrees[1] = pos_Tool_Rotate_ButtonToolCheck; //should have tool
	ProcessSteps_unload_stowInsert_degrees[2] = pos_Tool_Rotate_UnderToolHolder_CenteredUnderCurrentTool; //ready to lift into position
	ProcessSteps_unload_stowInsert_degrees[3] = pos_Tool_Height_ToolLoweredButStillInHolder;
	ProcessSteps_unload_stowInsert_degrees[4] = pos_Tool_Lock_Unlocked; 
	ProcessSteps_unload_stowInsert_degrees[5] = pos_Tool_Height_ToolFullyInsertedInHolder;
	ProcessSteps_unload_stowInsert_degrees[6] = pos_Tool_Height_ToolFullyInsertedInHolder_NoPressure;
	ProcessSteps_unload_stowInsert_degrees[7] = pos_Tool_Rotate_ReleaseFromHotendUnderToolHolder;
	ProcessSteps_unload_stowInsert_degrees[8] = pos_Tool_Rotate_BetweenBothNozzles; //pos_Tool_Rotate_LeftOfToolInHolder; //pos_Tool_Rotate_BetweenBothNozzles; //pos_Tool_Rotate_LeftOfToolInHolder;
	ProcessSteps_unload_stowInsert_degrees[9] = pos_Tool_Height_LowestLevel;
	ProcessSteps_unload_stowInsert_degrees[10] = pos_Tool_Rotate_ButtonToolCheck;
	ProcessSteps_unload_stowInsert_degrees[11] = pos_Tool_Rotate_ButtingTheToolToTheLeftOfNext;
	
	ProcessSteps_unload_stowInsert_msDelayPerDegreeMoved[0] = 0;
	ProcessSteps_unload_stowInsert_msDelayPerDegreeMoved[1] = 0;
	ProcessSteps_unload_stowInsert_msDelayPerDegreeMoved[2] = 0;
	ProcessSteps_unload_stowInsert_msDelayPerDegreeMoved[3] = 0; //60;
	ProcessSteps_unload_stowInsert_msDelayPerDegreeMoved[4] = 0;
	ProcessSteps_unload_stowInsert_msDelayPerDegreeMoved[5] = 0;
	ProcessSteps_unload_stowInsert_msDelayPerDegreeMoved[6] = 0;
	ProcessSteps_unload_stowInsert_msDelayPerDegreeMoved[7] = 0;
	ProcessSteps_unload_stowInsert_msDelayPerDegreeMoved[8] = 0;
	ProcessSteps_unload_stowInsert_msDelayPerDegreeMoved[9] = 0;
	ProcessSteps_unload_stowInsert_msDelayPerDegreeMoved[10] = 0;
	ProcessSteps_unload_stowInsert_msDelayPerDegreeMoved[11] = 0;
	
	ProcessSteps_unload_stowInsert_msDelayAfterCommandSent[0] = 17;
	ProcessSteps_unload_stowInsert_msDelayAfterCommandSent[1] = 120; //700; //200; //button check should have tool
	ProcessSteps_unload_stowInsert_msDelayAfterCommandSent[2] = 28;
	ProcessSteps_unload_stowInsert_msDelayAfterCommandSent[3] = 38;
	ProcessSteps_unload_stowInsert_msDelayAfterCommandSent[4] = 20; //130;
	ProcessSteps_unload_stowInsert_msDelayAfterCommandSent[5] = 28;
	ProcessSteps_unload_stowInsert_msDelayAfterCommandSent[6] = 27;
	ProcessSteps_unload_stowInsert_msDelayAfterCommandSent[7] = 50; //400; //50; //release from hotend which is now stowed
	ProcessSteps_unload_stowInsert_msDelayAfterCommandSent[8] = 25;
	ProcessSteps_unload_stowInsert_msDelayAfterCommandSent[9] = 55; //150; //lower to lowest level
	ProcessSteps_unload_stowInsert_msDelayAfterCommandSent[10] = 60; //300; //180; button check should be empty
	ProcessSteps_unload_stowInsert_msDelayAfterCommandSent[11] = 30;
	
	ProcessSteps_unload_stowInsert_stepType[0] = eeRegularStep;
	ProcessSteps_unload_stowInsert_stepType[1] = eeButtonCheck_HoldingTool;
	ProcessSteps_unload_stowInsert_stepType[2] = eeRegularStep;
	ProcessSteps_unload_stowInsert_stepType[3] = eeRegularStep;
	ProcessSteps_unload_stowInsert_stepType[4] = eeRegularStep;
	ProcessSteps_unload_stowInsert_stepType[5] = eeRegularStep;
	ProcessSteps_unload_stowInsert_stepType[6] = eeRegularStep;
	ProcessSteps_unload_stowInsert_stepType[7] = eeToolHolderPrepRotate;
	ProcessSteps_unload_stowInsert_stepType[8] = eeRegularStep;
	ProcessSteps_unload_stowInsert_stepType[9] = eeRegularStep;
	ProcessSteps_unload_stowInsert_stepType[10] = eeButtonCheck_Empty;
	ProcessSteps_unload_stowInsert_stepType[11] = eeRegularStep;


	//void SetProcessSteps_unload_dumpWaste(){
	ProcessSteps_unload_dumpWaste_servoNumber[0] = s_WasteCup_Action;
	ProcessSteps_unload_dumpWaste_servoNumber[1] = s_WasteCup_Action; 

	ProcessSteps_unload_dumpWaste_degrees[0] = pos_WasteCup_Action_Dump;
	ProcessSteps_unload_dumpWaste_degrees[1] = pos_WasteCup_Action_Fill;
	
	
	ProcessSteps_unload_dumpWaste_msDelayPerDegreeMoved[0] = 0;
	ProcessSteps_unload_dumpWaste_msDelayPerDegreeMoved[1] = 0;

	ProcessSteps_unload_dumpWaste_msDelayAfterCommandSent[0] = 28;
	ProcessSteps_unload_dumpWaste_msDelayAfterCommandSent[1] = 20;

	ProcessSteps_unload_dumpWaste_stepType[0] = eeRegularStep;
	ProcessSteps_unload_dumpWaste_stepType[1] = eeRegularStep;
}



void setup() {
  Serial.begin(9600);

  lcd.begin(16, 2);

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(300);  // Digital servos run at 300Hz updates
  
  // Initialize the pin for the end effector insert full/empty checks
  pinMode(CheckButton_Pin, INPUT_PULLUP);
  EmptyButtonState = digitalRead(CheckButton_Pin);
  delay(10);

  //adjusted on Sep 3rd 2024 to accomodate the TR homing routine which requires the EmptyButtonState be determined before here.
  SetSwapStepLocations();
  
  SetServosToStartPositions(); //added Sep 3rd 2024 for TR homing
  
  
  //align to the first tool
  ToolHolder_AlignToThisTool(0); //this must go here not in homing TR or in SetServosToStartPositions because homing calls it and then resets it to no1 instead of whichever unload it's on.
  
  //CheckToolRotateHomePosition(); //don't put this here. It messes with the octoprint connection on startup. Instead, once octoprint connects we'll call the TR home directly from octoprint.
  
  updateLCD(msg_READY_TO_SWAP, msg_INSERT_EMPTY);
}




// int checkIntParity(int number) {
  // int count = 0;
  // while (number) {
    // count++;
    // number = number & (number - 1);
  // }
  // return ~count & 1; // returns 1 for odd parity, 0 for even parity
// }


bool CheckButton_Pressed()
{
	delay(10);
	
	if(digitalRead(CheckButton_Pin) == EmptyButtonState)
	{
		return false;
	}
	else
	{
		return true;
	}
}


void SetServoPosition(int ServoNum, int TargetAngle, int msDelay)
{
	int pulselength = 0;
	int currentAngle = servos_currentAngle[ServoNum];
	int angleDifference = TargetAngle - currentAngle;

	int msCountedBeforeLock = 0;

  //if the msDelay is zero then don't use a loop
  if(msDelay == 0)
  {
      servos_currentAngle[ServoNum] = TargetAngle;  
      pulselength = map(servos_currentAngle[ServoNum], 0, servos_maxAngle[ServoNum], servo_pwm_min, servo_pwm_max);
      pwm.setPWM(servos_pin[ServoNum], 0, pulselength); 
  }
  //else use a loop to inject the delay and fake accel
  else
  {
  	if (angleDifference > 0)
  	{  	
  		for(int i = currentAngle; i <= TargetAngle; i++)
  		{
  			servos_currentAngle[ServoNum] = i;  
  			pulselength = map(servos_currentAngle[ServoNum], 0, servos_maxAngle[ServoNum], servo_pwm_min, servo_pwm_max);
  			pwm.setPWM(servos_pin[ServoNum], 0, pulselength);	
  			delay(msDelay);
			
			
			//deploy the tool lock part way thru the extrude
			if (LockToolPartWayThru)
			{
				msCountedBeforeLock += msDelay;

				//lock the tool
				if(msCountedBeforeLock >= numMsUntilLock)
				{
					LockToolPartWayThru = false;
					servos_currentAngle[s_Tool_Lock] = pos_Tool_Lock_Locked;
					pulselength = map(servos_currentAngle[s_Tool_Lock], 0, servos_maxAngle[ServoNum], servo_pwm_min, servo_pwm_max);
					pwm.setPWM(servos_pin[s_Tool_Lock], 0, pulselength);	
				}
			}
  		}	
  	}
  	else
  	{
  		for(int i = currentAngle; i >= TargetAngle; i--)
  		{
  			servos_currentAngle[ServoNum] = i;  
  			pulselength = map(servos_currentAngle[ServoNum], 0, servos_maxAngle[ServoNum], servo_pwm_min, servo_pwm_max);
  			pwm.setPWM(servos_pin[ServoNum], 0, pulselength);	
  			delay(msDelay);
  		}	
  	}
  }
}

void ProcessStep(int currentServo
				, int targetAngle
				, int msDelayPerDegreeMoved 
				, int msDelayAfterCommandSent
				, int stepType) {
	
  double ram_msPerStepAdjustment = 1 + pgm_read_float(&msPerStepAdjustment);
  double Adjusted_msDelayAfterCommandSent = msDelayAfterCommandSent * ram_msPerStepAdjustment;
  byte slowDownDegrees = 20;
  int pulselength = 0;
  byte slowSpeed = 80;
	  
	// Serial.print("ram_msPerStepAdjustment:");
	// Serial.println(ram_msPerStepAdjustment);	
	// Serial.print("msDelayAfterCommandSent Orig:");
	// Serial.println(msDelayAfterCommandSent);
	// Serial.print("msDelayAfterCommandSent Adjusted:");
	// Serial.println(Adjusted_msDelayAfterCommandSent);


  switch(stepType) {
    //check to make sure the end effector is clear of inserts
    case eeButtonCheck_Empty:	
	  //split the button press into 2 parts
	  //fast move and slow move 
	  //to avoid knocking the wires off the button
		
	  //fast move for 90%
      SetServoPosition(currentServo, targetAngle + slowDownDegrees, msDelayPerDegreeMoved);
	  //slow move for 10%
      SetServoPosition(currentServo, targetAngle, slowSpeed);
      delay(msDelayAfterCommandSent);
	  
      if(CheckButton_Pressed() && ErrorCheckingEnabed) {
        printWithParity_P(msg_ERROR_HAS_TOOL_BUT_SHOULD_BE_EMPTY);
        updateLCD(msg_ERROR_NOT_EMPTY, msg_S_TO_RETRY);
        InErrorState = true;
      } else {      
        if(CheckButton_Pressed()) {
          printWithParity_P(msg_BUTTON_PRESSED);
        } else {
          printWithParity_P(msg_BUTTON_NOT_PRESSED);
        }  
        InErrorState = false;
      }
      break;
      
    //check to make sure the end effector is holding an insert
    case eeButtonCheck_HoldingTool:	
	  //split the button press into 2 parts
	  //fast move and slow move 
	  //to avoid knocking the wires off the button
		
	  //fast move for 90%
      SetServoPosition(currentServo, targetAngle + slowDownDegrees, msDelayPerDegreeMoved);
	  //slow move for 10%
      SetServoPosition(currentServo, targetAngle, slowSpeed);
      delay(msDelayAfterCommandSent);

      if(!CheckButton_Pressed() && ErrorCheckingEnabed) {
        printWithParity_P(msg_ERROR_IS_EMPTY_BUT_SHOULD_HAVE_TOOL);
        updateLCD(msg_ERROR_EMPTY, msg_S_TO_RETRY);
        InErrorState = true;
      } else {      
        if(CheckButton_Pressed()) {
          printWithParity_P(msg_BUTTON_PRESSED);
        } else {
          printWithParity_P(msg_BUTTON_NOT_PRESSED);
        }  
        InErrorState = false;
      }
      break;

    case eeToolHolderPrepRotate:
      servos_currentAngle[s_ToolHolder_Rotate] = servos_currentAngle[s_ToolHolder_Rotate] + eeToolHolderPrepRotate_degrees;
      pulselength = map(servos_currentAngle[s_ToolHolder_Rotate], servoMinAngle, servos_maxAngle[s_ToolHolder_Rotate], servo_pwm_min, servo_pwm_max);
      pwm.setPWM(servos_pin[s_ToolHolder_Rotate], 0, pulselength); 

      SetServoPosition(currentServo, targetAngle, msDelayPerDegreeMoved);

      delay(msDelayAfterCommandSent);
      break;
      
    case eeToolHolderPrepUNrotate:
      SetServoPosition(currentServo, targetAngle, msDelayPerDegreeMoved); //rotate tool actuator
      delay(70);
      pulselength = map(servos_currentAngle[s_ToolHolder_Rotate] - eeToolHolderPrepUNrotate_degrees, servoMinAngle, servos_maxAngle[s_ToolHolder_Rotate], servo_pwm_min, servo_pwm_max);
      pwm.setPWM(servos_pin[s_ToolHolder_Rotate], 0, pulselength); 
      delay(msDelayAfterCommandSent);
      pulselength = map(servos_currentAngle[s_ToolHolder_Rotate], servoMinAngle, servos_maxAngle[s_ToolHolder_Rotate], servo_pwm_min, servo_pwm_max);
      pwm.setPWM(servos_pin[s_ToolHolder_Rotate], 0, pulselength); 
      break;
      
    case eeRegularStep:
      SetServoPosition(currentServo, targetAngle, msDelayPerDegreeMoved);
      delay(msDelayAfterCommandSent);
      break;
      
    // case eeAddHalfDegreePrecision:
      // int precisionPulseLength = 0;
      // precisionPulseLength = fMap((float)targetAngle + (float)0.5, 0, servos_maxAngle[currentServo], servo_pwm_min, servo_pwm_max);
      // pwm.setPWM(servos_pin[currentServo], 0, precisionPulseLength);
      // delay(msDelayAfterCommandSent);
      // break;
  }
}

//pass in the array containing all the steps, and the array of the degrees for each step
//this will loop thru each step and if there is an error 
//it will wait for the user to correct and press the S button on the lcd
void ExecuteSteps(byte ProcessSteps_servoNumber[]
				,int ProcessSteps_degrees[]
				,byte ProcessSteps_msDelayPerDegreeMoved[]
				,byte ProcessSteps_msDelayAfterCommandSent[]
				,byte ProcessSteps_stepType[]
				,int NumSteps
				,int inputDelay=0) { // add inputDelay parameter with a default value of 0
    
		
  int pulselength = 0;
  int buttonPress;
  int currentStepOfProcess = 0;
  int delayPerDegree;
  
	//Serial.print("# Steps:");
	//Serial.println(NumSteps);
	
	
  while (currentStepOfProcess < NumSteps)
  {
	
    //updateLCD_line2(ProcessSteps_servoNumber[currentStepOfProcess]
	//				, ProcessSteps_degrees[currentStepOfProcess]);
		

	updateLCD_line2_with_step(ProcessSteps_servoNumber[currentStepOfProcess]
							, ProcessSteps_degrees[currentStepOfProcess]
							, currentStepOfProcess);	

							

    char* message = generateMessage(currentStepOfProcess 
                                    ,ProcessSteps_servoNumber[currentStepOfProcess]
					                          , ProcessSteps_degrees[currentStepOfProcess]);
  //printWithParity(message);

	if ( inputDelay == 0 )
		delayPerDegree = ProcessSteps_msDelayPerDegreeMoved[currentStepOfProcess];
	else
		delayPerDegree = inputDelay;
	
    ProcessStep(ProcessSteps_servoNumber[currentStepOfProcess]
				,ProcessSteps_degrees[currentStepOfProcess] 
				,delayPerDegree
				,ProcessSteps_msDelayAfterCommandSent[currentStepOfProcess]*10 + msDelayAfterCommandSent_Buffer //it's *10 because then we can store the delays as bytes
				,ProcessSteps_stepType[currentStepOfProcess]
				);
  
	//if the process step checked the button
	//and there was an error
	//pause for user intervention
    if(!InErrorState)
    {
      currentStepOfProcess++;
    }
    else
    {
      SetServoPosition(s_Tool_Rotate, 95, 0); //move to waiting/start position
      delay(400);

      //unlock end tool:
      pulselength = map(180, 0, servos_maxAngle[s_Tool_Lock], servo_pwm_min, servo_pwm_max);
      pwm.setPWM(servos_pin[s_Tool_Lock], 0, pulselength);
      delay(400);
      
      do 
      {
        buttonPress = analogRead(0);
      } while (buttonPress < 600 || buttonPress > 700); //if S button press then retry
  
      //set the tool lock to its current position 
      //should be locked if it SHOULD be full
      //should be Unlocked if it SHOULD be empty
      pulselength = map(servos_currentAngle[s_Tool_Lock], 0, servos_maxAngle[s_Tool_Lock], servo_pwm_min, servo_pwm_max);
      pwm.setPWM(servos_pin[s_Tool_Lock], 0, pulselength);
      delay(400);
  
      printWithParity_P(msg_ERROR_WAS_RESET);
      updateLCD_line1(msg_LOADING);
      InErrorState = false;
    }
  }
  
  currentStepOfProcess = 0;
}



void load_insert(int toolToLoad) {    
    toolIsLoaded = true;

    ToolHolder_AlignToThisTool(toolToLoad);

	ExecuteSteps(ProcessSteps_LoadTool_servoNumber
				,ProcessSteps_LoadTool_degrees
				,ProcessSteps_LoadTool_msDelayPerDegreeMoved
				,ProcessSteps_LoadTool_msDelayAfterCommandSent
				,ProcessSteps_LoadTool_stepType
				,numOfProcessSteps_LoadTool);
}

void unload_connect(){  
	ExecuteSteps(ProcessSteps_unload_connect_servoNumber
				,ProcessSteps_unload_connect_degrees
				,ProcessSteps_unload_connect_msDelayPerDegreeMoved
				,ProcessSteps_unload_connect_msDelayAfterCommandSent
				,ProcessSteps_unload_connect_stepType
				,numOfProcessSteps_unload_connect);
}

// Updated the unload_pulldown_CuttingHeight function to optionally accept an 'inputDelay' parameter.
// If a non-zero value is provided as 'inputDelay' when calling the function, 
// this 'inputDelay' value will override the default 'ProcessSteps_unload_pulldown_CuttingHeight_msDelayPerDegreeMoved' value as the delay per degree.
// If the 'inputDelay' is not provided or zero, the function will use 'ProcessSteps_unload_pulldown_CuttingHeight_msDelayPerDegreeMoved' as the delay per degree.
void unload_pulldown_LockingHeight(int inputDelay = 0){
	ExecuteSteps(ProcessSteps_unload_pulldown_LockingHeight_servoNumber
				,ProcessSteps_unload_pulldown_LockingHeight_degrees
				,ProcessSteps_unload_pulldown_LockingHeight_msDelayPerDegreeMoved
				,ProcessSteps_unload_pulldown_LockingHeight_msDelayAfterCommandSent
				,ProcessSteps_unload_pulldown_LockingHeight_stepType
				,numOfProcessSteps_unload_pulldown_LockingHeight
				,inputDelay);
}

void unload_pulldown_CuttingHeight(int inputDelay = 0){
	ExecuteSteps(ProcessSteps_unload_pulldown_CuttingHeight_servoNumber
				,ProcessSteps_unload_pulldown_CuttingHeight_degrees
				,ProcessSteps_unload_pulldown_CuttingHeight_msDelayPerDegreeMoved
				,ProcessSteps_unload_pulldown_CuttingHeight_msDelayAfterCommandSent
				,ProcessSteps_unload_pulldown_CuttingHeight_stepType
				,numOfProcessSteps_unload_pulldown_CuttingHeight
				,inputDelay);
}

void unload_deployCutter(int DeployAngle){
	ProcessSteps_unload_deployCutter_degrees[0] = DeployAngle;
	
	ExecuteSteps(ProcessSteps_unload_deployCutter_servoNumber
				,ProcessSteps_unload_deployCutter_degrees
				,ProcessSteps_unload_deployCutter_msDelayPerDegreeMoved
				,ProcessSteps_unload_deployCutter_msDelayAfterCommandSent
				,ProcessSteps_unload_deployCutter_stepType
				,numOfProcessSteps_unload_deployCutter);
}

void unload_deployCutter_ConnectWithFilamentGuide(){
	ExecuteSteps(ProcessSteps_unload_deployCutter_ConnectWithFilamentGuide_servoNumber
				,ProcessSteps_unload_deployCutter_ConnectWithFilamentGuide_degrees
				,ProcessSteps_unload_deployCutter_ConnectWithFilamentGuide_msDelayPerDegreeMoved
				,ProcessSteps_unload_deployCutter_ConnectWithFilamentGuide_msDelayAfterCommandSent
				,ProcessSteps_unload_deployCutter_ConnectWithFilamentGuide_stepType
				,numOfProcessSteps_unload_deployCutter_ConnectWithFilamentGuide);
}

void cutter_cut(){
	ExecuteSteps(ProcessSteps_cutter_cut_servoNumber
				,ProcessSteps_cutter_cut_degrees
				,ProcessSteps_cutter_cut_msDelayPerDegreeMoved
				,ProcessSteps_cutter_cut_msDelayAfterCommandSent
				,ProcessSteps_cutter_cut_stepType
				,numOfProcessSteps_cutter_cut);
}

void cutter_open(){
	ExecuteSteps(ProcessSteps_cutter_open_servoNumber
				,ProcessSteps_cutter_open_degrees
				,ProcessSteps_cutter_open_msDelayPerDegreeMoved
				,ProcessSteps_cutter_open_msDelayAfterCommandSent
				,ProcessSteps_cutter_open_stepType
				,numOfProcessSteps_cutter_open);
}

//design changes to the waste cup to servo connecting strap made this unnecessary
//palette only
void unload_avoidBin(){
	ExecuteSteps(ProcessSteps_unload_avoidBin_servoNumber
				,ProcessSteps_unload_avoidBin_degrees
				,ProcessSteps_unload_avoidBin_msDelayPerDegreeMoved
				,ProcessSteps_unload_avoidBin_msDelayAfterCommandSent
				,ProcessSteps_unload_avoidBin_stepType
				,numOfProcessSteps_unload_avoidBin);
}

void unload_stowCutter(){
	ExecuteSteps(ProcessSteps_unload_stowCutter_servoNumber
				,ProcessSteps_unload_stowCutter_degrees
				,ProcessSteps_unload_stowCutter_msDelayPerDegreeMoved
				,ProcessSteps_unload_stowCutter_msDelayAfterCommandSent
				,ProcessSteps_unload_stowCutter_stepType
				,numOfProcessSteps_unload_stowCutter);
	
}

void unload_stowInsert(){
	ExecuteSteps(ProcessSteps_unload_stowInsert_servoNumber
				,ProcessSteps_unload_stowInsert_degrees
				,ProcessSteps_unload_stowInsert_msDelayPerDegreeMoved
				,ProcessSteps_unload_stowInsert_msDelayAfterCommandSent
				,ProcessSteps_unload_stowInsert_stepType
				,numOfProcessSteps_unload_stowInsert);

	toolIsLoaded = false;
}

//palette only
void unload_dumpWaste(){
	ExecuteSteps(ProcessSteps_unload_dumpWaste_servoNumber
				,ProcessSteps_unload_dumpWaste_degrees
				,ProcessSteps_unload_dumpWaste_msDelayPerDegreeMoved
				,ProcessSteps_unload_dumpWaste_msDelayAfterCommandSent
				,ProcessSteps_unload_dumpWaste_stepType
				,numOfProcessSteps_unload_dumpWaste);
}

//move the wipe pad to a random position under the nozzle
void wiper_deploy(){
	int pulselength = map(random(110, 120), 0, servos_maxAngle[s_Cutter_Rotate], servo_pwm_min, servo_pwm_max); //was random(115, 125)
	pwm.setPWM(servos_pin[s_Cutter_Rotate], 0, pulselength);
	delay(100); //200
}

//Stow the wiper with random delay
void wiper_stow(){
	int pulselength = map(pos_Cutter_Rotate_Stowed, 0, servos_maxAngle[s_Cutter_Rotate], servo_pwm_min, servo_pwm_max);
	pwm.setPWM(servos_pin[s_Cutter_Rotate], 0, pulselength);
}


// function that continuously reads incoming serial data, 
//appending each character to a string until it encounters a newline character, 
//which signals the end of a message. 
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
	  
// Serial.print("Input string:");
// Serial.println(inputString);
	  
    } else if (inputStringIndex < inputStringSize - 1) {
      inputString[inputStringIndex] = inChar;
      inputStringIndex++;
    }
  }
}





//pos_Tool_Rotate_UnderExtruder_ConnectWithNozzleCollar //in position under extruder centered with bore
//Bore alignment turned on. Tool Rotate Servo moved to position Under Extruder Connect With Nozzle Collar.
//Bore alignment turned off. Tool Rotate Servo moved to position Butting The Tool To The Left Of Next.
void boreAlignOn() {
  //lock the insert into the end effector
	int pulselength = 0;
    servos_currentAngle[s_Tool_Lock] = pos_Tool_Lock_Locked;
    pulselength = map(servos_currentAngle[s_Tool_Lock], 0, servos_maxAngle[s_Tool_Lock], servo_pwm_min, servo_pwm_max);
    pwm.setPWM(servos_pin[s_Tool_Lock], 0, pulselength);

	delay(200); //wait for lock to be in place

    // Set the Tool_Rotate servo to the desired position
    servos_currentAngle[s_Tool_Rotate] = pos_Tool_Rotate_UnderExtruder_ConnectWithNozzleCollar;
    pulselength = map(servos_currentAngle[s_Tool_Rotate], 0, servos_maxAngle[s_Tool_Rotate], servo_pwm_min, servo_pwm_max);
    pwm.setPWM(servos_pin[s_Tool_Rotate], 0, pulselength);
	
	delay(500); //wait for lock to be in place
	
	//unlock the QuickSwap-Hotend
    servos_currentAngle[s_QuickSwapHotend_Lock] = pos_QuickSwapHotend_Lock_Unlocked;
    pulselength = map(servos_currentAngle[s_QuickSwapHotend_Lock], 0, servos_maxAngle[s_QuickSwapHotend_Lock], servo_pwm_min, servo_pwm_max);
    pwm.setPWM(servos_pin[s_QuickSwapHotend_Lock], 0, pulselength);	
	
	//wait for the end effector to be still
    //then unlock the tool
    delay(2000);
    servos_currentAngle[s_Tool_Lock] = pos_Tool_Lock_Unlocked;
    pulselength = map(servos_currentAngle[s_Tool_Lock], 0, servos_maxAngle[s_Tool_Lock], servo_pwm_min, servo_pwm_max);
    pwm.setPWM(servos_pin[s_Tool_Lock], 0, pulselength);
}

//pos_Tool_Rotate_ButtingTheToolToTheLeftOfNext //tool arm stored
void boreAlignOff() {
	//lock the insert into the end effector
	int pulselength = 0;
    servos_currentAngle[s_Tool_Lock] = pos_Tool_Lock_Locked;
    pulselength = map(servos_currentAngle[s_Tool_Lock], 0, servos_maxAngle[s_Tool_Lock], servo_pwm_min, servo_pwm_max);
    pwm.setPWM(servos_pin[s_Tool_Lock], 0, pulselength);
	delay(200); //wait for lock to be in place
	
    // Set the Tool_Rotate servo to the desired position
    servos_currentAngle[s_Tool_Rotate] = pos_Tool_Rotate_ButtingTheToolToTheLeftOfNext;
    pulselength = map(servos_currentAngle[s_Tool_Rotate], 0, servos_maxAngle[s_Tool_Rotate], servo_pwm_min, servo_pwm_max);
    pwm.setPWM(servos_pin[s_Tool_Rotate], 0, pulselength);
    //wait for the end effector to be still
    delay(2000);

    //then unlock the tool
    servos_currentAngle[s_Tool_Lock] = pos_Tool_Lock_Unlocked;
    pulselength = map(servos_currentAngle[s_Tool_Lock], 0, servos_maxAngle[s_Tool_Lock], servo_pwm_min, servo_pwm_max);
    pwm.setPWM(servos_pin[s_Tool_Lock], 0, pulselength);
	
	delay(500); //wait for lock to be in place
	
	//unlock the QuickSwap-Hotend
    servos_currentAngle[s_QuickSwapHotend_Lock] = pos_QuickSwapHotend_Lock_Locked;
    pulselength = map(servos_currentAngle[s_QuickSwapHotend_Lock], 0, servos_maxAngle[s_QuickSwapHotend_Lock], servo_pwm_min, servo_pwm_max);
    pwm.setPWM(servos_pin[s_QuickSwapHotend_Lock], 0, pulselength);	

}



void loop() {
  if (stringComplete) {
    int inputStringLength = strlen(inputString);
    int inputParity = inputString[inputStringLength - 1] - '0';
    inputString[inputStringLength - 1] = '\0'; // Remove the parity character
    char inputMessage_TextPart[inputStringSize];
    int inputMessage_NumberPart = 0;

    // Find the index where the number starts
    int numIndex = -1;
    for (int i = 0; i < inputStringLength; i++) {
      if (isdigit(inputString[i])) {
        numIndex = i;
        break;
      }
    }

    // Extract the text part and number part if a number is found
    if (numIndex != -1) {
      strncpy(inputMessage_TextPart, inputString, numIndex);
      inputMessage_TextPart[numIndex] = '\0'; // Ensure null termination
      inputMessage_NumberPart = atoi(inputString + numIndex);
    } else {
      strncpy(inputMessage_TextPart, inputString, inputStringSize);
    }

    if (checkParity(inputString) == inputParity) {
      if (strcmp_P(inputMessage_TextPart, msg_OCTOPRINT) == 0) {
        printWithParity_P(msg_SWAPPER);
      } else if (strcmp_P(inputMessage_TextPart, msg_LOAD_INSERT) == 0) {
        insertNumber = inputMessage_NumberPart;
        load_insert(insertNumber);
		printWithParity_Combined(inputString, msg_OK);
      } else if (strcmp_P(inputMessage_TextPart, msg_unload_CONNECT) == 0) {
        updateLCD_line1(msg_CONNECT);
        unload_connect();
		printWithParity_Combined(inputString, msg_OK);
      } else if (strcmp_P(inputMessage_TextPart, msg_unload_PULLDOWN_LockingHeight) == 0) {
        updateLCD_line1(msg_PULLDOWN);
        unload_pulldown_LockingHeight(inputMessage_NumberPart);
		printWithParity_Combined(inputString, msg_OK);
      } else if (strcmp_P(inputMessage_TextPart, msg_unload_PULLDOWN_CuttingHeight) == 0) {
        updateLCD_line1(msg_PULLDOWN);
        unload_pulldown_CuttingHeight(inputMessage_NumberPart);
		printWithParity_Combined(inputString, msg_OK);
      } else if (strcmp_P(inputMessage_TextPart, msg_unload_DEPLOYCUTTER) == 0) {
        updateLCD_line1(msg_DEPLOY_CUTTER);
        unload_deployCutter(inputMessage_NumberPart);
		printWithParity_Combined(inputString, msg_OK);
      } else if (strcmp_P(inputMessage_TextPart, msg_unload_DEPLOYCUTTER_CONNECT_WITH_FILAMENT_GUIDE) == 0) {
        updateLCD_line1(msg_DEPLOY_CUTTER);
        unload_deployCutter_ConnectWithFilamentGuide();
		printWithParity_Combined(inputString, msg_OK);
      } else if (strcmp_P(inputMessage_TextPart, msg_cutter_CUT) == 0) {
        updateLCD_line1(msg_LCD_cutter_cut);
        cutter_cut();
		printWithParity_Combined(inputString, msg_OK);
      } else if (strcmp_P(inputMessage_TextPart, msg_cutter_OPEN) == 0) {
        updateLCD_line1(msg_LCD_cutter_open);
        cutter_open();
		printWithParity_Combined(inputString, msg_OK);
      } else if (strcmp_P(inputMessage_TextPart, msg_unload_AVOIDBIN) == 0) {
        updateLCD_line1(msg_AVOID_WASTE_BIN);
        unload_avoidBin();
		printWithParity_Combined(inputString, msg_OK);
      } else if (strcmp_P(inputMessage_TextPart, msg_unload_stowCutter) == 0) {
        updateLCD_line1(msg_STOW_CUTTER);
        unload_stowCutter();
		printWithParity_Combined(inputString, msg_OK);
      } else if (strcmp_P(inputMessage_TextPart, msg_unload_stowInsert) == 0) {
        updateLCD_line1(msg_STOW_INSERT);
        unload_stowInsert();
		printWithParity_Combined(inputString, msg_OK);
      } else if (strcmp_P(inputMessage_TextPart, msg_unload_dumpWaste) == 0) {
        updateLCD_line1(msg_DUMP_WASTE);
        unload_dumpWaste();
		printWithParity_Combined(inputString, msg_OK);
      } else if (strcmp_P(inputMessage_TextPart, msg_unloadED_MESSAGE) == 0) {
        insertNumber = 0;
        updateLCD(msg_READY_TO_SWAP, msg_INSERT_EMPTY);
		printWithParity_Combined(inputString, msg_OK);
      } else if (strcmp_P(inputMessage_TextPart, msg_SWAP_MESSAGE) == 0) {
        updateLCD_line1(insertNumber == 0 ? msg_SWAPPING_FORMAT_1 : msg_SWAPPING_FORMAT_2, insertNumber, inputMessage_NumberPart);
		printWithParity_Combined(inputString, msg_OK);
      } else if (strcmp_P(inputMessage_TextPart, msg_WIPER_DEPLOY) == 0) {
        updateLCD_line1(msg_DEPLOY_WIPER);
        wiper_deploy();
		printWithParity_Combined(inputString, msg_OK);
        updateLCD_line1(msg_WIPER_DEPLOYED);
      } else if (strcmp_P(inputMessage_TextPart, msg_WIPER_STOW) == 0) {
        // updateLCD_line1(msg_STOW_WIPER); //commented out on Aug 23rd 2024 because LCD messages should be controlled in Octoprint
        wiper_stow();
		printWithParity_Combined(inputString, msg_OK);
      }  else if (strcmp_P(inputMessage_TextPart, msg_INSERT_NUMBER) == 0) { //updated aug 23rd 2024
		printWithParity_Combined(inputString, msg_OK);
        delay(3000);
        updateLCD(msg_READY_TO_SWAP, msg_INSERT_FORMAT, inputMessage_NumberPart);
      } else if (strcmp_P(inputMessage_TextPart, msg_BoreAlignOn) == 0) {
        boreAlignOn();
		printWithParity_Combined(inputString, msg_OK);
      } else if (strcmp_P(inputMessage_TextPart, msg_BoreAlignOff) == 0) {
        boreAlignOff();
		printWithParity_Combined(inputString, msg_OK);
      } else if (strcmp_P(inputMessage_TextPart, msg_WriteToEeprom_MajorVersion) == 0) {
        EEPROM.write(30, inputMessage_NumberPart);
		printWithParity_Combined(inputString, msg_OK);
      } else if (strcmp_P(inputMessage_TextPart, msg_WriteToEeprom_MinorVersion) == 0) {
        EEPROM.write(31, inputMessage_NumberPart);
		printWithParity_Combined(inputString, msg_OK);
      } else if (strcmp_P(inputMessage_TextPart, msg_WriteToEeprom_PatchVersion) == 0) {
        EEPROM.write(32, inputMessage_NumberPart);
		printWithParity_Combined(inputString, msg_OK);
      } else if (strcmp_P(inputMessage_TextPart, msg_ReadFromEeprom_MajorVersion) == 0) {
        int majorVersion = EEPROM.read(30);
          PrintIntWithParityAsChar(majorVersion);
      } else if (strcmp_P(inputMessage_TextPart, msg_ReadFromEeprom_MinorVersion) == 0) {
          int minorVersion = EEPROM.read(31);
          PrintIntWithParityAsChar(minorVersion);
      } else if (strcmp_P(inputMessage_TextPart, msg_ReadFromEeprom_PatchVersion) == 0) {
          int patchVersion = EEPROM.read(32);
          PrintIntWithParityAsChar(patchVersion);
	  } else if (strcmp_P(inputMessage_TextPart, msg_RetrieveCurrentFirmwareVersion) == 0) {
		printWithParity_P(firmwareVersion);
		printWithParity_P(PSTR("ok"));
	  } else if (strcmp_P(inputMessage_TextPart, msg_HomeToolRotate) == 0) {
		CheckToolRotateHomePosition();
		printWithParity_Combined(inputString, msg_OK);
      } else {
        // Handle command not found
        printWithParity_P(msg_COMMAND_NOT_FOUND);
      }
    } else {
      printWithParity_P(msg_ParityCheckFailed);
    }

    memset(inputString, 0, inputStringSize);
    inputStringIndex = 0;
    stringComplete = false;
  }
}
