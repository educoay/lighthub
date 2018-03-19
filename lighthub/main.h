//
// Created by livello on 13.03.18.
//

#ifndef LIGHTHUB_MAIN_H
#define LIGHTHUB_MAIN_H

#endif //LIGHTHUB_MAIN_H

#include "Arduino.h"
#include "DallasTemperature.h"

void watchdogSetup(void);
void callback(char* topic, byte* payload, unsigned int length);
#ifndef __ESP__
void printIPAddress();
#endif
void printMACAddress();
void restoreState();
int  lanLoop();
void Changed (int i, DeviceAddress addr, int val);
void modbusIdle(void);
void _handleHelp(int arg_cnt, char **args);
void _kill(int arg_cnt, char **args);
void parseConfig();
void _loadConfig (int arg_cnt, char **args);
int loadConfig (int arg_cnt, char **args);
void _mqttConfigReq (int arg_cnt, char **args);
int mqttConfigReq (int arg_cnt, char **args);
int mqttConfigResp (char * as);
void _saveConfig(int arg_cnt, char **args);
void _setConfig(int arg_cnt, char **args);
void _getConfig(int arg_cnt, char **args);
void printBool (bool arg);
void saveFlash(short n, char* str);
void loadFlash(short n, char* str);
int getConfig (int arg_cnt, char **args);
void preTransmission();
void postTransmission();
void setup_main();
void loop_main();
void owIdle(void);
void modbusIdle(void);
void inputLoop(void);
void modbusLoop(void);
void thermoLoop(void);
short thermoSetCurTemp(char * name, short t);