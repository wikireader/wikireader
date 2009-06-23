
noinst_PROGRAMS = master_read_to_file master_write_from_file \
	slave_read_to_file slave_write_from_file
bin_PROGRAMS = ibtest

ibtest_SOURCES = ibtest.c
ibtest_CFLAGS = $(LIBGPIB_CFLAGS)
ibtest_LDADD = $(LIBGPIB_LDFLAGS)

master_read_to_file_SOURCES = master_read_to_file.c
master_read_to_file_CFLAGS = $(LIBGPIB_CFLAGS)
master_read_to_file_LDADD = $(LIBGPIB_LDFLAGS)

master_write_from_file_SOURCES = master_write_from_file.c
master_write_from_file_CFLAGS = $(LIBGPIB_CFLAGS)
master_write_from_file_LDADD = $(LIBGPIB_LDFLAGS)

slave_read_to_file_SOURCES = slave_read_to_file.c
slave_read_to_file_CFLAGS = $(LIBGPIB_CFLAGS)
slave_read_to_file_LDADD = $(LIBGPIB_LDFLAGS)

slave_write_from_file_SOURCES = slave_write_from_file.c
slave_write_from_file_CFLAGS = $(LIBGPIB_CFLAGS)
slave_write_from_file_LDADD = $(LIBGPIB_LDFLAGS)
