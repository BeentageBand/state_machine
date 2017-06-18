/*=====================================================================================*/
/**
 * hama_hsm_types.h
 * author : puch
 * date : Oct 22 2015
 *
 * description : Any comments
 *
 */
/*=====================================================================================*/
#ifndef HAMA_HSM_TYPES_H_
#define HAMA_HSM_TYPES_H_
/*=====================================================================================*
 * Project Includes
 *=====================================================================================*/
#include "std_reuse.h"
/*=====================================================================================* 
 * Standard Includes
 *=====================================================================================*/

/*=====================================================================================* 
 * Exported Define Macros
 *=====================================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/*=====================================================================================* 
 * Exported Type Declarations
 *=====================================================================================*/
union Hama_HSM;
typedef uint16_t Hama_HSM_Signal_T;
typedef uint16_t Hama_HSM_State_T;
typedef void(*HSM_Transitions_Function_T)(union Hama_HSM * const machine);

typedef struct
{
   Hama_HSM_Signal_T signal;
   void * data;
   size_t data_size;
}Hama_HSM_Event_T;

typedef struct
{
   Hama_HSM_State_T source_state;
   Hama_HSM_Signal_T signal;
   bool (*guard)(union Hama_HSM * const machine);
   void (*transition)(union Hama_HSM * const machine);
   Hama_HSM_State_T target_state;
}Hama_HSM_Statechart_T;

typedef struct
{
   Hama_HSM_State_T state;
   Hama_HSM_State_T super;
   void (*entry)(union Hama_HSM * const machine);
   void (*exit)(union Hama_HSM * const machine);
}Hama_HSM_Handle_T;

#ifdef __cplusplus
}
#endif
/*=====================================================================================* 
 * hama_hsm_types.h
 *=====================================================================================*
 * Log History
 *
 *=====================================================================================*/
#endif /*HAMA_HSM_TYPES_H_*/
