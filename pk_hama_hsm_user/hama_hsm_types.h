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

/*=====================================================================================* 
 * Standard Includes
 *=====================================================================================*/
#include <stdint.h>
/*=====================================================================================* 
 * Exported Define Macros
 *=====================================================================================*/
#define Num_Elements(array) static_cast<uint32_t>(sizeof(array)/sizeof(array[0]))
/*=====================================================================================* 
 * Exported Type Declarations
 *=====================================================================================*/
typedef uint16_t Hama_HSM_Signal_T;
typedef uint16_t Hama_HSM_State_T;

typedef struct
{
   Hama_HSM_Signal_T signal;
}Hama_HSM_Event_T;

struct Hama_HSM_T;

typedef struct
{
   Hama_HSM_Signal_T signal;
   Hama_HSM_State_T next_state;
   bool (*handle)(Hama_HSM_T & machine);
}Hama_HSM_Statechart_T;

typedef struct
{
   Hama_HSM_State_T state;
   Hama_HSM_State_T super;
   Hama_HSM_Statechart_T * statechart;
   uint32_t sizeof_statechart;
   void (*entry)(Hama_HSM_T & machine);
   void (*exit)(Hama_HSM_T & machine);
}Hama_HSM_Handle_T;

struct Hama_HSM_T
{
   Hama_HSM_State_T current_state;
   Hama_HSM_Handle_T * statehandler;
   uint32_t sizeof_statehandler;
};
/*=====================================================================================* 
 * hama_hsm_types.h
 *=====================================================================================*
 * Log History
 *
 *=====================================================================================*/
#endif /*HAMA_HSM_TYPES_H_*/
