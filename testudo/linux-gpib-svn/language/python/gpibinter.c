/***********************************************************
 * Python wrapper module for gpib library functions.
 ************************************************************/


#include "Python.h"

#ifdef USE_INES
#include <ugpib.h>
#else
#include <gpib/ib.h>
#endif

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

static PyObject *GpibError;


struct _iberr_string {
	int code;
	char *meaning;
} _iberr_string;

static struct _iberr_string GPIB_errors[] = {
	{EDVR, "A system call has failed. ibcnt/ibcntl will be set to the value of errno."},
	{ECIC, "Your interface board needs to be controller-in-charge, but is not."},
	{ENOL, "You have attempted to write data or command bytes, but there are no listeners currently addressed."},
	{EADR, "The interface board has failed to address itself properly before starting an io operation."},
	{EARG, "One or more arguments to the function call were invalid."},
	{ESAC, "The interface board needs to be system controller, but is not."},
	{EABO, "A read or write of data bytes has been aborted, possibly due to a timeout or reception of a device clear command."},
	{ENEB, "The GPIB interface board does not exist, its driver is not loaded, or it is in use by another process."},
	{EDMA, "Not used (DMA error), included for compatibility purposes."},
	{EOIP, "Function call can not proceed due to an asynchronous IO operation (ibrda(), ibwrta(), or ibcmda()) in progress."},
	{ECAP, "Incapable of executing function call, due the GPIB board lacking the capability, or the capability being disabled in software."},
	{EFSO, "File system error. ibcnt/ibcntl will be set to the value of errno."},
	{EBUS, "An attempt to write command bytes to the bus has timed out."},
	{ESTB, "One or more serial poll status bytes have been lost. This can occur due to too many status bytes accumulating (through automatic serial polling) without being read."},
	{ESRQ, "The serial poll request service line is stuck on."},
	{ETAB, "This error can be returned by ibevent(), FindLstn(), or FindRQS(). See their descriptions for more information."},
	{0, NULL},
};

void _SetGpibError(const char *funcname)
{
	char *errstr;
	struct _iberr_string entry;
	int sverrno, code;

	code = ThreadIberr();
	errstr = (char *) PyMem_Malloc(4096);

	if (code == EDVR || code == EFSO) {
		sverrno = ThreadIbcntl();
		snprintf(errstr, 4096, "%s() error: %s (errno: %d)", 
			 funcname, strerror(sverrno), sverrno);
	} else {
		int i;
		for (i=0; entry=GPIB_errors[i], entry.meaning!=NULL; i++) {
			if (entry.code == code)
				break;
		}
		if (entry.meaning != NULL)
			snprintf(errstr, 4096, "%s() failed: %s", 
				 funcname, entry.meaning);
		else
			snprintf(errstr, 4096, 
				 "%s() failed: unknown reason (iberr: %d).", funcname, code);
	}
	PyErr_SetString(GpibError, errstr);
	PyMem_Free(errstr);
}



/* ----------------------------------------------------- */

static char gpib_find__doc__[] =
	"find -- get a device handle from configuration file\n"
	"find(name) -> handle";

static PyObject* gpib_find(PyObject *self, PyObject *args)
{
	char *name;
	int ud;

	if (!PyArg_ParseTuple(args, "s:find", &name))
		return NULL;

	ud = ibfind(name);
	if(ud < 0){
		_SetGpibError("find");
		return NULL;
	}
	return PyInt_FromLong(ud);
}

static char gpib_dev__doc__[] =
	"dev -- get a device handle\n"
	"dev(boardid, pad, [sad, timeout, eot, eos_mode]) -> handle";

static PyObject* gpib_dev(PyObject *self, PyObject *args)
{
	int ud = -1;
	int board = 0;
	int pad = 0;
	int sad = NO_SAD;
	int tmo = T30s;
	int eot = 1;
	int eos_mode = 0;

	if (!PyArg_ParseTuple(args, "ii|iiii:dev", &board, &pad, &sad, &tmo, &eot, &eos_mode))
		return NULL;
	ud = ibdev(board, pad, sad, tmo, eot, eos_mode);
	if (ud < 0) {
		_SetGpibError("dev");
		return NULL;
	}
	return PyInt_FromLong(ud);
}


static char gpib_ask__doc__[] =
	"ask -- query configuration (board or device)\n"
	"ask(handle, option) -> result\n\n"
	"option should be one one of the symbolic constants gpib.IbaXXXX";

