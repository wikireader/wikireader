#include <string.h>

#include <external_interface.h>

struct file_ops wikilib_file_ops;
struct machine_ops wikilib_machine_ops;

int wikilib_init (struct file_ops *file_ops, struct machine_ops *machine_ops)
{
	if (!file_ops || !machine_ops)
		return -1;
	
	memcpy(&wikilib_file_ops, file_ops, sizeof(wikilib_file_ops));
	memcpy(&wikilib_machine_ops, machine_ops, sizeof(wikilib_machine_ops));

	return 0;
}

int wikilib_process_input_event (int key, int state)
{

	return 0;
}

