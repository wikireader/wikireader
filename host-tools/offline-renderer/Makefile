SAMO_LIB := $(shell readlink -es ../samo-lib || readlink -es ../../samo-lib || readlink -es ../../../samo-lib)

include ${SAMO_LIB}/Mk/definitions.mk

PYLZMA_DIR := pylzma-0.3.0

TARGETS =

.PHONY: all
all: index

.PHONY: index
index: pylzma
	@if [ -z "${XML_FILES}" ] ; then echo XML_FILES is not set ; exit 1; fi
	./make_article_idx.py --verbose ${XML_FILES}
	./parse_articles.py --verbose ${XML_FILES}
	./make_wrformat.py --verbose --font-path=${HOST_TOOLS}/fonts all_articles.html
	./combine_idx.py --verbose

# sudo apt-get install python-dev
.PHONY: pylzma
pylzma:
	cd ${PYLZMA_DIR} && \
	python setup.py build && \
	sudo python setup.py install

.PHONY: install
install: all
	#@if [ ! -d "${DESTDIR}" ] ; then echo DESTDIR: "'"${DESTDIR}"'" is not a directory ; exit 1; fi
	#${COPY} *.bmf "${DESTDIR}"/

build:
	${MKDIR} "$@"

.PHONY: clean
clean: pylzma-clean
	${RM} -r build ${TARGETS} article_idx.py *.pyc all_articles.html *.dat *.idx *.idx-tmp *.pfx *.fnd

.PHONY: pylzma-clean
pylzma-clean:
	cd ${PYLZMA_DIR} && \
	python setup.py clean


include ${SAMO_LIB}/Mk/rules.mk
