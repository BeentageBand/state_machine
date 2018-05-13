#ifndef FSM_H_
#define FSM_H_

#include "state_machine.h"

struct FSM_Chart
{
	STID_T stid;
	union St_Machine_Transition * transition_tb;
	size_t ntransitions;
};

typedef union State_Machine_Class FSM_Class_T;

typedef union FSM
{
	FSM_Class_T _private * _private vtbl;
	union State_Machine State_Machine;
}FSM_T;

extern FSM_Class_T _private FSM_Class;

extern void Populate_FSM(union FSM * const fsm,
		struct FSM_Chart * st_chart,
		size_t const char_size,
		union St_Machine_State * const states,
		size_t const n_states);

#define FSM_Declare_Chart(state_def, fsm_chart_name) \
	state_def(Declare_Handles) \
	state_def(Declare_TrChart) \
	static struct FSM_Chart fsm_chart_name []= \
	{ \
	state_def(Declare_StChart) \
	};
#define FSM_STATE_DEF(cb, stid, transitions) CAT(FSM_State, cb) (stid, transitions)
#define FSM_State_Declare_Handles(stid, transition) transition
#define FSM_State_Declare_StChart(stid, transition) {stid, CAT(stid, Transition_Tb), Num_Elems(CAT(stid, Transition_Tb))},
#define FSM_State_Declare_TrChart(stid, transition) \
	static struct St_Machine_Transition CAT(stid, Transition_Tb) [] = \
	{ \
		transition \
	};
#define FSM_TRANSITION_DEF(cb, mid, stid, action) CAT(FSM_Transition, cb)(mid, stid, action)
#define FSM_Transition_Declare_Handles(mid, stid, action) \
	static bool CAT(stid, guard)(union State_Machine * const fsm, union St_Machine_State * const state); \
	static void action(union State_Machine * const fsm); 
#define FSM_Transition_Declare_TrChart(mid, stid, action) {mid, stid, CAT(stid, guard), action},

#endif /*FSM_H_*/
