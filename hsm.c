#include "hsm.h"

static void populate_hsm_state(union HSM_State * const hsm_state, union HSM_State_Class * const hsm_state_class);
static void hsm_delete(struct Object * const obj);
static union St_Machine_State * hsm_get_state(union State_Machine * const st_m);

static union HSM HSM = {{NULL}};

HSM_Class_T HSM_Class = 
{
	{&State_Machine_Class, hsm_delete},
	NULL,
	hsm_get_state,
};

void Populate_HSM(union HSM * const this,
		struct HSM_Chart * const hsm_chart,
		size_t const n_chart,
		union St_Machine_State * const states,
		size_t const nstates)
{
	if(NULL == HSM.vtbl)
	{


	}

	memcpy(this, &HSM, sizeof(HSM));



}