static PyObject* gpib_ask(PyObject *self, PyObject *args)
{
	int device;
	int option;
	int result;

	if (!PyArg_ParseTuple(args, "ii:ask", &device, &option))
		return NULL;

	if (ibask(device, option, &result) & ERR) {
		_SetGpibError("ask");
		return NULL;
	}

	return PyInt_FromLong(result);
}


static char gpib_config__doc__[] =
	"config -- change configuration (board or device)\n"
	"config(handle, option, setting)\n\n"
	"option should be one one of the symbolic constants gpib.IbcXXXX";
 
static PyObject* gpib_config(PyObject *self, PyObject *args)
{
	int device;
	int option;
	int setting;
	int sta;

	if (!PyArg_ParseTuple(args, "iii:config", &device, &option, &setting))
		return NULL;

	sta = ibconfig(device, option, setting);
	if(sta & ERR) {
		_SetGpibError("config");
		return NULL;
	}

	return PyInt_FromLong(sta);
}

static char gpib_listener__doc__[] =
	"listener -- check if listener is present (board or device)\n"
	"listener(handle, pad, [sad]) -> boolean";

static PyObject* gpib_listener(PyObject *self, PyObject *args)
{
	int device;
	int pad;
	int sad = NO_SAD;
	short found_listener;

	if(!PyArg_ParseTuple(args, "ii|i:listener", &device, &pad, &sad))
		return NULL;
	if(ibln(device, pad, sad, &found_listener) & ERR){
		_SetGpibError("listener");
		return NULL;
	}

	return PyBool_FromLong(found_listener);
}

static char gpib_read__doc__[] =
	"read -- read data bytes (board or device)\n"
	"read(handle, num_bytes) -> string";

static PyObject* gpib_read(PyObject *self, PyObject *args)
{
	int device;
	int len;
	PyObject *retval;

	if (!PyArg_ParseTuple(args, "ii:read", &device,&len))
		return NULL;

	/* Instead of using a C-buffer and then copying it into a
	   Python-string, just build up an uninitialized Python-string.
	   No copying needed, more efficient. (yes, this is legal, see
	   http://www.python.org/doc/current/api/stringObjects.html) */

	retval = PyString_FromStringAndSize(NULL, len);
	if(retval == NULL)
	{
		PyErr_SetString(GpibError, "Read Error: can't get Memory.");
		return NULL;
	}

	if( ibrd(device, PyString_AS_STRING(retval), len) & ERR )
	{
		_SetGpibError("read");
		Py_DECREF(retval);
		return NULL;
	}

	_PyString_Resize(&retval, ThreadIbcntl());
	return retval;
}

static char gpib_write__doc__[] =
	"write -- write data bytes (board or device)\n"
	"write(handle, data)";

static PyObject* gpib_write(PyObject *self, PyObject *args)
{
	char *command;
	int command_len;
	int  device;
	int sta;

	if (!PyArg_ParseTuple(args, "is#:write",&device, &command, &command_len))
		return NULL;
	sta = ibwrt(device, command, command_len);
	if( sta & ERR ){
		_SetGpibError("write");
		return NULL;
	}

	return PyInt_FromLong(sta);
}

static char gpib_write_async__doc__[] =
	"write_async -- write data bytes asynchronously (board or device)\n"
	"write_async(handle, data)";

static PyObject* gpib_write_async(PyObject *self, PyObject *args)
{
	char *command;
	int  command_len;
	int  device;
	int  sta;

	if (!PyArg_ParseTuple(args, "is#:write_async", &device, &command, &command_len))
		return NULL;
	sta = ibwrta(device, command, command_len);
	if( sta & ERR ){
		_SetGpibError("write_async");
		return NULL;
	}

	return PyInt_FromLong(sta);
}


static char gpib_command__doc__[] =
	"command -- write command bytes (board)\n"
	"command(handle, data)";

static PyObject* gpib_command(PyObject *self, PyObject *args)
{
	char *command;
	int  command_len;
	int  device;
	int  sta;

	if (!PyArg_ParseTuple(args, "is#:command", &device, &command, &command_len))
		return NULL;
	sta = ibcmd(device, command, command_len);
	if ( sta  & ERR ) {
		_SetGpibError("cmd");
		return NULL;
	}

	return PyInt_FromLong(sta);
}

static char gpib_remote_enable__doc__[] =
	"remote_enable -- set remote enable (board)\n"
	"remote_enable(handle, enable)";

static PyObject* gpib_remote_enable(PyObject *self, PyObject *args)
{
	int device;
	int val;
	int sta;

	if (!PyArg_ParseTuple(args, "ii:remote_enable", &device,&val))
		return NULL;

	sta = ibsre(device,val);
	if( sta & ERR){
		_SetGpibError("remote_enable");
		return NULL;
	}

	return PyInt_FromLong(sta);
}


