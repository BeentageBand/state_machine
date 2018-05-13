#ifndef ST_MACHINE_STATE_H_
#define ST_MACHINE_STATE_H_
#include "state_machine.h"

union St_Machine_State
{
	struct St_Machine_State _private * _private vtbl;
	struct
	{
		struct Object Object;
		STID_T _private stid;
		size_t nhandlers;
		struct St_Machine_Handle _private * _private handle;
	};
}St_Machine_State_T;

struct St_Machine_State_Class
{
	struct Class Class;
	STID_T (* _private next_st)(union St_Machine_State * const, IPC_MID_T const);
}St_Machine_State_Class_T;

extern St_Machine_State_Class_T _private St_Machine_State_Class;

extern void Populate_St_Machine_State(union St_Machine_State * const state, STID_T const stid,
		struct St_Machine_Handle * const handle_buff, size_t const nhandlers);

#endif /*ST_MACHINE_STATE_H_*/


