# SPDX-FileCopyrightText: CERN
#
# SPDX-License-Identifier: GPL-3.0-or-later

"""Python bindings for LGC2 (ctypes wrapper around pyLGC_C)."""

import ctypes
import os
import sys
from collections import namedtuple

import numpy as np

# ---------------------------------------------------------------------------
# Load the shared library
# ---------------------------------------------------------------------------

# NOTE: only Windows and Linux are officially supported. macOS has never
# been tested; if pyLGC_C is ever built on macOS (producing a .dylib), add
# a fallback for that here.
_dll_name = "pyLGC_C.dll" if sys.platform == "win32" else "libpyLGC_C.so"
_dll_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), _dll_name)
_lib = ctypes.CDLL(_dll_path)

# ---------------------------------------------------------------------------
# ctypes shorthand
# ---------------------------------------------------------------------------

_vp    = ctypes.c_void_p
_cp    = ctypes.c_char_p
_ip    = ctypes.POINTER(ctypes.c_int)
_dp    = ctypes.POINTER(ctypes.c_double)
_ipp   = ctypes.POINTER(_ip)
_dpp   = ctypes.POINTER(_dp)
_int   = ctypes.c_int
_dbl   = ctypes.c_double

# ---------------------------------------------------------------------------
# Declare C function signatures
# ---------------------------------------------------------------------------

def _sig(name, restype, argtypes):
    fn = getattr(_lib, name)
    fn.restype  = restype
    fn.argtypes = argtypes
    return fn

_get_last_error       = _sig("lgcGetLastError",       _cp,  [])
_free_double          = _sig("lgcFreeDoubleArray",     None, [_dp])
_free_int             = _sig("lgcFreeIntArray",        None, [_ip])

_ev_create            = _sig("lgcEvaluatorCreate",      _vp,  [_cp])
_ev_destroy           = _sig("lgcEvaluatorDestroy",     None, [_vp])
_ev_evaluate          = _sig("lgcEvaluatorEvaluate",    _int, [_vp])
_ev_set_params        = _sig("lgcEvaluatorSetParameters", _int, [_vp, _dp, _int])
_ev_get_params        = _sig("lgcEvaluatorGetEstimatedParameters", _int, [_vp, _dpp, _ip])
_ev_get_indices       = _sig("lgcEvaluatorGetProblemDimensions", _int, [_vp, _ip, _ip, _ip, _ip])
_ev_get_misc          = _sig("lgcEvaluatorGetMisclosure", _int, [_vp, _dpp, _ip])
_ev_get_cmisc         = _sig("lgcEvaluatorGetConstraintMisclosure", _int, [_vp, _dpp, _ip])
_ev_get_A             = _sig("lgcEvaluatorGetFirstDesignMatrix",     _int, [_vp, _ipp, _ipp, _dpp, _ip, _ip, _ip])
_ev_get_B             = _sig("lgcEvaluatorGetSecondDesignMatrix",     _int, [_vp, _ipp, _ipp, _dpp, _ip, _ip, _ip])
_ev_get_A2            = _sig("lgcEvaluatorGetConstraintDesignMatrix",    _int, [_vp, _ipp, _ipp, _dpp, _ip, _ip, _ip])
_ev_get_P             = _sig("lgcEvaluatorGetWeightMatrix",     _int, [_vp, _ipp, _ipp, _dpp, _ip, _ip, _ip])
_ev_solve             = _sig("lgcEvaluatorSolve",   _int, [_vp, _ip, _dpp, _ip])
_ev_obs_map           = _sig("lgcEvaluatorGetObsIndexToLineNumber", _int, [_vp, _ipp, _ipp, _ip])

_ev_get_point         = _sig("lgcEvaluatorGetPoint",   _vp,  [_vp, _cp])
_ev_get_frame         = _sig("lgcEvaluatorGetFrame",   _vp,  [_vp, _cp])
_adj_get_name         = _sig("lgcAdjObjGetName",       _cp,  [_vp])
_adj_get_uidx         = _sig("lgcAdjObjGetFirstUidx", _int, [_vp])
_adj_get_rel          = _sig("lgcAdjObjGetRelativeUnknIndices", _int, [_vp, _ipp, _ip])
_adj_get_est          = _sig("lgcAdjObjGetEstVector", _int, [_vp, _dpp, _ip])

# ---------------------------------------------------------------------------
# Error helpers
# ---------------------------------------------------------------------------

def _last_error():
    msg = _get_last_error()
    return msg.decode("utf-8") if msg else "Unknown error"

def _check(result):
    if result == -1:
        raise RuntimeError(_last_error())
    return result

def _check_ptr(ptr):
    if not ptr:
        raise RuntimeError(_last_error())
    return ptr

# ---------------------------------------------------------------------------
# Array extraction helpers
# ---------------------------------------------------------------------------

