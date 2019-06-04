#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_ADD_INCLUDEDIRS=.
COMPONENT_SRCDIRS += app
COMPONENT_SRCDIRS += app/launch
COMPONENT_SRCDIRS += app/sms
COMPONENT_SRCDIRS += device
COMPONENT_SRCDIRS += external
COMPONENT_SRCDIRS += font
COMPONENT_SRCDIRS += icon
COMPONENT_SRCDIRS += image
COMPONENT_SRCDIRS += ui
COMPONENT_SRCDIRS += watcher