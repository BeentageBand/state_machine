#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_
#include "state_machine_types.h"
#include "mail.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef union State_Machine
{
    struct State_Machine_Class _private * _private vtbl;
    struct
    {
        struct Object Object;
        union St_Machine_State _private * _private initial_st;
        union St_Machine_State _private * _private current_st;
    };
}State_Machine_T;

typedef struct State_Machine_Class
{
    struct Class Class;
    void (* _private dispatch)(union State_Machine * const, union Mail * const);
    bool (* _private transition_to)(union State_Machine * const, union St_Machine_State * const);
}State_Machine_Class_T;

extern State_Machine_Class_T _private State_Machine_Class;

extern void Populate_State_Machine(union State_Machine * const st_machine, union St_Machine_State * const initial_st);

#ifdef __cplusplus
}
#endif
#endif /*STATE_MACHINE_H_*/
