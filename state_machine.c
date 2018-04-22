/*=====================================================================================*/
/**
 * hama_hsm.c
 * author : puch
 * date : Oct 22 2015
 *
 * description : Any comments
 *
 */
/*=====================================================================================*/
#define CLASS_IMPLEMENTATION
/*=====================================================================================*
 * Project Includes
 *=====================================================================================*/
#include "hama_hsm.h"
#include "hama_dbg_trace.h"
/*=====================================================================================* 
 * Standard Includes
 *=====================================================================================*/
#include <stdlib.h>
/*=====================================================================================* 
 * Local X-Macros
 *=====================================================================================*/

/*=====================================================================================* 
 * Local Define Macros
 *=====================================================================================*/
#define HSM_Name(name) _concat(Hama_HSM_,name)
#define HSM_Type(name) _concat(HSM_Name(name),_T)
#define HSM_Tb_Search(K, T) _concat(K,_concat(T,Tb_Search))
#define HSM_Tb_Search_Def(K, T) \
static uint32_t HSM_Tb_Search(K, T)(HSM_Type(K) const * key, HSM_Type(T) * table, \
uint32_t const size, HSM_Type(T) ** found, uint32_t i) \
{ \
   for(; i < size; ++i) \
   { \
      if(_concat(K,_concat(T,EqualsTo))(key, &table[i])) \
      { \
         *found = &table[i]; \
         break; \
      } \
   } \
   return i; \
} \
/*=====================================================================================* 
 * Local Type Definitions
 *=====================================================================================*/
typedef struct
{
   Hama_HSM_Signal_T signal;
   Hama_HSM_State_T state;
}Hama_HSM_Key_T;
/*=====================================================================================* 
 * Local Object Definitions
 *=====================================================================================*/
CLASS_DEFINITION
/*=====================================================================================* 
 * Exported Object Definitions
 *=====================================================================================*/

/*=====================================================================================* 
 * Local Function Prototypes
 *=====================================================================================*/
static void Hama_HSM_ctor(Hama_HSM_T * const this, Hama_HSM_State_T const initial_state,
      Hama_HSM_Handle_T * const statehandler, uint32_t const sizeof_statehandler,
      Hama_HSM_Statechart_T * const statechart, uint32_t const sizeof_statechart);

static void On_Entry(Hama_HSM_T * const this, Hama_HSM_State_T const target);

static void On_Exit(Hama_HSM_T * const this);

static Hama_HSM_State_T On_Handle(Hama_HSM_T * const this, Hama_HSM_Signal_T const signal);
/*=====================================================================================* 
 * Local Inline-Function Like Macros
 *=====================================================================================*/
static bool_t KeyStatechartEqualsTo(Hama_HSM_Key_T const * const a, Hama_HSM_Statechart_T const * const b)
{
   TR_INFO_5("%s, key state %d == statechart state %d && "
         "key signal %d == statechart signal %d ", __FUNCTION__,
         a->state, b->source_state, a->signal, b->signal);
   return (a->signal == b->signal) && (a->state == b->source_state);
}

static bool_t StateHandleEqualsTo(Hama_HSM_State_T const * const a, Hama_HSM_Handle_T const * const b)
{
   TR_INFO_3("%s, state %d == handle %d", __FUNCTION__, *a, b->state);
   return *a == b->state;
}
/*=====================================================================================* 
 * Local Function Definitions
 *=====================================================================================*/
void Hama_HSM_init(void)
{
   printf("%s \n", __FUNCTION__);
   Hama_HSM_Vtbl.Object.rtti = &Hama_HSM_Rtti;
   Hama_HSM_Vtbl.Object.destroy = Hama_HSM_Dtor;
   Hama_HSM_Vtbl.dispatch = NULL;
   Hama_HSM_Vtbl.ctor = Hama_HSM_ctor;

}
void Hama_HSM_shut(void) {}

void Hama_HSM_Dtor(Object_T * const obj)
{
}

HSM_Tb_Search_Def(Key,Statechart)

HSM_Tb_Search_Def(State,Handle)

void On_Entry(Hama_HSM_T * const this, Hama_HSM_State_T const target)
{
   uint32_t state_num = 0;
   Hama_HSM_State_T super_state = target;
   Hama_HSM_State_T prev_state = this->size_statehandler; // top state's super state
   Hama_HSM_Handle_T * handle = NULL;
   TR_INFO_1("%s", __FUNCTION__);
   // Find top state
   do
   {
      super_state = target;
      do
      {
         if(NULL != handle)
         {
            super_state = handle->super;
            TR_INFO_1("state = %d", super_state);
         }
         state_num = HSM_Tb_Search(State, Handle)(&super_state,
               this->statehandler, this->size_statehandler, &handle,state_num);
      }
      while(NULL != handle &&
            prev_state != handle->super);

      if(NULL != handle && NULL != handle->entry)
      {
         handle->entry(this);
      }
      prev_state = super_state;
   }
   while(super_state != target);
}