static char gpib_clear__doc__[] =
	"clear -- clear device (device)\n"
	"clear(handle)";

static PyObject* gpib_clear(PyObject *self, PyObject *args)
{
	int device;
	int sta;

	if (!PyArg_ParseTuple(args, "i:clear", &device))
		return NULL;

	sta = ibclr(device);
	if( sta & ERR){
		_SetGpibError("clear");
		return NULL;
	}

	return PyInt_FromLong(sta);
}


static char gpib_interface_clear__doc__[] =
	"interface_clear -- perform interface clear (board)\n"
	"interface_clear(handle)";

static PyObject* gpib_interface_clear(PyObject *self, PyObject *args)
{
	int device;
	int sta;

	if (!PyArg_ParseTuple(args, "i:interface_clear", &device))
		return NULL;

	sta = ibsic(device);
	if ( sta & ERR){
		_SetGpibError("interface_clear");
		return NULL;
	}

	return PyInt_FromLong(sta);
}


static char gpib_close__doc__[] =
	"close -- close descriptor (board or device)\n"
	"close(handle)";

static PyObject* gpib_close(PyObject *self, PyObject *args)
{
	int device;
	int sta;

	if (!PyArg_ParseTuple(args, "i:close", &device))
		return NULL;

	sta = ibonl(device, 0);
	if( sta & ERR ){
		_SetGpibError("close");
		return NULL;
	}

	return PyInt_FromLong(sta);
}

static char gpib_wait__doc__[] =
	"wait -- wait for event (board or device)\n"
	"wait(handle, mask)";

static PyObject* gpib_wait(PyObject *self, PyObject *args)
{
	int device;
	int mask;
	int sta;

	if (!PyArg_ParseTuple(args, "ii:wait", &device, &mask))
		return NULL;

	sta = ibwait(device, mask);
	if(sta & ERR) {
		_SetGpibError("wait");
		return NULL;
	}

	return PyInt_FromLong(sta);
}

static char gpib_timeout__doc__[] =
	"timeout -- adjust io timeout (board or device)\n"
	"timeout(handle, timeout)\n\n"
	"timeout should be one of the symbolic constants TNONE to T1000s";

static PyObject* gpib_timeout(PyObject *self, PyObject *args)
{
	int device;
	int value;
	int sta;

	if (!PyArg_ParseTuple(args, "ii:timeout", &device,&value))
		return NULL;
	sta = ibtmo(device, value);
	if( sta & ERR){
		_SetGpibError("tmo");
		return NULL;
	}
	return PyInt_FromLong(sta);
}

static char gpib_serial_poll__doc__[] =
	"serial_poll -- conduct serial poll (device)\n"
	"serial_poll(handle) -> status_byte";

static PyObject* gpib_serial_poll(PyObject *self, PyObject *args)
{
	int device;
	char spr;

	if (!PyArg_ParseTuple(args, "i:serial_poll", &device))
		return NULL;

	if( ibrsp(device, &spr) & ERR){
		_SetGpibError("serial_poll");
		return NULL;
	}
	
	return Py_BuildValue("c", spr);
}

static char gpib_trigger__doc__[] =
	"trigger -- trigger device (device)\n"
	"trigger(handle)";

static PyObject* gpib_trigger(PyObject *self, PyObject *args)
{
	int device;
	int sta;

	if (!PyArg_ParseTuple(args, "i:trigger", &device))
		return NULL;

	sta = ibtrg(device);
	if( sta & ERR){
		_SetGpibError("trg");
		return NULL;
	}

	return PyInt_FromLong(sta);
}

static char gpib_ibsta__doc__[] =
	"ibsta -- retrieve status\n"
	"ibsta()";

static PyObject* gpib_ibsta(PyObject *self, PyObject *args)
{
	return PyInt_FromLong(ThreadIbsta());
}

static char gpib_ibcnt__doc__[] =
	"ibcnt -- retrieve number of bytes transferred\n"
	"ibcnt()";

static PyObject* gpib_ibcnt(PyObject *self, PyObject *args)
{
	return PyInt_FromLong(ThreadIbcntl());
}

/* List of methods defined in the module */

