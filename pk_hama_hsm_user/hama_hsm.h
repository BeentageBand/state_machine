/*=====================================================================================*/
/**
 * hama_hsm.h
 * author : puch
 * date : Oct 22 2015
 *
 * description : Any comments
 *
 */
/*=====================================================================================*/
#ifndef HAMA_HSM_H_
#define HAMA_HSM_H_
/*=====================================================================================*
 * Project Includes
 *=====================================================================================*/
#include "hama_hsm_types.h"
/*=====================================================================================* 
 * Standard Includes
 *=====================================================================================*/

/*=====================================================================================* 
 * Exported X-Macros
 *=====================================================================================*/
namespace hama
{
/*=====================================================================================* 
 * Exported Define Macros
 *=====================================================================================*/
#define HSM_STATE_MACHINE_DEF(_name, _macro) \
   _macro(_name, HSM_STATE_HANDLER_DEF, HSM_STATECHART_DEF)           \
   static Hama_HSM_Handle_T HSM_##_name [] =                          \
   {                                                                  \
         _macro(_name, HSM_STATE_HANDLER_INDEX, HSM_STATECHART_INDEX) \
   }                                                                  \


#define HSM_STATE_HANDLER_DEF(_name, _state, _super, _entry, _exit, _statechart) \
   static Hama_HSM_Statechart_T HSM_Statechart_##_name##_##_state [] =                    \
   {                                                                                      \
      _statechart                                                                         \
   };                                                                                     \

#define HSM_STATECHART_DEF(_next_state, _signal, _handle) \
{                                                \
   _signal,                                      \
   _next_state,                                  \
   _handle                                       \
},                                               \

#define HSM_STATE_HANDLER_INDEX(_name, _state, _super, _entry, _exit, _statechart) \
{                                                                                           \
   _state,                                                                                  \
   _super,                                                                                  \
   HSM_Statechart_##_name##_##_state,                                                       \
   Num_Elements(HSM_Statechart_##_name##_##_state),                                         \
   _entry,                                                                                  \
   _exit,                                                                                   \
},                                                                                          \

#define HSM_STATECHART_INDEX(_next_state, _signal, _handle) \

/*=====================================================================================* 
 * Exported Type Declarations
 *=====================================================================================*/

/*=====================================================================================* 
 * Exported Object Declarations
 *=====================================================================================*/

/*=====================================================================================* 
 * Exported Function Prototypes
 *=====================================================================================*/
extern void HSM_Init(Hama_HSM_State_T const initial_state, Hama_HSM_T& machine,
      Hama_HSM_Handle_T * statehandler, uint32_t const sizeof_statehandler);

extern void HSM_Dispatch(Hama_HSM_T& machine, Hama_HSM_Event_T const & hsm_ev);

/*=====================================================================================* 
 * Exported Function Like Macros
 *=====================================================================================*/

}
/*=====================================================================================* 
 * hama_hsm.h
 *=====================================================================================*
 * Log History
 *
 *=====================================================================================*/
#endif /*HAMA_HSM_H_*/