void On_Exit(Hama_HSM_T * const this)
{
   uint32_t state_num = 0;
   Hama_HSM_State_T super_state = this->current_state;
   Hama_HSM_Handle_T * handle = NULL;
   TR_INFO_1("%s", __FUNCTION__);
   do
   {
      state_num = HSM_Tb_Search(State, Handle)(&super_state,
            this->statehandler, this->size_statehandler, &handle, state_num);
      if(NULL != handle && handle->exit)
      {
         handle->exit(this);
         super_state = handle->super;
      }
   }
   while(NULL != handle &&
         super_state < this->size_statehandler);
}

Hama_HSM_State_T On_Guard(Hama_HSM_T  * const this, Hama_HSM_Signal_T const signal,
      Hama_HSM_Handle_T * const handle)
{
   Hama_HSM_State_T target = this->size_statehandler;
   uint32_t statechart_num = 0;
   Hama_HSM_Statechart_T * pseudostate = NULL;
   Hama_HSM_Key_T key = { signal, handle->state };
   TR_INFO_1("%s", __FUNCTION__);

   while(statechart_num < this->size_statechart)
   {
      statechart_num = HSM_Tb_Search(Key, Statechart)(&key,
                  &this->statechart[statechart_num],
                  this->size_statechart,
                  &pseudostate, statechart_num);

      if(statechart_num < this->size_statechart)
      {
         TR_INFO_3("source %d, signal %d, idx %d", pseudostate->source_state,
               pseudostate->signal, statechart_num);
         if(NULL == pseudostate->guard || pseudostate->guard(this))
         {
            this->transitions[this->transition_idx++] = pseudostate->transition;
            ++statechart_num;
            target = pseudostate->target_state;
         }
         else
         {
            break;
         }
      }
   }
   return target;
}

Hama_HSM_State_T On_Handle(Hama_HSM_T * const this, Hama_HSM_Signal_T const signal)
{
   uint32_t state_num = 0;
   Hama_HSM_State_T target = this->size_statehandler;
   Hama_HSM_State_T super_state = this->current_state;
   Hama_HSM_Handle_T * handle = NULL;
   TR_INFO_1("%s", __FUNCTION__);
   this->transition_idx = 0;
   do
   {
      state_num = HSM_Tb_Search(State, Handle)(&super_state,
            this->statehandler, this->size_statehandler, &handle, state_num);

      if(NULL != handle) //State exists!
      {
         TR_INFO_2("handler state %d, signal %d", handle->state, signal);
         target = On_Guard(this, signal, handle);

         if(target < this->size_statehandler)
         {
            // Transition is done, target is found
            break;
         }
         else
         {
            //No transition is found, move to parent_state handle
            super_state = handle->super;
         }
      }
   }
   while(NULL != handle &&
         super_state < this->size_statehandler);
   return target;
}

void On_Transition(Hama_HSM_T * const machine)
{
   TR_INFO_1("%s", __FUNCTION__);
   void (**transition)(Hama_HSM_T * const machine) = machine->transitions;
   while(machine->transition_idx--)
   {
      if(NULL != *transition)
      {
         (*transition)(machine);
      }
      ++transition;
   }
}
/*=====================================================================================* 
 * Exported Function Definitions
 *=====================================================================================*/
void Hama_HSM_ctor(Hama_HSM_T * const this, Hama_HSM_State_T const initial_state,
      Hama_HSM_Handle_T * const statehandler, uint32_t const sizeof_statehandler,
      Hama_HSM_Statechart_T * const statechart, uint32_t const sizeof_statechart)
{
   this->current_state = initial_state;
   this->statehandler = statehandler;
   this->size_statehandler = sizeof_statehandler;
   this->statechart = statechart;
   this->size_statechart = sizeof_statechart;
   this->transition_idx = 0;

   On_Entry(this, initial_state);

   TR_INFO_1("%s Machine Definition :", __FUNCTION__);
   TR_INFO_2("current state - %d, total states - %d ", this->current_state, this->size_statehandler);

   for(uint8_t i = 0; i < this->size_statehandler; ++i)
   {   }
}

void Hama_HSM_dispatch(Hama_HSM_T * const machine, Hama_HSM_Event_T const * const hsm_ev)
{
   TR_INFO_2("Dispatch on current %d, signal %d",
         machine->current_state, hsm_ev->signal);
   //Find current state handler
   if(machine->current_state < machine->size_statehandler)
   {
      Hama_HSM_State_T target_state = On_Handle(machine, hsm_ev->signal);

      if(target_state < machine->size_statehandler)
      {
         TR_INFO_1("Transition to state %d", target_state);
         On_Exit(machine);
         On_Transition(machine);
         On_Entry(machine, target_state);
         machine->current_state = target_state;
      }
      else
      {
         TR_INFO("No transition to state");
      }
   }
}
/*=====================================================================================* 
 * hama_hsm.c
 *=====================================================================================*
 * Log History
 *
 *=====================================================================================*/
