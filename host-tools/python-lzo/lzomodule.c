/* lzomodule.c -- Python bindings for the LZO compression library

   This file is part of the LZO real-time data compression library.

   Copyright (C) 2002 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 2001 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 2000 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1999 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1998 Markus Franz Xaver Johannes Oberhumer

   The LZO library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   The LZO library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the LZO library; see the file COPYING.
   If not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   Markus F.X.J. Oberhumer
   <markus@oberhumer.com>
   http://www.oberhumer.com/opensource/lzo/
 */


#define MODULE_VERSION  "1.08"

#include <Python.h>
#include <lzo1x.h>

/* Ensure we have updated versions */
#if !defined(PY_VERSION_HEX) || (PY_VERSION_HEX < 0x010502f0)
#  error "Need Python version 1.5.2 or greater"
#endif
#if !defined(LZO_VERSION) || (LZO_VERSION < 0x1070)
#  error "Need LZO version 1.07 or greater"
#endif

#undef UNUSED
#define UNUSED(var)     ((void)&var)

static PyObject *LzoError;


/***********************************************************************
// compress
************************************************************************/

static /* const */ char compress__doc__[] =
"compress(string) -- Compress string using the default compression level, "
"returning a string containing compressed data.\n"
"compress(string, level) -- Compress string, using the chosen compression "
"level (either 1 or 9).  Return a string containing the compressed data.\n"
;

static PyObject *
compress(PyObject *dummy, PyObject *args)
{
    PyObject *result_str;
    lzo_voidp wrkmem = NULL;
    const lzo_bytep in;
    lzo_bytep out;
    lzo_uint in_len;
    lzo_uint out_len;
    lzo_uint new_len;
    int len;
    int level = 1;
    int err;

    /* init */
    UNUSED(dummy);
    if (!PyArg_ParseTuple(args, "s#|i", &in, &len, &level))
        return NULL;
    if (len < 0)
        return NULL;
    in_len = len;
    out_len = in_len + in_len / 64 + 16 + 3;

    /* alloc buffers */
    result_str = PyString_FromStringAndSize(NULL, 5 + out_len);
    if (result_str == NULL)
        return PyErr_NoMemory();
    if (level == 1)
        wrkmem = (lzo_voidp) PyMem_Malloc(LZO1X_1_MEM_COMPRESS);
    else
        wrkmem = (lzo_voidp) PyMem_Malloc(LZO1X_999_MEM_COMPRESS);
    if (wrkmem == NULL)
    {
        Py_DECREF(result_str);
        return PyErr_NoMemory();
    }

    /* compress */
    out = (lzo_bytep) PyString_AsString(result_str);
    new_len = out_len;
    if (level == 1)
    {
        out[0] = 0xf0;
        err = lzo1x_1_compress(in, in_len, out+5, &new_len, wrkmem);
    }
    else
    {
        out[0] = 0xf1;
        err = lzo1x_999_compress(in, in_len, out+5, &new_len, wrkmem);
    }
    PyMem_Free(wrkmem);
    if (err != LZO_E_OK || new_len > out_len)
    {
        /* this should NEVER happen */
        Py_DECREF(result_str);
        PyErr_Format(LzoError, "Error %i while compressing data", err);
        return NULL;
    }

    /* save uncompressed length */
    out[1] = (unsigned char) ((in_len >> 24) & 0xff);
    out[2] = (unsigned char) ((in_len >> 16) & 0xff);
    out[3] = (unsigned char) ((in_len >>  8) & 0xff);
    out[4] = (unsigned char) ((in_len >>  0) & 0xff);

    /* return */
    if (new_len != out_len)
        _PyString_Resize(&result_str, 5 + new_len);
    return result_str;
}


/***********************************************************************
// decompress
************************************************************************/

static /* const */ char decompress__doc__[] =
"decompress(string) -- Decompress the data in string, returning a string containing the decompressed data.\n"
;

static PyObject *
decompress(PyObject *dummy, PyObject *args)
{
    PyObject *result_str;
    const lzo_bytep in;
    lzo_bytep out;
    lzo_uint in_len;
    lzo_uint out_len;
    lzo_uint new_len;
    int len;
    int err;

    /* init */
    UNUSED(dummy);
    if (!PyArg_ParseTuple(args, "s#", &in, &len))
        return NULL;
    if (len < 5 + 3 || in[0] < 0xf0 || in[0] > 0xf1)
        goto header_error;
    in_len = len - 5;
    out_len = (in[1] << 24) | (in[2] << 16) | (in[3] << 8) | in[4];
    if ((int)out_len < 0 || in_len > out_len + out_len / 64 + 16 + 3)
        goto header_error;

    /* alloc buffers */
    result_str = PyString_FromStringAndSize(NULL, out_len);
    if (result_str == NULL)
        return PyErr_NoMemory();

    /* decompress */
    out = (lzo_bytep) PyString_AsString(result_str);
    new_len = out_len;
    err = lzo1x_decompress_safe(in+5, in_len, out, &new_len, NULL);
    if (err != LZO_E_OK || new_len != out_len)
    {
        Py_DECREF(result_str);
        PyErr_Format(LzoError, "Compressed data violation %i", err);
        return NULL;
    }

    /* success */
    return result_str;

header_error:
    PyErr_SetString(LzoError, "Header error - invalid compressed data");
    return NULL;
}


/***********************************************************************
// optimize
************************************************************************/

static /* const */ char optimize__doc__[] =
"optimize(string) -- Optimize the representation of the compressed data, returning a string containing the compressed data.\n"
;

