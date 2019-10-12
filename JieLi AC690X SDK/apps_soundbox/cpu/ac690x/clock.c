#include "clock.h"
#include "clock_api.h"
#include "bluetooth/avctp_user.h"
#include "bluetooth/bluetooth_api.h"


struct clock_switch_user{
    struct list_head clock_enter_head; 
    struct list_head clock_exit_head; 
};

static struct clock_switch_user clock_switch_user_t;

#define __this      (&clock_switch_user_t)


void clock_switch_register(u8 type, struct clock_switch *switcher, int (*fun)(void), char *user)
{
    struct list_head *head;

    head = (type == CLOCK_SWITCH_ENTER) ? &__this->clock_enter_head : &__this->clock_exit_head;

	struct clock_switch *p;

    switcher->fun = fun;
    switcher->user = user;

	list_for_each_entry(p, head, entry){
		if (p == switcher){
            //re register so do nothing
            return;
		}
	}

	list_add_tail((struct list_head *)&switcher->entry, head);
}

void set_sys_freq_init(void)
{
    INIT_LIST_HEAD(&__this->clock_enter_head);
    INIT_LIST_HEAD(&__this->clock_exit_head);
}

void clock_init_app(SYS_CLOCK_INPUT sys_in, u32 input_freq, u32 out_freq)
{
    clock_init(sys_in, input_freq, out_freq);

    set_sys_freq_init();

    init_t *init_func;

    list_for_each_clock_switcher(init_func)
    {
        (*init_func)();
    }
}


AT(.common)
void set_sys_freq(u32 out_freq)
{
	if(out_freq == SYS_CLK)
		return;

	struct clock_switch *p;
	
	list_for_each_entry(p, &__this->clock_enter_head, entry){
        p->fun();
	}

    ble_controller_suspend();
	user_prote_bt_process(1);

    clock_set_sys_freq(out_freq);

	user_prote_bt_process(0);
    ble_controller_resume();

	list_for_each_entry(p, &__this->clock_exit_head, entry){
        p->fun();
	}

	printf("set_sys_freq = %d\n",SYS_CLK);
}
