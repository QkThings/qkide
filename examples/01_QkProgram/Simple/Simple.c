#include <qk_program.h>
#include <qk_debug.h>
#include "Simple.h"

#define CFG_COUNT 1
#define DAT_COUNT 2
#define EVT_COUNT 1
#define ACT_COUNT 1

qk_data   dat_buf[DAT_COUNT];
qk_event  evt_buf[EVT_COUNT];
qk_action act_buf[ACT_COUNT];
qk_config cfg_buf[CFG_COUNT];

float data_cnt = 0.0;
float time = 0.0, wave;
float evt_args[1];

void sample_callback()
{
	// Generate some data
	time += 0.01;
	wave = sin(2.0*M_PI*3.0*time)*cos(2.0*M_PI*time);
	qk_data_set_value_f(0, wave);
	qk_data_set_value_f(1, data_cnt++);

	// Generate debug string
    if(((int)data_cnt % 25) == 0)
        QK_LOG_DEBUG("hello %d", data_cnt);

	// Generate event
    if(((int)data_cnt % 50) == 0)
    {
        evt_args[0] = (int) data_cnt;
        qk_event_set_args(0, evt_args, 1);
        qk_event_generate(0, "Milestone reached: %0");
    }
        
}

void action_callback(qk_action_id id)
{
	if(id == 0)
		qk_board_led_set(qk_action_get_value_b(0));
}

void qk_setup()
{
	qk_log_set_levels(QK_LOG_LEVEL_DEBUG | QK_LOG_LEVEL_ERROR);

	qk_board_set_name("test_simple");

	qk_config_set_buffer(cfg_buf, CFG_COUNT);
	
	qk_config_set_label(0, "INT");
	qk_config_set_type(0, QK_CONFIG_TYPE_INTDEC);
	qk_config_set_value_i(0, 123);

	qk_data_set_buffer(dat_buf, DAT_COUNT);
	qk_data_set_type(QK_DATA_TYPE_INT);
	qk_data_set_label(0, "CNT");

	qk_event_set_buffer(evt_buf, EVT_COUNT);
	qk_event_set_label(0, "EVT");
	
	qk_action_set_buffer(act_buf, ACT_COUNT);
	qk_action_set_label(0, "ACT_BOOL");
	qk_action_set_type(0, QK_ACTION_TYPE_BOOL);

	qk_action_set_callback(action_callback);

	qk_sampling_set_callback(QK_SAMPLING_CALLBACK_SAMPLE, sample_callback);	
	qk_sampling_set_frequency(10);
}

int main(void)
{
	return qk_main();
}
