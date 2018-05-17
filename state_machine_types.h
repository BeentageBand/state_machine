#ifndef STATE_MACHINE_TYPES_H_
#define STATE_MACHINE_TYPES_H_

#include "cobject.h"
#include "ipc_types.h"

#define STID_NOT_FOUND UINT32_T_MAX
#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t STID_T;

/*
 * Foward Declaration
 */
union State_Machine;
union St_Machine_State;

struct St_Machine_Transition
{
   IPC_MID_T mid;
   STID_T stid;
   bool (* guard)(union State_Machine * const, union St_Machine_State * const);
   void (* action) (union State_Machine * const);
};

#ifdef __cplusplus
}
#endif
#endif /*STATE_MACHINE_TYPES_H_*/
