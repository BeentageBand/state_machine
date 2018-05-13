#define COBJECT_IMPLEMENTATION
#include "state_machine.h"
#include "dbg_log.h"

static void state_machine_delete(struct Object * const obj);
static void state_machine_dispatch(union State_Machine * const this, union Mail * const mail);
static bool state_machine_transition_to(union State_Machine * const this, struct St_Machine_Transition * const transition,
		union St_Machine_State * const state);
static union St_Machine_State * state_machine_get_state(union State_Machine * const this, STID_T const stid);

struct State_Machine_Class State_Machine_Class =
{
      {state_machine_delete, NULL},
      state_machine_dispatch,
      state_machine_transition_to,
	  state_machine_get_state
};

static State_Machine State_Machine = {{NULL}};

void state_machine_delete(struct object * const obj)
{
	union State_Machine * const this = _cast(State_Machine, obj);
	memcpy(this, &State_Machine, sizeof(State_Machine));
	this->vtbl = NULL;

}

void state_machine_dispatch(union State_Machine * const this, union Mail * const mail)
{
	STID_T current_st = this->current_st;
	STID_T next_stid = this->initial_st; 
	struct St_Machine_Transition transition =  {0, next_stid, NULL, this->vtbl->init};

	if(NULL != current_st)
	{
		memcpy(&transition, current_st->vtbl->next_st(current_st, mail->mid), sizeof(transition));
		next_stid = transition.stid;
	}
	else
	{
		Dbg_Info("Init State Machine on STID = %d", stid);
	}

	union St_Machine_State * next_st = this->vtbl->get_state(this, next_stid);

	if(NULL != next_st)
	{
		this->vtbl->state_transition_to(this, &transition, next_st);
	}
	else
	{
		Dbg_Info("no event found for STID = %d, given MID = %d",
				next_stid,
				mail->mid);
	}
}

bool state_machine_transition_to(union State_Machine * const this, struct St_Machine_Transition * const transition,
		union St_Machine_State * const state)
{
	if(NULL != transition->guard)
	{
		if(!transition->guard(this, state))
		{
			return false;
		}
	}

	this->current_st = state;
	Isnt_Nullptr(transition->action, true);
	transition->action(this);
	return true;
}

union St_Machine_State * state_machine_get_state(union State_Machine * const this, STID_T const stid)
{
	union St_Machine_State * const state = NULL;
	Dbg_Fault("%s, abstract method, please override", __func__);

	if(stid < this->n_states)
	{
		state = this->st_char + stid;
	}
	return state;
}

void Populate_State_Machine(union State_Machine * const this,
		union St_Machine_State * const st_chart,
		size_t const n_states,
		STID_T const initial_st)
{
      if(NULL == State_Machine.vtbl)
      {
            State_Machine.vtbl = &State_Machine_Class;
            State_Machine.initial_st = STID_NOT_STATE;
            State_Machine.current_st = STID_NOT_STATE;
			State_Machine.st_chart = NULL;
			State_Machine.n_states = 0UL;
      }
      memcpy(this, &State_Machine, sizeof(State_Machine));
      this->initial_st = initial_st;
	  this->st_chart = st_chart;
	  this->n_states = n_states;
}
