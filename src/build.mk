# Standard behavior must be included here
INCLUDE_DIRS += $(SOURCE_PATH)/$(USRSRC)  # add user sources to include path
CPPSRC += $(call target_files,$(USRSRC_SLASH),*.cpp)
CSRC += $(call target_files,$(USRSRC_SLASH),*.c)

CPPFLAGS += -lc -specs=nosys.specs
CPPFLAGS += -std=gnu++14

APPSOURCES=$(call target_files,$(USRSRC_SLASH),*.cpp)
APPSOURCES+=$(call target_files,$(USRSRC_SLASH),*.c)

# Custom stuff can be added here
INCLUDE_DIRS += $(SOURCE_PATH)/../boost_1_66_0
