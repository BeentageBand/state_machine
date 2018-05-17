#ifndef HSM_H_
#define HSM_H_
#include "state_machine.h"

#ifdef __cplusplus
extern "C" {
#endif

struct HSM_Chart
{
   IPC_MID_T mid;
   STID_T stid;
   STDI_T super_stid;
   void (* on_entry)(union HSM * const);
   void (* on_exit)(union HSM * const);
   struct St_Machine_Transitions * transition_tb;
   size_t ntransitions;
};

typedef struct State_Machine_Class HSM_Class_T;
typedef union St_Machine_State HSM_State_T;

union HSM 
{
   HSM_Class_T _private * _private vtbl;
   union State_Machine State_Machine;
}HSM_T;

union HSM_State_Class
{
   struct 
   {
      struct St_Machine_State St_Machine_State;
      void (* _private entry)(union St_Machine_State * const, union HSM * const);
      void (* _private exit)(union St_Machine_State * const, union HSM * const);
   };
   struct Class Class;
};

extern HSM_Class_T _private HSM_Class;

extern void Populate_HSM(union HSM * const hsm,
      struct HSM_Chart * const hsm_chart,
      size_t const n_chart,
      union St_Machine_State * const states,
      size_t const nstates);

#define HSM_Declare_Chart(state_def, hsm_name) \
   state_def(Declare_Handle) \
    state_def(Declare_State) \
   static HSM_State_T HSM_State [] = \
   { \
      state_def(Declare_Chart) \
   };

#define HSM_STATE_DEF(cb, stid, entry, exit, transition) CAT(HSM_STATE, cb)(stid, entry, exit, transition)
#define HSM_TRANSITION_DEF(cb, mid, guard, action) CAT(HSM_STATE, cb)(stid, mid, guard, action)
#define HSM_STATE_Declare_Handle(stid, entry, exit, transition) transition
#define HSM_STATE_Declare_State(stid, entry, exit, transition) \
   static HSM_State_Class_T CAT(stid, State_Class) = \
   { \
      {{{NULL, CAT(stid, state_delete}}, \
         CAT(stid, state_entry), \
         CAT(stid, state_exit) \
      } \
   };

#ifdef __cplusplus
}
#endif
#endif /*HSM_H_*/
