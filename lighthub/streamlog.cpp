#include "streamlog.h"
#include <Arduino.h>
#include "statusled.h"

#if defined (STATUSLED)
extern statusLED LED;
#endif

#ifdef SYSLOG_ENABLE
extern bool syslogInitialized;
Streamlog::Streamlog (HardwareSerial * _serialPort, int _severity , Syslog * _syslog, uint8_t _ledPattern )
{
      serialPort=_serialPort;
      severity=_severity;
      syslog=_syslog;
      ledPattern=_ledPattern;
}
#else
Streamlog::Streamlog (HardwareSerial * _serialPort, int _severity,  uint8_t _ledPattern)
{
      serialPort=_serialPort;
      severity=_severity;
}
#endif

void Streamlog::begin(unsigned long speed)
{
  if (serialPort) serialPort->begin(speed);
};

void Streamlog::end()
{
  if (serialPort) serialPort->end();
};

int Streamlog::available(void)
{
  if (serialPort) return serialPort->available();
  return 0;
};

int Streamlog::peek(void)
{
  if (serialPort) return serialPort->peek();
  return 0;
};

int Streamlog::read(void)
{
  if (serialPort) return serialPort->read();
  return 0;
};


void Streamlog::flush(void)
{
  if (serialPort) serialPort->flush();

};

size_t Streamlog::write(uint8_t ch)
{
#ifdef SYSLOG_ENABLE
if (syslogInitialized)
  {
  if (ch=='\n')
              {
                logBuffer[logBufferPos]=0;
                if (syslog) syslog->log(severity,(char *)logBuffer);
                logBufferPos=0;
              }
      else
        {
          if (logBufferPos<LOGBUFFER_SIZE-1 && (ch!='\r')) logBuffer[logBufferPos++]=ch;
        }
   }
#endif

  #if defined (STATUSLED)
  if ((ch=='\n') && ledPattern) LED.flash(ledPattern);
  #endif

  if (serialPort) return serialPort->write(ch);

  return 1;
};