static PyObject *
optimize(PyObject *dummy, PyObject *args)
{
    PyObject *result_str;
    lzo_bytep in;
    lzo_bytep out;
    lzo_uint in_len;
    lzo_uint out_len;
    lzo_uint new_len;
    int len;
    int err;

    /* init */
    UNUSED(dummy);
    if (!PyArg_ParseTuple(args, "s#", &in, &len))
        return NULL;
    if (len < 5 + 3 || in[0] < 0xf0 || in[0] > 0xf1)
        goto header_error;
    in_len = len - 5;
    out_len = (in[1] << 24) | (in[2] << 16) | (in[3] << 8) | in[4];
    if ((int)out_len < 0 || in_len > out_len + out_len / 64 + 16 + 3)
        goto header_error;

    /* alloc buffers */
    result_str = PyString_FromStringAndSize(in, len);
    if (result_str == NULL)
        return PyErr_NoMemory();
    out = (lzo_bytep) PyMem_Malloc(out_len > 0 ? out_len : 1);
    if (out == NULL)
    {
        Py_DECREF(result_str);
        return PyErr_NoMemory();
    }

    /* optimize */
    in = (lzo_bytep) PyString_AsString(result_str);
    new_len = out_len;
    err = lzo1x_optimize(in+5, in_len, out, &new_len, NULL);
    PyMem_Free(out);
    if (err != LZO_E_OK || new_len != out_len)
    {
        Py_DECREF(result_str);
        PyErr_Format(LzoError, "Compressed data violation %i", err);
        return NULL;
    }

    /* success */
    return result_str;

header_error:
    PyErr_SetString(LzoError, "Header error - invalid compressed data");
    return NULL;
}


/***********************************************************************
// adler32
************************************************************************/

static /* const */ char adler32__doc__[] =
"adler32(string) -- Compute an Adler-32 checksum of string, using "
"a default starting value, and returning an integer value.\n"
"adler32(string, value) -- Compute an Adler-32 checksum of string, using "
"the starting value provided, and returning an integer value\n"
;

static PyObject *
adler32(PyObject *dummy, PyObject *args)
{
    char *buf;
    int len;
    unsigned long val = 1; /* == lzo_adler32(0, NULL, 0); */

    UNUSED(dummy);
    if (!PyArg_ParseTuple(args, "s#|l", &buf, &len, &val))
        return NULL;
    if (len > 0)
        val = lzo_adler32((lzo_uint32)val, (const lzo_bytep)buf, len);
    return PyInt_FromLong(val);
}


/***********************************************************************
// crc32
************************************************************************/

static /* const */ char crc32__doc__[] =
"crc32(string) -- Compute a CRC-32 checksum of string, using "
"a default starting value, and returning an integer value.\n"
"crc32(string, value) -- Compute a CRC-32 checksum of string, using "
"the starting value provided, and returning an integer value.\n"
;

static PyObject *
crc32(PyObject *dummy, PyObject *args)
{
    char *buf;
    int len;
    unsigned long val = 0; /* == lzo_crc32(0, NULL, 0); */

    UNUSED(dummy);
    if (!PyArg_ParseTuple(args, "s#|l", &buf, &len, &val))
        return NULL;
    if (len > 0)
        val = lzo_crc32((lzo_uint32)val, (const lzo_bytep)buf, len);
    return PyInt_FromLong(val);
}


/***********************************************************************
// main
************************************************************************/

static /* const */ PyMethodDef methods[] =
{
    {"adler32",    (PyCFunction)adler32,    METH_VARARGS, adler32__doc__},
    {"compress",   (PyCFunction)compress,   METH_VARARGS, compress__doc__},
    {"crc32",      (PyCFunction)crc32,      METH_VARARGS, crc32__doc__},
    {"decompress", (PyCFunction)decompress, METH_VARARGS, decompress__doc__},
    {"optimize",   (PyCFunction)optimize,   METH_VARARGS, optimize__doc__},
    {NULL, NULL, 0, NULL}
};


static /* const */ char module_documentation[]=
"The functions in this module allow compression and decompression "
"using the LZO library.\n\n"
"adler32(string) -- Compute an Adler-32 checksum.\n"
"adler32(string, start) -- Compute an Adler-32 checksum using a given starting value.\n"
"compress(string) -- Compress a string.\n"
"compress(string, level) -- Compress a string with the given level of compression (either 1 or 9).\n"
"crc32(string) -- Compute a CRC-32 checksum.\n"
"crc32(string, start) -- Compute a CRC-32 checksum using a given starting value.\n"
"decompress(string) -- Decompresses a compressed string.\n"
"optimize(string) -- Optimize a compressed string.\n"
;


#ifdef _MSC_VER
_declspec(dllexport)
#endif
void initlzo(void)
{
    PyObject *m, *d, *v;

    if (lzo_init() != LZO_E_OK)
        return;

    m = Py_InitModule4("lzo", methods, module_documentation,
                       NULL, PYTHON_API_VERSION);
    d = PyModule_GetDict(m);

    LzoError = PyErr_NewException("lzo.error", NULL, NULL);
    PyDict_SetItemString(d, "error", LzoError);

    v = PyString_FromString("Markus F.X.J. Oberhumer <markus@oberhumer.com>");
    PyDict_SetItemString(d, "__author__", v);
    Py_DECREF(v);
    v = PyString_FromString(MODULE_VERSION);
    PyDict_SetItemString(d, "__version__", v);
    Py_DECREF(v);
    v = PyInt_FromLong((long)lzo_version());
    PyDict_SetItemString(d, "LZO_VERSION", v);
    Py_DECREF(v);
    v = PyString_FromString(lzo_version_string());
    PyDict_SetItemString(d, "LZO_VERSION_STRING", v);
    Py_DECREF(v);
    v = PyString_FromString(lzo_version_date());
    PyDict_SetItemString(d, "LZO_VERSION_DATE", v);
    Py_DECREF(v);
}


/*
vi:ts=4:et
*/
