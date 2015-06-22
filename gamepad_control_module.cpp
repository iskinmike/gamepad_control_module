#ifdef _WIN32
	#define _CRT_SECURE_NO_WARNINGS
#endif	

#include <stdlib.h>
#include <time.h>

#include <string>
#include <map>
#include <iostream>

#include <dinput.h>

#include <module.h>
#include <control_module.h>
#include <SimpleIni.h>
#include "gamepad_control_module.h"


// GLOBAL VARIABLES
unsigned int COUNT_AXIS = 18;

bool is_error_init = false;

// LT/RT Bool variables
bool is_T_was_changed[4] = { 0, 0, 0, 0 };
// B1-B4 Bool variables
bool is_B_was_changed[4] = { 0, 0, 0, 0 };
// Arrows Bool variable
bool is_Arrows_was_changed = false;
// R1/2 / L1/2 select start Bool variables
bool is_RLSS_was_changed[6] = { 0, 0, 0, 0, 0, 0 };

// All buttons
bool is_Button_was_changed[18] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
// All Sticks
bool is_Stick_was_changed[4] = { 0, 0, 0, 0 };


#ifdef _WIN32
	EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#endif

// MACROSES

#define SEND_BUTTON_CHANGE(index_of_button)\
	if (JState.rgbButtons[index_of_button] != 0){\
	(*sendAxisState)(index_of_button + 1, 1);\
	is_Button_was_changed[index_of_button] = true;\
	}\
	else {\
		if (is_Button_was_changed[index_of_button]){\
			(*sendAxisState)(index_of_button + 1, 0);\
			is_Button_was_changed[index_of_button] = false;\
		}\
	}

inline const char *copyStrValue(const char *source) {
	char *dest = new char[strlen(source) + 1];
	strcpy(dest, source);
	return dest;
}

BOOL CALLBACK callForEnumDevices(LPCDIDEVICEINSTANCE pdInst, LPVOID pvRef){
	std::cout << "callback function works!" << std::endl;
	//std::cout << pdInst->tszInstanceName << std::endl;

	*((GUID *)pvRef) = (pdInst->guidInstance);

	//std::cout << (*((GUID *)pvRef)).Data1 << std::endl;
	return DIENUM_STOP;
}

void GamepadControlModule::execute(sendAxisState_t sendAxisState) {
	srand(time(NULL));
	HRESULT hr;
	if (FAILED(hr = joystick->Acquire())) {
		return;
	}
	try {
		while (true) {
			joystick->Poll();
			joystick->GetDeviceState(sizeof(JState), &JState);
			for (auto i = axis_bind_map.begin(); i != axis_bind_map.end(); i++){
				switch (i->first)
				{
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 12: // binary axis
				{
					if (JState.rgbButtons[i->first - 1] != 0 && i->second=="Exit"){
						throw std::exception();
					}
					if (JState.rgbButtons[i->first - 1] != 0){
						(*sendAxisState)(axis_names[i->second], axis[axis_names[i->second]]->upper_value);
						is_Button_was_changed[i->first - 1] = true;
					}
					else {
						if (is_Button_was_changed[i->first - 1]){
							(*sendAxisState)(axis_names[i->second], axis[axis_names[i->second]]->lower_value);
							is_Button_was_changed[i->first - 1] = false;
						}
					}
					break;
				}
				case 13: // .lRz
				{
					if (JState.lRz != 32767){
						(*sendAxisState)(axis_names[i->second], JState.lRz);
						is_Button_was_changed[i->first - 1] = true;
					}
					else {
						if (is_Button_was_changed[i->first - 1]){
							(*sendAxisState)(axis_names[i->second], 32767);
							is_Button_was_changed[i->first - 1] = false;
						}
					}
					break;
				}
				case 14: // .lX
				{
					if (JState.lX != 32767){
						(*sendAxisState)(axis_names[i->second], JState.lX);
						is_Button_was_changed[i->first - 1] = true;
					}
					else {
						if (is_Button_was_changed[i->first - 1]){
							(*sendAxisState)(axis_names[i->second], 32767);
							is_Button_was_changed[i->first - 1] = false;
						}
					}
					break;
				}
				case 15: // .lY
				{
					if (JState.lY != 32767){
						(*sendAxisState)(axis_names[i->second], JState.lY);
						is_Button_was_changed[i->first - 1] = true;
					}
					else {
						if (is_Button_was_changed[i->first - 1]){
							(*sendAxisState)(axis_names[i->second], 32767);
							is_Button_was_changed[i->first - 1] = false;
						}
					}
					break;
				}
				case 16: // .lZ
				{
					if (JState.lZ != 32767){
						(*sendAxisState)(axis_names[i->second], JState.lZ);
						is_Button_was_changed[i->first - 1] = true;
					}
					else {
						if (is_Button_was_changed[i->first - 1]){
							(*sendAxisState)(axis_names[i->second], 32767);
							is_Button_was_changed[i->first - 1] = false;
						}
					}
					break;
				}
				case 17:{ // Arrows UD
					if (JState.rgdwPOV[0] != 4294967295){
						switch (JState.rgdwPOV[0]){
						case 0:
						case 4500:
						case 4500 * 7:{
							(*sendAxisState)(axis_names[i->second], 1);
							is_Button_was_changed[i->first - 1] = true;
							break;
						}
						case 3 * 4500:
						case 4 * 4500:
						case 5 * 4500:{
							(*sendAxisState)(axis_names[i->second], -1);
							is_Button_was_changed[i->first - 1] = true;
							break;
						}
						default:{break; }
						}
					}
					else {
						if (is_Button_was_changed[i->first - 1]){
							(*sendAxisState)(axis_names[i->second], 0);
							is_Button_was_changed[i->first - 1] = false;
						}
					}
					break;
				}
				case 18:{ // Arrows LR
					if (JState.rgdwPOV[0] != 4294967295){
						switch (JState.rgdwPOV[0]){
						case 4500:
						case 2 * 4500:
						case 3 * 4500:{
							(*sendAxisState)(axis_names[i->second], 1);
							is_Button_was_changed[i->first - 1] = true;
							break;
						}
						case 5 * 4500:
						case 6 * 4500:
						case 7 * 4500:{
							(*sendAxisState)(axis_names[i->second], -1);
							is_Button_was_changed[i->first - 1] = true;
							break;
						}
						default:{break; }
						}
					}
					else {
						if (is_Button_was_changed[i->first - 1]){
							(*sendAxisState)(axis_names[i->second], 0);
							is_Button_was_changed[i->first - 1] = false;
						}
					}
					break;
				}


				default:
					break;
				}
			}
		}
	}
	catch (...){}
/*
	for (int i = 0; i < 10; i++){
		joystick->Poll();
		joystick->GetDeviceState(sizeof(JState), &JState);

		int x = rand() % 2;//int x = rand() % 201 - 100;
		int y = rand() % 2;
		int z = rand() % 2;//int z = rand() % 101;
		std::cout << x << " " << y << " " << z << std::endl;
		if (JState.rgbButtons[0] == 128){
			(*sendAxisState)(1, 1);
		}
		else{ (*sendAxisState)(1, 0); }
		
		(*sendAxisState)(2, y);
		(*sendAxisState)(3, z);

#ifdef _WIN32
		Sleep(1000);
#else
		usleep(1000000);
#endif	
	}
*/
	joystick->Unacquire();
}


