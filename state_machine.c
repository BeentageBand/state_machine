#define COBJECT_IMPLEMENTATION
#undef Dbg_FID
#define Dbg_FID DBG_FID_DEF(STATE_MACHINE_FID,0)
#include "dbg_log.h"
#include "state_machine.h"

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

static union State_Machine State_Machine = {NULL};

void state_machine_delete(struct Object * const obj)
{
   union State_Machine * const this = (union State_Machine *)Object_Cast(&State_Machine_Class.Class, obj);
   memcpy(this, &State_Machine, sizeof(State_Machine));
   this->vtbl = NULL;

}

void state_machine_dispatch(union State_Machine * const this, union Mail * const mail)
{
   STID_T current_st = this->current_st;
   STID_T next_stid = this->initial_st; 
   struct St_Machine_Transition transition =  {0, next_stid, NULL, NULL};

   if(STID_NOT_STATE != current_st)
   {
      uint8_t i;
      for(i = 0; i < this->st_chart[current_st].nhandlers; ++i)
      {
            if(mail->mid == this->st_chart[current_st].handle[i].mid)
            {
                  break;
            }

      }
      memcpy(&transition, this->st_chart[current_st].handle + i, sizeof(transition));
      next_stid = transition.stid;
   }
   else
   {
      Dbg_Info("Init State Machine on STID = %d", this->initial_st);
   }

   union St_Machine_State * next_st = this->vtbl->get_state(this, next_stid);

   if(NULL != next_st)
   {
      this->vtbl->transition_to(this, &transition, next_st);
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

   this->current_st = state->stid;
   Isnt_Nullptr(transition->action, true);
   transition->action(this);
   return true;
}

union St_Machine_State * state_machine_get_state(union State_Machine * const this, STID_T const stid)
{
   union St_Machine_State * state = NULL;
   Dbg_Fault("%s, abstract method, please override", __func__);

   if(stid < this->n_states)
   {
      state = this->st_chart + stid;
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
