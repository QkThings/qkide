#include <qk_program.h>
#include <qk_debug.h>
#include "AllFeatures.h"
       
qk_data  	dat_buf[DAT_COUNT];
qk_event	evt_buf[EVT_COUNT];
qk_action	act_buf[ACT_COUNT];

float time = 0.0;
float wave = 0.0;
float evt_args[3];
uint16_t counter = 10;

void test_sample()
{	
	// Send a debug string	
  QK_DEBUG("test_sample() [called %d times]", counter++);

	// Generate some data
  time += 0.01;
	wave = sin(2.0*M_PI*3.0*time)*cos(2.0*M_PI*time);
	
	// Fill data values
  qk_setDataValueF(0, time);
  qk_setDataValueF(1, sin(2.0*M_PI*2.0*time));
  qk_setDataValueF(2, wave);
	qk_setDataValueF(3, wave+0.5);
	qk_setDataValueF(4, wave+0.8);
	qk_setDataValueF(5, wave+1.0);

	// Fire some events
	if((counter % 10) == 0)
	{
  	evt_args[0] = 123.123+(float)counter;
  	evt_args[1] = 456.456+(float)counter;
  	qk_fireEvent(0, evt_args, 2, "event fired with some arguments");	
	}
	if((counter % 25) == 0)
	{
	  evt_args[0] = 1.123+(float)counter;
  	evt_args[1] = 2.456+(float)counter;
		evt_args[2] = 3.456+(float)counter;
  	qk_fireEvent(0, evt_args, 3, "arguments: %0, %1 and %2.");
	}
}

void qk_setup()
{
  qk_setBoardName("HelloArduino");
  qk_setBoardVersion(0x1234);

  qk_setDataBuffer(dat_buf, DAT_COUNT);
  qk_setDataType(QK_DATA_TYPE_FLOAT);
  qk_setDataLabel(0, "TIME");
  qk_setDataLabel(1, "SIN");
  qk_setDataLabel(2, "WAVE1");
	qk_setDataLabel(3, "WAVE2");
	qk_setDataLabel(4, "WAVE3");
	qk_setDataLabel(5, "WAVE4");

  qk_setEventBuffer(evt_buf, EVT_COUNT);
  qk_setEventLabel(0, "EVENT_0");

  qk_setSampleCallback(test_sample);
	
	qk_setSamplingFrequency(10);
}

int main(void)
{
  return qk_main();
}
