dist_check_SCRIPTS = run.sh
TESTS = $(dist_check_SCRIPTS)
EXTRA_DIST = $(dist_check_SCRIPTS) ieee1609-contributed-extension.asn1
CLEANFILES = *.c *.h *.o Makefile.am.* converter-example.mk pdu_collection.c asn_constant.h

TESTS_ENVIRONMENT = \
    top_builddir='$(top_builddir)' \
    srcdir='$(srcdir)' \
    builddir='$(builddir)'

clean-local:
	-rm -f ieee1609-contributed-extension.asn1