def _doubles(func, *args):
    data = _dp()
    n    = _int()
    _check(func(*args, ctypes.byref(data), ctypes.byref(n)))
    try:
        out = np.ctypeslib.as_array(data, shape=(n.value,)).copy()
    finally:
        _free_double(data)
    return out

def _ints(func, *args):
    data = _ip()
    n    = _int()
    _check(func(*args, ctypes.byref(data), ctypes.byref(n)))
    try:
        out = np.ctypeslib.as_array(data, shape=(n.value,)).copy()
    finally:
        _free_int(data)
    return out

def _sparse(func, *args):
    rows  = _ip()
    cols  = _ip()
    vals  = _dp()
    nnz   = _int()
    nrows = _int()
    ncols = _int()
    _check(func(*args,
                ctypes.byref(rows), ctypes.byref(cols), ctypes.byref(vals),
                ctypes.byref(nnz), ctypes.byref(nrows), ctypes.byref(ncols)))
    try:
        k = nnz.value
        r = np.ctypeslib.as_array(rows, shape=(k,)).copy()
        c = np.ctypeslib.as_array(cols, shape=(k,)).copy()
        v = np.ctypeslib.as_array(vals, shape=(k,)).copy()
    finally:
        _free_int(rows)
        _free_int(cols)
        _free_double(vals)
    return (r, c, v, nrows.value, ncols.value)

# ---------------------------------------------------------------------------
# Public types
# ---------------------------------------------------------------------------

UEOIndices = namedtuple("UEOIndices", ["UIndex", "EIndex", "OIndex", "CIndex"])


class AdjustableObject:
    """Borrowed reference to an adjustable object (lifetime tied to Evaluator).
    Works for both points and frames."""

    def __init__(self, handle):
        self._h = handle

    def getName(self):
        return _adj_get_name(self._h).decode("utf-8")

    def getFirstUidx(self):
        return _adj_get_uidx(self._h)

    def getRelativeUnknIndices(self):
        return _ints(_adj_get_rel, self._h)

    def getEstVector(self):
        return _doubles(_adj_get_est, self._h)

# Backwards-compatible aliases
AdjustablePoint = AdjustableObject
AdjustableFrame = AdjustableObject


class Evaluator:
    """Main LGC least-squares evaluator."""

    def __init__(self, filePath):
        self._h = _check_ptr(_ev_create(filePath.encode("utf-8")))

    def __del__(self, _destroy=_ev_destroy):
        if getattr(self, "_h", None):
            _destroy(self._h)
            self._h = None

    def evaluate(self):
        r = _ev_evaluate(self._h)
        if r == -1:
            raise RuntimeError(_last_error())
        return bool(r)

    def setParameters(self, para):
        arr = (ctypes.c_double * len(para))(*para)
        _check(_ev_set_params(self._h, arr, len(para)))

    def getEstimatedParameters(self):
        return _doubles(_ev_get_params, self._h)

    def getProblemDimensions(self):
        u, e, o, c = _int(), _int(), _int(), _int()
        _check(_ev_get_indices(self._h,
                               ctypes.byref(u), ctypes.byref(e),
                               ctypes.byref(o), ctypes.byref(c)))
        return UEOIndices(u.value, e.value, o.value, c.value)

    def getMisclosure(self):
        return _doubles(_ev_get_misc, self._h)

    def getConstraintMisclosure(self):
        return _doubles(_ev_get_cmisc, self._h)

    def getFirstDesignMatrix(self):
        return _sparse(_ev_get_A, self._h)

    def getSecondDesignMatrix(self):
        return _sparse(_ev_get_B, self._h)

    def getConstraintDesignMatrix(self):
        return _sparse(_ev_get_A2, self._h)

    def getWeightMatrix(self):
        return _sparse(_ev_get_P, self._h)

    def solve(self):
        ok  = _int()
        sol = _dp()
        n   = _int()
        _check(_ev_solve(self._h, ctypes.byref(ok), ctypes.byref(sol), ctypes.byref(n)))
        try:
            solution = np.ctypeslib.as_array(sol, shape=(n.value,)).copy()
        finally:
            _free_double(sol)
        return (bool(ok.value), solution)

    def getObsIndexToLineNumber(self):
        keys = _ip()
        vals = _ip()
        n    = _int()
        _check(_ev_obs_map(self._h, ctypes.byref(keys), ctypes.byref(vals), ctypes.byref(n)))
        result = {keys[i]: vals[i] for i in range(n.value)}
        _free_int(keys)
        _free_int(vals)
        return result

    def getPoint(self, name):
        h = _check_ptr(_ev_get_point(self._h, name.encode("utf-8")))
        return AdjustableObject(h)

    def getFrame(self, name):
        h = _check_ptr(_ev_get_frame(self._h, name.encode("utf-8")))
        return AdjustableObject(h)