static struct PyMethodDef gpib_methods[] = {
	{"find",		gpib_find,		METH_VARARGS,	gpib_find__doc__},
	{"ask",			gpib_ask,		METH_VARARGS,	gpib_ask__doc__},
	{"dev",			gpib_dev,		METH_VARARGS,	gpib_dev__doc__},
	{"config",		gpib_config,		METH_VARARGS,	gpib_config__doc__},
	{"listener",		gpib_listener,		METH_VARARGS,	gpib_listener__doc__},
	{"read",		gpib_read,		METH_VARARGS,	gpib_read__doc__},
	{"write",		gpib_write,		METH_VARARGS,	gpib_write__doc__},
	{"write_async",		gpib_write_async,	METH_VARARGS,	gpib_write_async__doc__},
	{"command",		gpib_command,		METH_VARARGS,	gpib_command__doc__},
	{"remote_enable",	gpib_remote_enable,	METH_VARARGS,	gpib_remote_enable__doc__},
	{"clear",		gpib_clear,		METH_VARARGS,	gpib_clear__doc__},
	{"interface_clear",	gpib_interface_clear,	METH_VARARGS,	gpib_interface_clear__doc__},
	{"close",		gpib_close,		METH_VARARGS,	gpib_close__doc__},
	{"wait",		gpib_wait,		METH_VARARGS,	gpib_wait__doc__},
	{"timeout",		gpib_timeout,		METH_VARARGS,	gpib_timeout__doc__},
	{"serial_poll",		gpib_serial_poll,	METH_VARARGS,	gpib_serial_poll__doc__},
	{"trigger",		gpib_trigger,		METH_VARARGS,	gpib_trigger__doc__},
	{"ibsta",		gpib_ibsta,		METH_NOARGS,	gpib_ibsta__doc__},
	{"ibcnt",		gpib_ibcnt,		METH_NOARGS,	gpib_ibcnt__doc__},
	{NULL,		NULL}		/* sentinel */
};


/* Initialization function for the module (*must* be called initgpib) */

static char gpib_module_documentation[] = 
	"This module is a thin wrapper around the Linux GPIB C library.\n"
	"Documentation for the C library is available at:\n"
	"    http://linux-gpib.sourceforge.net\n\n"
	"As in the C API, all functions return the value of ibsta,\n"
	"except where otherwise specified.";

