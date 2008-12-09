#ifndef WIKILIB_EXTERNAL_INTERFACE_H
#define WIKILIB_EXTERNAL_INTERFACE_H



struct machine_ops
{
	int (* set_power_mode)         (int mode);
	int (* get_battery_voltage)    (void);
	int (* sdcard_present)         (void);
	int (* sdcard_write_protected) (void);
};

struct file_ops
{
	int  (* open)  (const char *name, int mode);
	void (* close) (int fd);
	int  (* read)  (int fd, char *buf, unsigned long len);
	int  (* write) (int fd, char *buf, unsigned long len);
	int  (* seek)  (int fd, unsigned long pos, int whence);
};

int wikilib_init (struct file_ops *file_ops, struct machine_ops *cpu_ops);
int wikilib_process_input_event (int key, int state);

#endif /* WIKILIB_EXTERNAL_INTERFACE_H */

