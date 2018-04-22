#ifndef STATE_MACHINE_TYPES_H_
#define STATE_MACHINE_TYPES_H_

#include "cobject.h"
#include "ipc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef union St_Machine_State * St_Machine_State_Ptr_T;
#define CMap_Params IPC_MID_T, St_Machine_State_Ptr_T
#include "cmap.h"
#undef CMap_Params

union St_Machine_State
{
    struct St_Machine_State_Class _private * _private vtbl;
    struct
    {
        struct Object Object;
        St_Machine_Statechart_T _private statechart;
    };
}St_Machine_State_T;
#ifdef __cplusplus
}
#endif
#endif /*STATE_MACHINE_TYPES_H_*/
