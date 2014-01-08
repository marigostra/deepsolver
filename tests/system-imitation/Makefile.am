
AM_CXXFLAGS = $(DEEPSOLVER_CXXFLAGS) $(DEEPSOLVER_INCLUDES)

bin_PROGRAMS = system-imitation

system_imitation_LDADD = \
$(top_srcdir)/lib/deepsolver/libdeepsolver.la
system_imitation_DEPENDENCIES = $(system_imitation_LDADD)
system_imitation_SOURCES= system-imitation.cpp
