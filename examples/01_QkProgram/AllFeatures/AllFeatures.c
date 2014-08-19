#include <qk_program.h>
#include <qk_debug.h>
#include "AllFeatures.h"

qk_data   dat_buf[DAT_COUNT];
qk_event  evt_buf[EVT_COUNT];
qk_action act_buf[ACT_COUNT];
qk_config cfg_buf[CFG_COUNT];

float	time = 0.0;
float	wave = 0.0;
float	evt_args[3];
uint16_t	 counter = 10;

void sample_callback()
{
	// Send a debug string	
	QK_LOG_DEBUG("test_sample() [called %d times]", counter++);
		
	// Generate some data
	time += 0.01;
	wave = sin(2.0*M_PI*3.0*time)*cos(2.0*M_PI*time);

	// Fill data values
	qk_data_set_value_f(0, time);
	qk_data_set_value_f(1, sin(2.0*M_PI*2.0*time));
	qk_data_set_value_f(2, wave);
	qk_data_set_value_f(3, wave+0.5);
	qk_data_set_value_f(4, wave+0.8);
	qk_data_set_value_f(5, wave+1.0);

	// Generate some events
	if((counter % 10) == 0)
	{
  		evt_args[0] = 123.123+(float)counter;
  		evt_args[1] = 456.456+(float)counter;
		qk_event_set_args(0, evt_args, 2);
		//qk_event_generate(0, "event fired with some arguments");		
  		qk_event_generate(0, "testing");
	}
	if((counter % 25) == 0)
	{
		evt_args[0] = 1.123+(float)counter;
		evt_args[1] = 2.456+(float)counter;
		evt_args[2] = 3.456+(float)counter;
		qk_event_set_args(0, evt_args, 3);
		qk_event_generate(0, "arguments: %0, %1 and %2.");
	}
}

void action_callback(qk_action_id id)
{
	if(id == 0)
		hal_setLED(qk_action_get_value_b(0));
}

void qk_setup()
{
	qk_log_set_levels(QK_LOG_LEVEL_DEBUG | QK_LOG_LEVEL_ERROR);

	qk_board_set_name("HelloArduino");
	qk_board_set_version(0x1234);

	qk_config_set_buffer(cfg_buf, CFG_COUNT);
	
	qk_config_set_label(0, "INT");
	qk_config_set_type(0, QK_CONFIG_TYPE_INTDEC);
	qk_config_set_value_i(0, 123);

	qk_config_set_label(1, "HEX");
	qk_config_set_type(1, QK_CONFIG_TYPE_INTHEX);
	qk_config_set_value_i(1, 0xEFDA);

	qk_config_set_label(2, "BOOL");
	qk_config_set_type(2, QK_CONFIG_TYPE_BOOL);
	qk_config_set_value_b(2, true);

	qk_config_set_label(3, "FLOAT");
	qk_config_set_type(3, QK_CONFIG_TYPE_FLOAT);
	qk_config_set_value_f(3, 10.123);
	
	qk_datetime dt;
	qk_datetime_set_date(&dt, 14, 5, 19);
	qk_datetime_set_time(&dt, 22, 12, 45);

	qk_config_set_label(4, "DATETIME");
	qk_config_set_type(4, QK_CONFIG_TYPE_DATETIME);
	qk_config_set_value_dt(4, dt);

	qk_config_set_label(5, "TIME");
	qk_config_set_type(5, QK_CONFIG_TYPE_TIME);
	qk_config_set_value_dt(5, dt);

	qk_data_set_buffer(dat_buf, DAT_COUNT);
	qk_data_set_type(QK_DATA_TYPE_FLOAT);
	qk_data_set_label(0, "TIME");
	qk_data_set_label(1, "SIN");
	qk_data_set_label(2, "WAVE1");
	qk_data_set_label(3, "WAVE2");
	qk_data_set_label(4, "WAVE3");
	qk_data_set_label(5, "WAVE4");

	qk_event_set_buffer(evt_buf, EVT_COUNT);
	qk_event_set_label(0, "EVENT_0");
	
	qk_action_set_buffer(act_buf, ACT_COUNT);
	qk_action_set_label(0, "ACT_BOOL");
	qk_action_set_type(0, QK_ACTION_TYPE_BOOL);
	qk_action_set_label(1, "ACT_INT");
	qk_action_set_type(1, QK_ACTION_TYPE_INT);
	
	qk_action_set_callback(action_callback);

	qk_sampling_set_callback(QK_SAMPLING_CALLBACK_SAMPLE, sample_callback);
	qk_sampling_set_frequency(5);
}

int main(void)
{
	return qk_main();
}
