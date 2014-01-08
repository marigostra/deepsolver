
AM_CXXFLAGS = $(DEEPSOLVER_CXXFLAGS) $(DEEPSOLVER_INCLUDES)

bin_PROGRAMS = messages

messages_LDADD = \
$(top_srcdir)/lib/deepsolver/libdeepsolver.la
messages_DEPENDENCIES = $(messages_LDADD)
messages_SOURCES= messages.cpp
