#include <Arduino.h>
#include "itemCmd.h"
#include "main.h"
#include "Streaming.h"
#include "item.h"

#ifdef ADAFRUIT_LED
#include <Adafruit_NeoPixel.h>
#else
#include "FastLED.h"
#endif

int txt2cmd(char *payload) {
  int cmd = CMD_UNKNOWN;
  if (!payload || !payload[0]) return cmd;

  // Check for command
  if (*payload == '-' || (*payload >= '0' && *payload <= '9')) cmd = CMD_VOID;
  else if (*payload == '%') cmd = CMD_UP;
  else if (*payload == '{') cmd = CMD_JSON;
  else if (*payload == '#') cmd = CMD_RGB;
  else
  {
    for(uint8_t i=1; i<commandsNum ;i++)
        if (strcmp_P(payload, commands_P[i]) == 0)
             {
//             debugSerial<< i << F(" ") << pgm_read_word_near(&serialModes_P[i].mode)<< endl;
             return i;
           }
  }

  /*
  else if (strncmp_P(payload, HSV_P, strlen (HSV_P)) == 0) cmd = CMD_HSV;
  else if (strncmp_P(payload, RGB_P, strlen (RGB_P)) == 0) cmd = CMD_RGB;
  */

  return cmd;
}

itemCmd::itemCmd(uint8_t _type, uint8_t _code)
{
  cmd.itemArgType=_type;
  cmd.cmdCode=_code;
}

itemCmd itemCmd::setDefault()
{
  switch (cmd.itemArgType){
    case ST_FLOAT_CELSIUS: param.asfloat=20.;
    break;
    case ST_FLOAT_FARENHEIT: param.asfloat=75.;
    break;
    case ST_HSV: param.h=100; param.s=0; param.v=100;
    break;
    case ST_HSV255: param.h=100; param.s=0; param.v=255;
    break;
    case ST_PERCENTS: param.v=100;
    break;
    case ST_PERCENTS255: param.v=255;
    break;
    default:
    param.asInt32=0;
  }
  return *this;
}

itemCmd itemCmd::setH(uint16_t h)
{
  int par=h;
  switch (cmd.itemArgType)
  {
    case ST_VOID:
      cmd.itemArgType=ST_HSV;
    case ST_HSV:
      if (par>100) par=100;
    case ST_HSV255:
      if (par>255) par=255;
      if (par<0) par=0;
      param.h=par;
  }
  return *this;
}

itemCmd itemCmd::setS(uint8_t s)
{
  int par=s;
  switch (cmd.itemArgType)
  {
    case ST_VOID:
      cmd.itemArgType=ST_HSV;
    case ST_HSV:
      if (par>100) par=100;
    case ST_HSV255:
      if (par>255) par=255;
      if (par<0) par=0;
      param.s=par;
  }
  return *this;
}

itemCmd itemCmd::incrementPercents(int16_t dif)
{ int par=param.v;
  switch (cmd.itemArgType)
  {
    case ST_PERCENTS:
    case ST_HSV:
     par+=dif;
     if (par>100) par=100;
     if (par<0) par=0;
    break;
    case ST_PERCENTS255:
    case ST_HSV255:
     par+=dif;
     if (par>255) par=255;
     if (par<0) par=0;
    break;
  }
  param.v=par;
  return *this;
}

itemCmd itemCmd::incrementH(int16_t dif)
{ int par=param.h;
  switch (cmd.itemArgType)
  {
  case ST_HSV:
  case ST_HSV255:
   par+=dif;
   if (par>365) par=0;
   if (par<0) par=365;
  break;
}
param.h=par;
return *this;
}

itemCmd itemCmd::incrementS(int16_t dif)
{int par=param.s;
  switch (cmd.itemArgType)
  {
    case ST_PERCENTS:
    case ST_HSV:
     par+=dif;
     if (par>100) par=100;
     if (par<0) par=0;
    break;
    case ST_PERCENTS255:
    case ST_HSV255:
     par+=dif;
     if (par>255) par=255;
     if (par<0) par=0;
    break;
  }
  param.s=par;
  return *this;

}