void GamepadControlModule::prepare(colorPrintf_t *colorPrintf_p, colorPrintfVA_t *colorPrintfVA_p) {
	srand(time(NULL));
	this->colorPrintf = colorPrintf_p;

	int axis_id = 0;
	Gamepad_axis = new AxisData*[COUNT_AXIS];

#ifdef _WIN32
	WCHAR DllPath[MAX_PATH] = { 0 };
	GetModuleFileNameW((HINSTANCE)&__ImageBase, DllPath, (DWORD)MAX_PATH);

	WCHAR *tmp = wcsrchr(DllPath, L'\\');
	WCHAR wConfigPath[MAX_PATH] = { 0 };

	size_t path_len = tmp - DllPath;

	wcsncpy(wConfigPath, DllPath, path_len);
	wcscat(wConfigPath, L"\\config.ini");

	char ConfigPath[MAX_PATH] = { 0 };
	wcstombs(ConfigPath, wConfigPath, sizeof(ConfigPath));
#else
	Dl_info PathToSharedObject;
	void * pointer = reinterpret_cast<void*> (getControlModuleObject);
	dladdr(pointer, &PathToSharedObject);
	std::string dltemp(PathToSharedObject.dli_fname);

	int dlfound = dltemp.find_last_of("/");

	dltemp = dltemp.substr(0, dlfound);
	dltemp += "/config.ini";

	const char* ConfigPath = dltemp.c_str();
#endif

	CSimpleIniA ini;
	ini.SetMultiKey(true);

	if (ini.LoadFile(ConfigPath) < 0) {
		(*colorPrintf)(this, ConsoleColor(ConsoleColor::yellow), "Can't load '%s' file!\n", ConfigPath);
		is_error_init = true;
		return;
	}

	CSimpleIniA::TNamesDepend axis_names_ini;
	ini.GetAllKeys("axis", axis_names_ini);
	axis_id++;
	for (
		CSimpleIniA::TNamesDepend::const_iterator i = axis_names_ini.begin();
		i != axis_names_ini.end();
		++i)
	{
		long temp = 0;
		temp = ini.GetLongValue("axis", i->pItem, 0);

		if (temp){
			std::string col_axis(i->pItem);
			std::pair<int, std::string> tempPair(temp, col_axis);
			axis_bind_map.insert(tempPair);

			axis_names[col_axis] = axis_id;
			axis[axis_id] = new AxisData();
			axis[axis_id]->axis_index = axis_id;
			axis[axis_id]->name = copyStrValue(col_axis.c_str());
			axis[axis_id]->upper_value = ini.GetLongValue(i->pItem, "upper_value", 1);
			axis[axis_id]->lower_value = ini.GetLongValue(i->pItem, "lower_value", 0);

			axis_id++;
		}
	}

	COUNT_AXIS = axis.size();
	Gamepad_axis = new AxisData*[COUNT_AXIS];
	for (unsigned int j = 0; j < COUNT_AXIS; ++j) {
		Gamepad_axis[j] = new AxisData;
		Gamepad_axis[j]->axis_index = axis[j + 1]->axis_index;
		Gamepad_axis[j]->lower_value = axis[j + 1]->lower_value;
		Gamepad_axis[j]->upper_value = axis[j + 1]->upper_value;
		Gamepad_axis[j]->name = axis[j + 1]->name;
	}

}

