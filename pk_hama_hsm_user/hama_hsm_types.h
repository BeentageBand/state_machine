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
#include <stdlib.h>
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
   void * data;
   size_t data_size;
}Hama_HSM_Event_T;

struct Hama_HSM_T;

struct Hama_HSM_Statechart_T
{
   Hama_HSM_State_T source_state;
   Hama_HSM_Signal_T signal;
   bool (*guard)(Hama_HSM_T & machine);
   void (*transition)(Hama_HSM_T & machine);
   Hama_HSM_State_T target_state;
};

struct Hama_HSM_Handle_T
{
   Hama_HSM_State_T state;
   Hama_HSM_State_T super;
   void (*entry)(Hama_HSM_T & machine);
   void (*exit)(Hama_HSM_T & machine);
};

struct Hama_HSM_T
{
   Hama_HSM_State_T current_state;
   Hama_HSM_Handle_T * statehandler;
   Hama_HSM_Statechart_T * statechart;
   uint32_t size_statehandler;
   uint32_t size_statechart;
   void(*transitions[8])(Hama_HSM_T & machine);
   uint8_t transition_idx;
};
/*=====================================================================================* 
 * hama_hsm_types.h
 *=====================================================================================*
 * Log History
 *
 *=====================================================================================*/
#endif /*HAMA_HSM_TYPES_H_*/