itemCmd itemCmd::assignFrom(itemCmd from)
{
  bool RGBW_flag   = false;
  bool HSV255_flag = false;

  switch (cmd.itemArgType){ //Destination
     case ST_HSV:
     case ST_PERCENTS:
        switch (from.cmd.itemArgType)
             {
              case ST_RGBW:
                   param.w=from.param.w;
              case ST_RGB:
                   param.r=from.param.r;
                   param.g=from.param.g;
                   param.b=from.param.b;
                   cmd.itemArgType=from.cmd.itemArgType; //Changing if type
                   break;
              case ST_HSV:
                   param.h=from.param.h;
                   param.s=from.param.s;
                   param.v=from.param.v;
                   break;
              case ST_PERCENTS:
                   param.v=from.param.v;
                   break;
              case ST_HSV255:
                   param.h=from.param.h;
                   param.s=map(from.param.s,0,255,0,100);
                   param.v=map(from.param.v,0,255,0,100);
                   break;
              case ST_PERCENTS255:
                   param.v=map(from.param.v,0,255,0,100);
                   break;
              default:
                  debugSerial<<F("Wrong Assignment ")<<from.cmd.itemArgType<<F("->")<<cmd.itemArgType<<endl;
              }
        case ST_HSV255:
        case ST_PERCENTS255:
           switch (from.cmd.itemArgType)
             {
               case ST_RGBW:
                    param.w=from.param.w;
               case ST_RGB:
                    param.r=from.param.r;
                    param.g=from.param.g;
                    param.b=from.param.b;
                    cmd.itemArgType=from.cmd.itemArgType;
                    break;
              case ST_HSV:
                   param.h=from.param.h;
                   param.s=map(from.param.s,0,100,0,255);
                   param.v=map(from.param.v,0,100,0,255);
                   break;
              case ST_PERCENTS:
                   param.v=map(from.param.v,0,100,0,255);
                   break;
              case ST_HSV255:
                   param.h=from.param.h;
                   param.s=from.param.s;
                   param.v=from.param.v;
                   break;
              case ST_PERCENTS255:
                   param.v=from.param.v;
                   break;
              default:
                       debugSerial<<F("Wrong Assignment ")<<from.cmd.itemArgType<<F("->")<<cmd.itemArgType<<endl;
              }

        case ST_VOID:
             cmd.itemArgType=from.cmd.itemArgType;

        case ST_INT32:
        case ST_UINT32:
        case ST_FLOAT:
        case ST_FLOAT_CELSIUS:
        case ST_FLOAT_FARENHEIT:
           param.asInt32=from.param.asInt32;
           break;

        case ST_RGBW:
             RGBW_flag=true;
        case ST_RGB:
        switch (from.cmd.itemArgType)
          {
            case ST_RGBW:
            case ST_RGB:
                  param.asInt32=from.param.asInt32;
                 break;
           case ST_HSV255:
                HSV255_flag=true;
           case ST_HSV:
                { // HSV_XX to RGB_XX translation code
                int rgbSaturation;
                int rgbValue;

                if (HSV255_flag)
                  {
                    rgbSaturation = from.param.s;
                    rgbValue      = from.param.v;
                  }
                else
                  {
                    rgbSaturation =map(from.param.s, 0, 100, 0, 255);
                    rgbValue =     map(from.param.v, 0, 100, 0, 255);
                  }

                  if (RGBW_flag)
                  {
                      if (rgbSaturation < 128) { // Using white
                                      param.w=map((127 - rgbSaturation) * rgbValue, 0, 127*255, 0, 255);
                                      int rgbvLevel = map (rgbSaturation,0,127,0,255*2);
                                      rgbValue = map(rgbValue, 0, 255, 0, rgbvLevel);
                                      rgbSaturation = map(rgbSaturation, 0, 127, 255, 100);
                                      if (rgbValue>255) rgbValue = 255;
                                     }
                      else
                      {
                        rgbSaturation = map(rgbSaturation, 128, 255, 100, 255);
                        param.w=0;
                      }
                  }
                  #ifdef ADAFRUIT_LED
                    Adafruit_NeoPixel strip(0, 0, 0);
                    uint32_t rgb = strip.ColorHSV(map(from.param.h, 0, 365, 0, 65535), rgbSaturation, rgbValue);
                    param.r=(rgb >> 16)& 0xFF;
                    param.g=(rgb >> 8) & 0xFF;
                    param.b=rgb & 0xFF;
                  #else
                    CRGB rgb = CHSV(map(from.param.h, 0, 365, 0, 255), rgbSaturation, rgbValue);
                    param.r=rgb.r;
                    param.g=rgb.g;
                    param.b=rgb.b;
                  #endif
                }
            default:
                    debugSerial<<F("Wrong Assignment ")<<from.cmd.itemArgType<<F("->")<<cmd.itemArgType<<endl;
         } //Translation to RGB_XX
        break;
     } //Destination
    return *this;
}

bool itemCmd::isCommand()
{
  return (cmd.cmdCode);
}

bool itemCmd::isValue()
{
return (cmd.itemArgType);
}


long int itemCmd::getInt()
{
  switch (cmd.itemArgType) {

    case ST_INT32:
    case ST_PERCENTS:
    case ST_UINT32:
    case ST_PERCENTS255:
    case ST_HSV:
    case ST_HSV255:
      return param.aslong;

    case ST_FLOAT:
    case ST_FLOAT_CELSIUS:
    case ST_FLOAT_FARENHEIT:
      return int (param.asfloat);
    default:
    return 0;
  }
}


short itemCmd::getPercents()
{
  switch (cmd.itemArgType) {

    case ST_PERCENTS:
    case ST_HSV:
      return param.v;

    case ST_PERCENTS255:
    case ST_HSV255:
      return map(param.v,0,255,0,100);

    default:
    return 0;
  }
}

