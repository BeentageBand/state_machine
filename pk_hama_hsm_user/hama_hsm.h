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
#include "object.h"
/*=====================================================================================* 
 * Standard Includes
 *=====================================================================================*/

/*=====================================================================================* 
 * Exported X-Macros
 *=====================================================================================*/

/*=====================================================================================* 
 * Exported Define Macros
 *=====================================================================================*/
#undef CLASS_NAME
#undef CLASS_INHERITS
#undef CLASS_MEMBERS
#undef CLASS_METHODS

#define CLASS_NAME Hama_HSM
#define CLASS_INHERITS Object
#define CLASS_MEMBERS(_member) \
   _member(Hama_HSM_State_T _private, current_state)\
   _member(Hama_HSM_Handle_T * _private, statehandler) \
   _member(Hama_HSM_Statechart_T * _private, statechart) \
   _member(uint32_t _private, size_statehandler) \
   _member(uint32_t _private, size_statechart) \
   _member(HSM_Transitions_Function_T _private, transitions[8] ) \
   _member(uint8_t _private, transition_idx)

#define CLASS_METHODS(_method, _void_method) \
void _method(ctor, Hama_HSM_State_T const, Hama_HSM_Handle_T *, \
uint32_t const, Hama_HSM_Statechart_T * const, uint32_t const) \
void _method(dispatch, Hama_HSM_Event_T * const)

#ifdef __cplusplus
extern "C" {
#endif
/*=====================================================================================* 
 * Exported Type Declarations
 *=====================================================================================*/
CLASS_DECLARATION
/*=====================================================================================* 
 * Exported Object Declarations
 *=====================================================================================*/

/*=====================================================================================* 
 * Exported Function Prototypes
 *=====================================================================================*/

/*=====================================================================================* 
 * Exported Function Like Macros
 *=====================================================================================*/
#define HSM_STATE_MACHINE_DEF(_name, _handle, _statechart)             \
   static Hama_HSM_Handle_T HSM_Handle_##_name [] =                    \
   {                                                                   \
       _handle(HSM_STATE_HANDLER_DEF)                                  \
   };                                                                  \
   static Hama_HSM_Statechart_T HSM_Statechart_##_name [] =            \
   {                                                                   \
      _statechart(HSM_STATECHART_DEF)                                  \
   };                                                                  \

#define HSM_STATECHART_DEF(_signal, _source, _guard, _handle, _target) \
{                                                                      \
   _source,                                                            \
   _signal,                                                            \
   _guard,                                                             \
   _handle,                                                            \
   _target                                                             \
},                                                                     \

#define HSM_STATE_HANDLER_DEF( _state, _super, _entry, _exit) \
{                                                             \
   _state,                                                    \
   _super,                                                    \
   _entry,                                                    \
   _exit                                                      \
},                                                            \

#ifdef __cplusplus
}
#endif
/*=====================================================================================* 
 * hama_hsm.h
 *=====================================================================================*
 * Log History
 *
 *=====================================================================================*/
#endif /*HAMA_HSM_H_*/
