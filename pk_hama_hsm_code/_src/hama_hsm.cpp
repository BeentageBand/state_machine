/*=====================================================================================*/
/**
 * hama_hsm.cpp
 * author : puch
 * date : Oct 22 2015
 *
 * description : Any comments
 *
 */
/*=====================================================================================*/

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

/*=====================================================================================* 
 * Local Type Definitions
 *=====================================================================================*/
struct Statechart_Key_T
{
   Hama_HSM_Signal_T signal;
   Hama_HSM_State_T state;
};
/*=====================================================================================* 
 * Local Object Definitions
 *=====================================================================================*/

/*=====================================================================================* 
 * Exported Object Definitions
 *=====================================================================================*/

/*=====================================================================================* 
 * Local Function Prototypes
 *=====================================================================================*/
template <typename K, typename T, bool (*EqualsTo)(K const &, T const &)>
static uint32_t HSM_Tb_Search(K const & key, T * table, uint32_t const size, T * found);

static void On_Entry(Hama_HSM_T& machine, Hama_HSM_State_T const target);

static void On_Exit(Hama_HSM_T& machine);

static Hama_HSM_State_T On_Handle(Hama_HSM_T& machine, Hama_HSM_Signal_T const signal);
/*=====================================================================================* 
 * Local Inline-Function Like Macros
 *=====================================================================================*/
inline bool HSM_Statechart_Equals_To(Statechart_Key_T const & a, Hama_HSM_Statechart_T const & b)
{
   return (a.signal == b.signal) && (a.state == b.source_state);
}

inline bool HSM_Handle_Equals_To(Hama_HSM_State_T const & a, Hama_HSM_Handle_T const & b)
{
   return a == b.state;
}
/*=====================================================================================* 
 * Local Function Definitions
 *=====================================================================================*/
template <typename K, typename T, bool (*EqualsTo) (K const &, T const &) >
uint32_t HSM_Tb_Search(K const & key, T * table, uint32_t const size, T * found)
{
   uint32_t i = 0;
   for(; i < size; ++i)
   {
      if(EqualsTo(key, table[i]))
      {
         found = &table[i];
         break;
      }
   }
   return i;
}

void On_Entry(Hama_HSM_T& machine,Hama_HSM_State_T const target)
{
   Hama_HSM_State_T super_state = target;
   Hama_HSM_State_T prev_state = machine.size_statehandler; // top state's super state
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
         (void) HSM_Tb_Search<Hama_HSM_State_T, Hama_HSM_Handle_T,
               HSM_Handle_Equals_To>(super_state,
               machine.statehandler, machine.size_statehandler, handle);
      }
      while(NULL != handle &&
            prev_state != handle->super);

      if(NULL != handle && NULL != handle->entry)
      {
         handle->entry(machine);
      }
      prev_state = super_state;
   }
   while(super_state != target);
}

void On_Exit(Hama_HSM_T& machine)
{
   Hama_HSM_State_T super_state = machine.current_state;
   Hama_HSM_Handle_T * handle = NULL;
   TR_INFO_1("%s", __FUNCTION__);
   do
   {
      (void)HSM_Tb_Search<Hama_HSM_State_T, Hama_HSM_Handle_T,
            HSM_Handle_Equals_To>(super_state,
            machine.statehandler, machine.size_statehandler, handle);
      if(NULL != handle && handle->exit)
      {
         handle->exit(machine);
         super_state = handle->super;
      }
   }
   while(NULL != handle &&
         super_state < machine.size_statehandler);
}

Hama_HSM_State_T On_Guard(Hama_HSM_T & machine, Hama_HSM_Signal_T const signal,
      Hama_HSM_Handle_T * handle)
{
   Hama_HSM_State_T target = machine.size_statehandler;
   uint32_t statechart_num = 0;
   Hama_HSM_Statechart_T * pseudostate = NULL;
   Statechart_Key_T key = { signal, handle->state };
   TR_INFO_1("%s", __FUNCTION__);

   while(statechart_num < machine.size_statechart)
   {
      statechart_num = HSM_Tb_Search<Statechart_Key_T, Hama_HSM_Statechart_T,
            HSM_Statechart_Equals_To>(key,
            machine.statechart, machine.size_statechart, pseudostate);

      if(NULL != pseudostate)
      {
         if(NULL == pseudostate->guard || pseudostate->guard(machine))
         {
            machine.transitions[machine.transition_idx++] = pseudostate->transition;
         }
         else
         {
            break;
         }
      }
   }
   return target;
}

Hama_HSM_State_T On_Handle(Hama_HSM_T& machine, Hama_HSM_Signal_T const signal)
{
   Hama_HSM_State_T target = machine.size_statehandler;
   Hama_HSM_State_T super_state = machine.current_state;
   Hama_HSM_Handle_T * handle = NULL;
   TR_INFO_1("%s", __FUNCTION__);
   machine.transition_idx = 0;
   do
   {
      (void)HSM_Tb_Search<Hama_HSM_State_T, Hama_HSM_Handle_T,
            HSM_Handle_Equals_To>(super_state,
            machine.statehandler, machine.size_statehandler, handle);

      if(NULL != handle) //State exists!
      {
         target = On_Guard(machine, signal, handle);

         if(target < machine.size_statehandler)
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
         super_state < machine.size_statehandler);
   return target;
}

void On_Transition(Hama_HSM_T & machine)
{
   TR_INFO_1("%s", __FUNCTION__);
   void (**transition)(Hama_HSM_T & machine) = machine.transitions;
   while(machine.transition_idx--)
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
void hama::HSM_Init(Hama_HSM_State_T const initial_state, Hama_HSM_T& machine,
      Hama_HSM_Handle_T * statehandler, uint32_t const sizeof_statehandler,
      Hama_HSM_Statechart_T * statechart, uint32_t const sizeof_statechart)
{
   machine.current_state = initial_state;
   machine.statehandler = statehandler;
   machine.size_statehandler = sizeof_statehandler;
   machine.statechart = statechart;
   machine.size_statechart = sizeof_statechart;
   machine.transition_idx = 0;

   On_Entry(machine, initial_state);

   TR_INFO_1("%s Machine Definition :", __FUNCTION__);
   TR_INFO_2("current state - %d, total states - %d ", machine.current_state, machine.size_statehandler);

   for(uint8_t i = 0; i < machine.size_statehandler; ++i)
   {   }
}

void hama::HSM_Dispatch(Hama_HSM_T& machine, Hama_HSM_Event_T const & hsm_ev)
{
   TR_INFO_2("Dispatch on current %d, signal %d",
         machine.current_state, hsm_ev.signal);
   //Find current state handler
   if(machine.current_state < machine.size_statehandler)
   {
      Hama_HSM_State_T target_state = On_Handle(machine, hsm_ev.signal);

      if(target_state < machine.size_statehandler)
      {
         TR_INFO_1("Transition to state %d", target_state);
         On_Exit(machine);
         On_Transition(machine);
         On_Entry(machine, target_state);
         machine.current_state = target_state;
      }
      else
      {
         TR_INFO("No transition to state");
      }
   }
}
/*=====================================================================================* 
 * hama_hsm.cpp
 *=====================================================================================*
 * Log History
 *
 *=====================================================================================*/