short itemCmd::getPercents255()
{
  switch (cmd.itemArgType) {

    case ST_PERCENTS:
    case ST_HSV:
    return map(param.v,0,100,0,255);

    case ST_PERCENTS255:
    case ST_HSV255:
    return param.v;

    default:
    return 0;
  }
}

uint8_t itemCmd::getCmd()
{
  return cmd.cmdCode;
}

uint8_t itemCmd::getArgType()
{
  return cmd.itemArgType;
}

itemCmd itemCmd::setArgType(uint8_t type)
{
   cmd.itemArgType=type;
  return *this;
}

uint8_t itemCmd::getCmdParam()
{
  if (isCommand()) return cmd.cmdParam;
  return 0;
}

itemCmd itemCmd::Percents(int i)
    {
      if (i<0) i=0;
      if (i>100) i=100;

      switch (cmd.itemArgType)
      {
       case ST_HSV:
       case ST_PERCENTS:
         param.v=i;
       break;

       case ST_HSV255:
       case ST_PERCENTS255:
          param.v=map(i,0,100,0,255);
       default:
       cmd.itemArgType=ST_PERCENTS;
       param.v=i;
      }

      return *this;
    }

    itemCmd itemCmd::Percents255(int i)
        {
          if (i<0) i=0;
          if (i>255) i=255;

          switch (cmd.itemArgType)
          {
           case ST_HSV:
           case ST_PERCENTS:
             param.v=map(i,0,255,0,100);
           break;

           case ST_HSV255:
           case ST_PERCENTS255:
              param.v=i;;
           default:
           cmd.itemArgType=ST_PERCENTS255;
           param.v=i;
          }

          return *this;
        }

itemCmd itemCmd::Int(int32_t i)
        {
          cmd.itemArgType=ST_INT32;
          param.asInt32=i;
          return *this;
        }



itemCmd itemCmd::HSV(uint16_t h, uint8_t s, uint8_t v)
{
  cmd.itemArgType=ST_HSV;
  param.h=h;
  param.s=s;
  param.v=v;

  return *this;
}

itemCmd itemCmd::Int(uint32_t i)
                {
                  cmd.itemArgType=ST_UINT32;
                  param.asUint32=i;
                  return *this;
                }


itemCmd itemCmd::Cmd(uint8_t i)
    {
    //      cmd.itemArgType=ST_COMMAND;
          cmd.cmdCode=i;
          return *this;
    }


uint8_t itemCmd::getSuffix()
{
  return cmd.suffixCode;
}

itemCmd itemCmd::setSuffix(uint8_t suffix)
{
  cmd.suffixCode=suffix;
  return *this;
}

bool itemCmd::loadItem(Item * item, bool includeCommand)
{
  if (item && item->isValid())
  {
  param.asInt32=item->getVal();
  cmd.itemArgType=item->getSubtype();
  if (includeCommand) cmd.cmdCode=item->getCmd();
  return (cmd.itemArgType!=ST_VOID);
  }
return false;
}

bool itemCmd::saveItem(Item * item, bool includeCommand)
{
  if (item && item->isValid())
  {
  item->setVal(param.asInt32);
  item->setSubtype(cmd.itemArgType);
  if (includeCommand) item->setCmd(cmd.cmdCode);
  return true;
  }
return false;
}



char * itemCmd::toString(char * Buffer, int bufLen)
     {

       if (!Buffer || !bufLen) return NULL;
       *Buffer=0;
       char * argPtr=Buffer;
       if (isCommand())
                        {
                          int len;
                          strncpy_P(Buffer, commands_P[cmd.cmdCode], bufLen);
                          strncat(Buffer, " ", bufLen);
                          len=strlen(Buffer);
                          argPtr+=len;
                          bufLen-=len;
                        }

       switch (cmd.itemArgType)
       {

         case ST_PERCENTS:
         case ST_PERCENTS255:
         case ST_UINT32:
            snprintf(argPtr, bufLen, "%lu", param.asUint32);
         break;
         case ST_INT32:
            snprintf(argPtr, bufLen, "%ld", param.asInt32);

         break;
         case ST_HSV:
         case ST_HSV255:
         snprintf(argPtr, bufLen, "%d,%d,%d", param.h, param.s, param.v);

         break;
         case ST_FLOAT_CELSIUS:
         case ST_FLOAT_FARENHEIT:
         case ST_FLOAT:
         snprintf(argPtr, bufLen, "%.1f", param.asfloat);
         break;
         case ST_RGB:
         snprintf(argPtr, bufLen, "%d,%d,%d", param.r, param.g, param.b);
         break;

         case ST_RGBW:
         snprintf(argPtr, bufLen, "%d,%d,%d,%d", param.r, param.g, param.b,param.w);
         break;

         case ST_STRING:
         strncpy(argPtr, param.asString,bufLen);

         break;
         default:
         ;
       }
      return Buffer;
     }
