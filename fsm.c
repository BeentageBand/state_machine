#define COBJECT_IMPLEMENTATION
#include "fsm.h"

static void fsm_delete(struct Object * const obj);
static union St_Machine_State * fsm_get_state(union State_Machine * const st_m, STID_T const stid);

FSM_Class_T FSM_Class = 
{
   {fsm_delete, NULL},
};

static union FSM FSM = {NULL};

void fsm_delete(struct Object * const obj){}

union St_Machine_State * fsm_get_state(union State_Machine * const st_m, STID_T const stid)
{
   if(stid >= st_m->n_states) return NULL;
   return st_m->st_chart + stid;
}

void Populate_FSM(union FSM * const this,
      struct FSM_Chart * const transition_chart, 
      size_t ntransitions,
      union St_Machine_State * const st_chart,
      size_t const n_states)
{
   if(ntransitions != n_states) return;
   if(NULL == FSM.vtbl)
   {
      Populate_State_Machine(&FSM.State_Machine, NULL, 0, 0);
      Object_Init(&FSM.Object, &FSM_Class.Class, sizeof(FSM_Class.State_Machine));
      FSM_Class.State_Machine.get_state = fsm_get_state;
   }
   _clone(this, FSM);
   Populate_State_Machine(&this->State_Machine, st_chart, n_states, 0);
   this->vtbl = &FSM_Class;

   uint32_t i = n_states;
   while(i--)
   {
      Populate_St_Machine_State(st_chart + i, transition_chart[i].stid,
            transition_chart[i].transition_tb, transition_chart[i].ntransitions);
   }
}
