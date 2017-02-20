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

static void On_Entry(Hama_HSM_T& machine,Hama_HSM_Handle_T & handle);

static void On_Exit(Hama_HSM_T& machine, Hama_HSM_Handle_T & handle);

static void On_Handle(Hama_HSM_T& machine, Hama_HSM_Handle_T & handle);
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

void On_Entry(Hama_HSM_T& machine,Hama_HSM_Handle_T & handle)
{
   Hama_HSM_Handle_T * found_handle =
         HSM_Tb_Search<Hama_HSM_Handle_T, HSM_Handle_Equals_To>( handle.super,
            machine.statehandler, machine.sizeof_statehandler);
   TR_INFO_2("%s state -%d ", __FUNCTION__, handle.state);
   if(NULL != found_handle)
   {
      On_Entry(machine, *found_handle);
   }

   handle.entry(machine);
}
void On_Exit(Hama_HSM_T& machine, Hama_HSM_Handle_T & handle)
{
   handle.exit(machine);
   Hama_HSM_Handle_T * found_handle =
         HSM_Tb_Search<Hama_HSM_Handle_T, HSM_Handle_Equals_To>( handle.super,
               machine.statehandler, machine.sizeof_statehandler);

   TR_INFO_2("%s state -%d ", __FUNCTION__, handle.state);
   if(NULL != found_handle)
   {
      On_Exit(machine, *found_handle);
   }
}
void On_Handle(Hama_HSM_T& machine, Hama_HSM_Handle_T & handle)
{
   if(NULL != handle.handle)
   {
      handle.handle(machine);
   }
   else
   {
      Hama_HSM_Handle_T * found_handle =
            HSM_Tb_Search<Hama_HSM_Handle_T, HSM_Handle_Equals_To>( handle.super,
                  machine.statehandler, machine.sizeof_statehandler);
      TR_INFO_2("%s state -%d ", __FUNCTION__, handle.state);
      if(NULL != found_handle)
      {
         On_Handle(machine, *found_handle);
      }
   }
}
/*=====================================================================================* 
 * Exported Function Definitions
 *=====================================================================================*/
void hama::HSM_Init(Hama_HSM_T& machine, Hama_HSM_Handle_T * statehandler, uint32_t const sizeof_statehandler)
{
   machine.current_state = 0;
   machine.statehandler = statehandler;
   machine.sizeof_statehandler = sizeof_statehandler;

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
   Hama_HSM_Handle_T * found_handle =
         HSM_Tb_Search<Hama_HSM_Handle_T, HSM_Handle_Equals_To>( machine.current_state,
               machine.statehandler, machine.sizeof_statehandler);

   TR_INFO_1("handle state %d", machine.current_state);

   if(NULL != found_handle)
   {
      Hama_HSM_Statechart_T * found_statechart =
            HSM_Tb_Search<Hama_HSM_Statechart_T, HSM_Statechart_Equals_To>( hsm_ev.signal,
                  found_handle->statechart, found_handle->sizeof_statechart);

      TR_INFO_1("state chart signal - %d", hsm_ev.signal);
      if(NULL != found_statechart)
      {
         Hama_HSM_Handle_T * found_next_handle =
               HSM_Tb_Search<Hama_HSM_Handle_T, HSM_Handle_Equals_To>( found_statechart->next_state,
                     machine.statehandler, machine.sizeof_statehandler);

         TR_INFO_1("state transition - %d", found_statechart->next_state);
         if(NULL != found_next_handle)
         {
            On_Exit(machine, *found_handle);
            On_Entry(machine, *found_next_handle);
            machine.current_state = found_next_handle->state;
         }
         else
         {
            TR_INFO("NULL == found_next_handle");
         }
      }
      else
      {
         TR_INFO("NULL == found_statechart");
      }
   }
   else
   {
      TR_INFO("NULL == found_handle");
   }
}
/*=====================================================================================* 
 * hama_hsm.cpp
 *=====================================================================================*
 * Log History
 *
 *=====================================================================================*/
