#add maze library
lib_LIBRARIES=$(top_builddir)/lib/libmaze.a
__top_builddir__lib_libmaze_a_SOURCES=

#build git_info.hpp
%D%/git_info.hpp: $(top_srcdir)/.git/HEAD $(top_srcdir)/.git/index $(top_srcdir)/config/get_git_info.sh Makefile
	@echo "Reading git version"
	@$(top_srcdir)/config/get_git_info.sh $(top_srcdir) > $@
$(top_srcdir)/.git/HEAD:
$(top_srcdir)/.git/index:

BUILT_SOURCES+=%D%/git_info.hpp

include $(top_srcdir)/src/base/Makefile.am
include $(top_srcdir)/src/debug/Makefile.am
include $(top_srcdir)/src/grammar/Makefile.am
include $(top_srcdir)/src/resources/Makefile.am
include $(top_srcdir)/src/threads/Makefile.am