void initgpib(void)
{
	PyObject *m;

	/* Create the module and add the functions */
	m = Py_InitModule4("gpib", gpib_methods, gpib_module_documentation,
		(PyObject*)NULL, PYTHON_API_VERSION);

	/* Add GpibError exception to the module */
	GpibError = PyErr_NewException("gpib.GpibError", NULL, NULL);
	PyModule_AddObject(m, "GpibError", GpibError);

	/* Add some symbolic constants to the module */
	/* timeout values */
	PyModule_AddIntConstant(m, "TNONE", TNONE);
	PyModule_AddIntConstant(m, "T10us", T10us);
	PyModule_AddIntConstant(m, "T30us", T30us);
	PyModule_AddIntConstant(m, "T100us", T100us);
	PyModule_AddIntConstant(m, "T300us", T300us);
	PyModule_AddIntConstant(m, "T1ms", T1ms);
	PyModule_AddIntConstant(m, "T3ms", T3ms);
	PyModule_AddIntConstant(m, "T10ms", T10ms);
	PyModule_AddIntConstant(m, "T30ms", T30ms);
	PyModule_AddIntConstant(m, "T100ms", T100ms);
	PyModule_AddIntConstant(m, "T300ms", T300ms);
	PyModule_AddIntConstant(m, "T1s", T1s);
	PyModule_AddIntConstant(m, "T3s", T3s);
	PyModule_AddIntConstant(m, "T10s", T10s);
	PyModule_AddIntConstant(m, "T30s", T30s);
	PyModule_AddIntConstant(m, "T100s", T100s);
	PyModule_AddIntConstant(m, "T300s", T300s);
	PyModule_AddIntConstant(m, "T1000s", T1000s);
	
	/* ibconfig() option values */
	PyModule_AddIntConstant(m, "IbcPAD", IbcPAD);
	PyModule_AddIntConstant(m, "IbcSAD", IbcSAD);
	PyModule_AddIntConstant(m, "IbcTMO", IbcTMO);
	PyModule_AddIntConstant(m, "IbcEOT", IbcEOT);
	PyModule_AddIntConstant(m, "IbcPPC", IbcPPC);
	PyModule_AddIntConstant(m, "IbcREADDR", IbcREADDR);
	PyModule_AddIntConstant(m, "IbcAUTOPOLL", IbcAUTOPOLL);
	PyModule_AddIntConstant(m, "IbcCICPROT", IbcCICPROT);
	PyModule_AddIntConstant(m, "IbcSC", IbcSC);
	PyModule_AddIntConstant(m, "IbcSRE", IbcSRE);
	PyModule_AddIntConstant(m, "IbcEOSrd", IbcEOSrd);
	PyModule_AddIntConstant(m, "IbcEOSwrt", IbcEOSwrt);
	PyModule_AddIntConstant(m, "IbcEOScmp", IbcEOScmp);
	PyModule_AddIntConstant(m, "IbcEOSchar", IbcEOSchar);
	PyModule_AddIntConstant(m, "IbcPP2", IbcPP2);
	PyModule_AddIntConstant(m, "IbcTIMING", IbcTIMING);
	PyModule_AddIntConstant(m, "IbcReadAdjust", IbcReadAdjust);
	PyModule_AddIntConstant(m, "IbcWriteAdjust", IbcWriteAdjust);
	PyModule_AddIntConstant(m, "IbcEventQueue", IbcEventQueue);
	PyModule_AddIntConstant(m, "IbcSPollBit", IbcSPollBit);
	PyModule_AddIntConstant(m, "IbcSendLLO", IbcSendLLO);
	PyModule_AddIntConstant(m, "IbcSPollTime", IbcSPollTime);
	PyModule_AddIntConstant(m, "IbcPPollTime", IbcPPollTime);
	PyModule_AddIntConstant(m, "IbcEndBitIsNormal", IbcEndBitIsNormal);
	PyModule_AddIntConstant(m, "IbcUnAddr", IbcUnAddr);
	PyModule_AddIntConstant(m, "IbcHSCableLength", IbcHSCableLength);
	PyModule_AddIntConstant(m, "IbcIst", IbcIst);
	PyModule_AddIntConstant(m, "IbcRsv", IbcRsv);
	PyModule_AddIntConstant(m, "IbcBNA", IbcBNA);

	/* ibask() option values */
	PyModule_AddIntConstant(m, "IbaPAD", IbaPAD);
	PyModule_AddIntConstant(m, "IbaSAD", IbaSAD);
	PyModule_AddIntConstant(m, "IbaTMO", IbaTMO);
	PyModule_AddIntConstant(m, "IbaEOT", IbaEOT);
	PyModule_AddIntConstant(m, "IbaPPC", IbaPPC);
	PyModule_AddIntConstant(m, "IbaREADDR", IbaREADDR);
	PyModule_AddIntConstant(m, "IbaAUTOPOLL", IbaAUTOPOLL);
	PyModule_AddIntConstant(m, "IbaCICPROT", IbaCICPROT);
	PyModule_AddIntConstant(m, "IbaSC", IbaSC);
	PyModule_AddIntConstant(m, "IbaSRE", IbaSRE);
	PyModule_AddIntConstant(m, "IbaEOSrd", IbaEOSrd);
	PyModule_AddIntConstant(m, "IbaEOSwrt", IbaEOSwrt);
	PyModule_AddIntConstant(m, "IbaEOScmp", IbaEOScmp);
	PyModule_AddIntConstant(m, "IbaEOSchar", IbaEOSchar);
	PyModule_AddIntConstant(m, "IbaPP2", IbaPP2);
	PyModule_AddIntConstant(m, "IbaTIMING", IbaTIMING);
	PyModule_AddIntConstant(m, "IbaReadAdjust", IbaReadAdjust);
	PyModule_AddIntConstant(m, "IbaWriteAdjust", IbaWriteAdjust);
	PyModule_AddIntConstant(m, "IbaEventQueue", IbaEventQueue);
	PyModule_AddIntConstant(m, "IbaSPollBit", IbaSPollBit);
	PyModule_AddIntConstant(m, "IbaSendLLO", IbaSendLLO);
	PyModule_AddIntConstant(m, "IbaSPollTime", IbaSPollTime);
	PyModule_AddIntConstant(m, "IbaPPollTime", IbaPPollTime);
	PyModule_AddIntConstant(m, "IbaEndBitIsNormal", IbaEndBitIsNormal);
	PyModule_AddIntConstant(m, "IbaUnAddr", IbaUnAddr);
	PyModule_AddIntConstant(m, "IbaHSCableLength", IbaHSCableLength);
	PyModule_AddIntConstant(m, "IbaIst", IbaIst);
	PyModule_AddIntConstant(m, "IbaRsv", IbaRsv);
	PyModule_AddIntConstant(m, "IbaBNA", IbaBNA);
	PyModule_AddIntConstant(m, "Iba7BitEOS", Iba7BitEOS);

	/* Check for errors */
	if (PyErr_Occurred())
		Py_FatalError("can't initialize module gpib");
}
