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

/*=====================================================================================* 
 * Local Object Definitions
 *=====================================================================================*/

/*=====================================================================================* 
 * Exported Object Definitions
 *=====================================================================================*/

/*=====================================================================================* 
 * Local Function Prototypes
 *=====================================================================================*/
template <typename T, bool (*EqualsTo) (uint8_t const, T const &) >
static T * HSM_Tb_Search(uint8_t const key, T * table, uint32_t const size);

static void On_Entry(Hama_HSM_T& machine, Hama_HSM_State_T const target);

static void On_Exit(Hama_HSM_T& machine);

static Hama_HSM_State_T On_Handle(Hama_HSM_T& machine, Hama_HSM_Signal_T const signal);
/*=====================================================================================* 
 * Local Inline-Function Like Macros
 *=====================================================================================*/
inline bool HSM_Statechart_Equals_To(uint8_t const a, Hama_HSM_Statechart_T const & b)
{
   return a == b.signal;
}

inline bool HSM_Handle_Equals_To(uint8_t const a, Hama_HSM_Handle_T const & b)
{
   return a == b.state;
}
/*=====================================================================================* 
 * Local Function Definitions
 *=====================================================================================*/
template <typename T, bool (*EqualsTo) (uint8_t const, T const &) >
T * HSM_Tb_Search(uint8_t const key, T * table, uint32_t const size)
{
   T * found = NULL;

   for(uint32_t i = 0; i < size; ++i)
   {
      if(EqualsTo(key, table[i]))
      {
         found = &table[i];
         break;
      }
   }

   return found;
}

void On_Entry(Hama_HSM_T& machine,Hama_HSM_State_T const target)
{
   Hama_HSM_State_T super_state = target;
   Hama_HSM_State_T prev_state = machine.sizeof_statehandler; // top state's super state
   Hama_HSM_Handle_T const * handle = NULL;
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
         }
         handle = HSM_Tb_Search<Hama_HSM_Handle_T,HSM_Handle_Equals_To>(super_state,
               machine.statehandler, machine.sizeof_statehandler);
      }
      while(NULL != handle &&
            prev_state != handle->super);

      handle->entry(machine);
      prev_state = super_state;
   }
   while(super_state != target);
}

void On_Exit(Hama_HSM_T& machine)
{
   Hama_HSM_State_T super_state = machine.current_state;
   Hama_HSM_Handle_T const * handle = NULL;
   TR_INFO_1("%s", __FUNCTION__);
   do
   {
      handle = HSM_Tb_Search<Hama_HSM_Handle_T, HSM_Handle_Equals_To>(super_state,
            machine.statehandler, machine.sizeof_statehandler);
      handle->exit(machine);
      super_state = handle->super;
   }
   while(NULL != handle &&
         super_state < machine.sizeof_statehandler);
}

Hama_HSM_State_T On_Handle(Hama_HSM_T& machine, Hama_HSM_Signal_T const signal)
{
   Hama_HSM_State_T target = machine.sizeof_statehandler;
   Hama_HSM_State_T super_state = machine.current_state;
   Hama_HSM_Handle_T const * handle = NULL;
   TR_INFO_1("%s", __FUNCTION__);
   do
   {
      handle = HSM_Tb_Search<Hama_HSM_Handle_T, HSM_Handle_Equals_To>(super_state,
            machine.statehandler, machine.sizeof_statehandler);

      if(NULL != handle)
      {
         Hama_HSM_Statechart_T const * statechart =
               HSM_Tb_Search<Hama_HSM_Statechart_T, HSM_Statechart_Equals_To>(signal,
                     handle->statechart, handle->sizeof_statechart);

         if(NULL != statechart)
         {
            if(statechart->handle(machine))
            {
               target = statechart->next_state;
            }
            break;
         }
         else
         {
            super_state = handle->super;
         }
      }
   }
   while(NULL != handle &&
         super_state < machine.sizeof_statehandler);
   return target;
}
/*=====================================================================================* 
 * Exported Function Definitions
 *=====================================================================================*/
void hama::HSM_Init(Hama_HSM_State_T const initial_state, Hama_HSM_T& machine, Hama_HSM_Handle_T * statehandler, uint32_t const sizeof_statehandler)
{
   machine.current_state = initial_state;
   machine.statehandler = statehandler;
   machine.sizeof_statehandler = sizeof_statehandler;

   On_Entry(machine, initial_state);

   TR_INFO_1("%s Machine Definition :", __FUNCTION__);
   TR_INFO_2("current state - %d, total states - %d ", machine.current_state, machine.sizeof_statehandler);

   for(uint8_t i = 0; i < machine.sizeof_statehandler; ++i)
   {
      if( NULL != machine.statehandler)
      {
          TR_INFO_3("State - %d, Super state-%d, total state transitions - %d",
                machine.statehandler[i].state, machine.statehandler[i].super, machine.statehandler[i].sizeof_statechart);

          for(uint8_t j = 0; j < machine.statehandler[i].sizeof_statechart; ++j)
          {
             if(NULL != machine.statehandler[i].statechart)
             {
                TR_INFO_2("Next State - %d, Trigger Signal - %d",
                      machine.statehandler[i].statechart[j].next_state,
                      machine.statehandler[i].statechart[j].signal);
             }
             else
             {
                break;
             }
          }
      }
      else
      {
         break;
      }
   }
}

void hama::HSM_Dispatch(Hama_HSM_T& machine, Hama_HSM_Event_T const & hsm_ev)
{
   //Find current state handler
   if(machine.current_state < machine.sizeof_statehandler)
   {
      Hama_HSM_State_T target_state = On_Handle(machine, hsm_ev.signal);
      if(target_state < machine.sizeof_statehandler)
      {
         On_Exit(machine);
         On_Entry(machine, target_state);
         machine.current_state = target_state;
      }
   }


}
/*=====================================================================================* 
 * hama_hsm.cpp
 *=====================================================================================*
 * Log History
 *
 *=====================================================================================*/