int GamepadControlModule::init() {

	LPDIRECTINPUT8 di;
	HRESULT hr;

	// Create a DirectInput device
	if (FAILED(hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION,
		IID_IDirectInput8, (VOID**)&di, NULL))) {
		return hr;
	}

	//LPDIRECTINPUTDEVICE8W joystick;
	ZeroMemory(&joystick, sizeof(LPDIRECTINPUTDEVICE8W));
	GUID JoyGUID;

	if (FAILED(hr = di->EnumDevices(DI8DEVCLASS_GAMECTRL, callForEnumDevices, &JoyGUID, DIEDFL_ATTACHEDONLY))){
		std::cout << "cant enum" << std::endl;
		return hr;
	};
	if (FAILED(hr = di->CreateDevice(JoyGUID, &joystick, NULL))){
		std::cout << "cant CreateDevice" << std::endl;
		return hr;
	};
	std::cout << JoyGUID.Data1 << std::endl;

	// ������������� ������ ������
	if (FAILED(hr = joystick->SetDataFormat(&c_dfDIJoystick))){
		std::cout << "cant Set DATA" << std::endl;
	}
	
	// 
	HWND hwndC = GetConsoleWindow();
	if (FAILED(hr = joystick->SetCooperativeLevel(hwndC, DISCL_NONEXCLUSIVE))){
		std::cout << "cant Set COOP" << std::endl;
	}

	// 
	DIPROPRANGE dipr;
	
	ZeroMemory(&JState, sizeof(DIJOYSTATE));
	// ������� ������� ���������
	ZeroMemory(&dipr, sizeof(DIPROPRANGE));

	dipr.diph.dwSize = sizeof(dipr);
	dipr.diph.dwHeaderSize = sizeof(dipr);
	// ��� ��� X
	dipr.diph.dwObj = DIJOFS_X;
	dipr.diph.dwHow = DIPH_BYOFFSET; // �������� � ������� ������
	// �������� ��������
	dipr.lMin = -1024;
	dipr.lMax = 1024;
	// Set to X
	joystick->SetProperty(DIPROP_RANGE, &dipr.diph);
	// Set to Y
	dipr.diph.dwObj = DIJOFS_Y;
	joystick->SetProperty(DIPROP_RANGE, &dipr.diph);

	// ��������� ������� ����
	DIPROPDWORD dipdw;
	ZeroMemory(&dipdw, sizeof(DIPROPDWORD));

	dipdw.diph.dwObj = DIJOFS_X; // ��� X
	dipdw.dwData = 1500;     // �������������� 15% ���������
	// Dead zone to X
	joystick->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
	// dead zone to Y
	dipdw.diph.dwObj = DIJOFS_Y; // ��� Y
	joystick->SetProperty(DIPROP_DEADZONE, &dipdw.diph);

	return  0;
}

const char *GamepadControlModule::getUID() {
	return "Gamepad control module 1";
}

AxisData** GamepadControlModule::getAxis(unsigned int *count_axis) {
	(*count_axis) = COUNT_AXIS;
	std::cout << "getAxis works!" << std::endl;
	return Gamepad_axis;
}

void GamepadControlModule::destroy() {
	for (unsigned int j = 0; j < COUNT_AXIS; ++j) {
		delete Gamepad_axis[j];
	}
	delete[] Gamepad_axis;
	delete this;
}

void *GamepadControlModule::writePC(unsigned int *buffer_length) {
	*buffer_length = 0;
	return NULL;
}

int GamepadControlModule::startProgram(int uniq_index) {
	return 0;
}

void GamepadControlModule::readPC(void *buffer, unsigned int buffer_length) {
}

int GamepadControlModule::endProgram(int uniq_index) {
	return 0;
}

PREFIX_FUNC_DLL  ControlModule* getControlModuleObject() {
	return new GamepadControlModule();
}