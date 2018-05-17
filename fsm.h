#ifndef FSM_H_
#define FSM_H_
#include "state_machine.h"

#ifdef __cplusplus
extern "C"{
#endif
struct FSM_Chart
{
	STID_T stid;
	struct St_Machine_Transition * transition_tb;
	size_t ntransitions;
};

typedef struct State_Machine_Class FSM_Class_T;

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
/* 
 * Example:
 * #define SOME_FSM(cb) \
 * FSM_STATE_DEF(cb, INIT_STID, \
 *		FSM_TRANSITION_DEF(cb, SIGNAL_1_MID, STATE_1_STID, process_action_1) \
 *		FSM_TRANSITION_DEF(cb, SIGNAL_2_MID, STATE_2_STID, process_action_2) \
 *		) \
 * FSM_STATE_DEF(cb, STATE_1_STID, \
 *		FSM_TRANSITION_DEF(cb, SIGNAL_SHUT_MID, DONE_STID, process_action_shut) \
 *		FSM_TRANSITION_DEF(cb, SIGNAL_2_MID, STATE_2_STID, process_action_2) \
 *		) \
 * FSM_STATE_DEF(cb, STATE_2_STID, \
 *		FSM_TRANSITION_DEF(cb, SIGNAL_SHUT_MID, DONE_STID, process_action_shut) \
 *		FSM_TRANSITION_DEF(cb, SIGNAL_1_MID, STATE_1_STID, process_action_1) \
 *		) \
 * FSM_STATE_DEF(cb, DONE_STID, \
 *		FSM_TRANSITION_DEF(cb, SIGNAL_1_MID, INIT_STID, process_action_done) \
 *		FSM_TRANSITION_DEF(cb, SIGNAL_2_MID, INIT_STID, process_action_done) \
 *		) \
 *
 * Use MACROS:
 *
 * //FSM Chart declaration
 * FSM_Declare_Chart(SOME_FSM, SOME_FSM_Chart)
 *
 * Implement:
 * static bool STATE_INIT_STID_guard(union State_Machine * const fsm, union St_Machine_State * const state)
 * {...}
 * .... // Implement each state guard
 * 
 * static bool process_action_1(union State_Machine * const fsm)
 * {...} // Implement each action
 */

#define FSM_Declare_Chart(state_def, fsm_chart_name) \
	state_def(Declare_Handles) \
	state_def(Declare_TrChart) \
	static struct FSM_Chart fsm_chart_name []= \
	{ \
	state_def(Declare_StChart) \
	};

#define FSM_STATE_DEF(cb, stid, transitions) CAT(FSM_State_, cb) (stid, transitions)
#define FSM_State_Declare_Handles(stid, transition) \
	static bool CAT(stid, _guard)(union State_Machine * const fsm, union St_Machine_State * const state); \
	transition
#define FSM_State_Declare_StChart(stid, transition) {stid, CAT(stid, _Transition_Tb), Num_Elems(CAT(stid, _Transition_Tb))},
#define FSM_State_Declare_TrChart(stid, ...) \
	static struct St_Machine_Transition CAT(stid, _Transition_Tb) [] = \
	{ \
		__VA_ARGS__ \
	};
#define FSM_TRANSITION_DEF(cb, mid, stid, action) CAT(FSM_Transition_, cb)(mid, stid, action)
#define FSM_Transition_Declare_Handles(mid, stid, action)
#define FSM_Transition_Declare_StChart(mid, stid, action)
#define FSM_Transition_Declare_TrChart(mid, stid, action) {mid, stid, CAT(stid, _guard), action},


#ifdef __cplusplus
}
#endif
#endif /*FSM_H_*/
