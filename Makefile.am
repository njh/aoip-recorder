AUTOMAKE_OPTIONS = foreign

SUBDIRS = src .

if HAVE_CHECK
SUBDIRS += tests
endif

test: check

# Copy README.md to README when building distribution
dist-hook:
	[ -f README.md ] && cat README.md > README || true
	[ -f NEWS.md ] && cat NEWS.md > NEWS || true
