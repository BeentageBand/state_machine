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

typedef union FSM_Class 
{
	struct State_Machine_Class State_Machine;
	struct Class Class;
}FSM_Class_T;

typedef union FSM
{
   FSM_Class_T _private * _private vtbl;
   union State_Machine State_Machine;
   struct Object Object;
}FSM_T;

extern FSM_Class_T _private FSM_Class;

extern void Populate_FSM(union FSM * const fsm,
      struct St_Machine_Transition * const transition_chart, 
      size_t ntransitions,
      union St_Machine_State * const st_chart,
      size_t const n_states);
/* 
 * Example:
 * #define SOME_FSM(cb) \
 * FSM_STATE_DEF(cb, INIT_STID, \
 *      FSM_TRANSITION_DEF(cb, SIGNAL_1_MID, STATE_1_STID, guard_1, process_action_1) \
 *      FSM_TRANSITION_DEF(cb, SIGNAL_2_MID, STATE_2_STID, guard_2, process_action_2) \
 *      ) \
 * FSM_STATE_DEF(cb, STATE_1_STID, \
 *      FSM_TRANSITION_DEF(cb, SIGNAL_SHUT_MID, DONE_STID, guard_shut, process_action_shut) \
 *      FSM_TRANSITION_DEF(cb, SIGNAL_2_MID, STATE_2_STID, guard_2, process_action_2) \
 *      ) \
 * FSM_STATE_DEF(cb, STATE_2_STID, \
 *      FSM_TRANSITION_DEF(cb, SIGNAL_SHUT_MID, DONE_STID, guard_shut, process_action_shut) \
 *      FSM_TRANSITION_DEF(cb, SIGNAL_1_MID, STATE_1_STID, guard_action_1, process_action_1) \
 *      ) \
 * FSM_STATE_DEF(cb, DONE_STID, \
 *      FSM_TRANSITION_DEF(cb, SIGNAL_1_MID, INIT_STID, guard_done, process_action_done) \
 *      FSM_TRANSITION_DEF(cb, SIGNAL_2_MID, INIT_STID, guard_done, process_action_done) \
 *      ) \
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
   state_def(Declare_TrChart) \
   struct FSM_Chart fsm_chart_name []= \
   { \
   state_def(Declare_StChart) \
   };

#define FSM_STATE_DEF(cb, stid, transitions) CAT(FSM_State_, cb) (stid, transitions)
#define FSM_State_Declare_StChart(stid, transition) {stid, CAT(stid, _Transition_Tb), Num_Elems(CAT(stid, _Transition_Tb))},
#define FSM_State_Declare_TrChart(stid, ...) \
   struct St_Machine_Transition CAT(stid, _Transition_Tb) [] = \
   { \
      __VA_ARGS__ \
   };
#define FSM_TRANSITION_DEF(cb, mid, stid, guard, action) CAT(FSM_Transition_, cb)(mid, stid, guard, action)
#define FSM_Transition_Declare_Handles(mid, stid, guard, action)
#define FSM_Transition_Declare_StChart(mid, stid, guard, action)
#define FSM_Transition_Declare_TrChart(mid, stid, guard, action) {mid, stid, guard, action},


#ifdef __cplusplus
}
#endif
#endif /*FSM_H_*/
