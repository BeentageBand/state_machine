
define $(_flavor_)_$(_feat_)_MAKE

$(_flavor_)_$(_feat_)_inc=\
fsm.h \
hsm.h \
state_machine.h \
state_machine_types.h \

$(_flavor_)_$(_feat_)_lib_objs=\
fsm \
hsm \
state_machine \

$(_flavor_)_$(_feat_)_lib=state_machine

endef

include $(PROJ_MAK_DIR)/epilog.mk