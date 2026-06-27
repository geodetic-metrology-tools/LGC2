# SPDX-FileCopyrightText: CERN
#
# SPDX-License-Identifier: GPL-3.0-or-later

"""Python bindings for the LGC2 monitoring API (ctypes wrapper around LGC_API).

The flat C ABI (moni* functions) is exported directly from the LGC_API shared
library, so this wrapper loads LGC_API.dll / libLGC_API.so itself -- no separate
binding DLL is required.
"""

import ctypes
import os
import sys
from collections import namedtuple

import numpy as np

# ---------------------------------------------------------------------------
# Load the shared library
# ---------------------------------------------------------------------------

# NOTE: only Windows and Linux are officially supported. macOS has never
# been tested; if LGC_API is ever built on macOS (producing a .dylib), add
# a fallback for that here.
_dll_name = "LGC_API.dll" if sys.platform == "win32" else "libLGC_API.so"
_dll_dir = os.path.dirname(os.path.abspath(__file__))
_dll_path = os.path.join(_dll_dir, _dll_name)

# Make sure the loader can resolve any co-located runtime dependencies
# (e.g. the OpenMP runtime) from the library's own directory on Windows.
if sys.platform == "win32" and hasattr(os, "add_dll_directory"):
    os.add_dll_directory(_dll_dir)
_lib = ctypes.CDLL(_dll_path)

# ---------------------------------------------------------------------------
# ctypes shorthand
# ---------------------------------------------------------------------------

_vp = ctypes.c_void_p
_cp = ctypes.c_char_p
_ip = ctypes.POINTER(ctypes.c_int)
_dp = ctypes.POINTER(ctypes.c_double)
_dpp = ctypes.POINTER(_dp)
_int = ctypes.c_int
_dbl = ctypes.c_double

# ---------------------------------------------------------------------------
# Declare C function signatures
# ---------------------------------------------------------------------------

def _sig(name, restype, argtypes):
    fn = getattr(_lib, name)
    fn.restype = restype
    fn.argtypes = argtypes
    return fn

_get_last_error = _sig("moniGetLastError", _cp, [])
_free_double = _sig("moniFreeDoubleArray", None, [_dp])

_create = _sig("moniCreate", _vp, [_cp])
_destroy = _sig("moniDestroy", None, [_vp])
_reset = _sig("moniReset", _int, [_vp])

_adjust = _sig("moniAdjust", _int, [_vp])
_get_status = _sig("moniGetStatus", _int, [_vp])
_get_sigma0 = _sig("moniGetSigma0", _int, [_vp, _dp])
_write_input = _sig("moniWriteLGCInputFile", _int, [_vp])

_set_activation = _sig("moniSetActivationStatus", _int, [_vp, _cp, _int])
_get_activation = _sig("moniGetActivationStatus", _int, [_vp, _cp])
_update_meas = _sig("moniUpdateMeas", _int, [_vp, _cp, _dp, _int])
_set_obs_sigma = _sig("moniSetObsSigma", _int, [_vp, _cp, _dp, _int])
_get_meas = _sig("moniGetMeas", _int, [_vp, _cp, _dpp, _ip])
_get_residual = _sig("moniGetEstimateResidual", _int, [_vp, _cp, _dpp, _ip])
_get_calc_meas = _sig("moniGetCalcMeas", _int, [_vp, _cp, _dpp, _ip])
_get_obs_sigma = _sig("moniGetObsSigma", _int, [_vp, _cp, _dpp, _ip])

_set_fixed_point = _sig("moniSetFixedPointParameter", _int, [_vp, _cp, _int, _dbl])
_set_fixed_frame = _sig("moniSetFixedFrameParameter", _int, [_vp, _cp, _int, _dbl])
_set_fixed_sag = _sig("moniSetFixedSagParameter", _int, [_vp, _cp, _int, _dbl])
_freeze_point = _sig("moniFreezePointParameter", _int, [_vp, _cp, _int, _dbl])
_freeze_frame = _sig("moniFreezeFrameParameter", _int, [_vp, _cp, _int, _dbl])
_freeze_sag = _sig("moniFreezeSagParameter", _int, [_vp, _cp, _int, _dbl])
_unfreeze_point = _sig("moniUnfreezePointParameter", _int, [_vp, _cp, _int])
_unfreeze_frame = _sig("moniUnfreezeFrameParameter", _int, [_vp, _cp, _int])
_unfreeze_sag = _sig("moniUnfreezeSagParameter", _int, [_vp, _cp, _int])

_get_point_est = _sig("moniGetPointEstimate", _int, [_vp, _cp, _dpp, _ip])
_get_point_est_frame = _sig("moniGetPointEstimateInFrame", _int, [_vp, _cp, _cp, _dpp, _ip])
_get_frame_est = _sig("moniGetFrameEstimate", _int, [_vp, _cp, _dpp, _ip])
_get_sag_est = _sig("moniGetSagEstimate", _int, [_vp, _cp, _dpp, _ip])
_get_point_prec = _sig("moniGetPointEstimatePrec", _int, [_vp, _cp, _dpp, _ip])
_get_point_prec_frame = _sig("moniGetPointEstimatePrecInFrame", _int, [_vp, _cp, _cp, _dpp, _ip])
_get_frame_prec = _sig("moniGetFrameEstimatePrec", _int, [_vp, _cp, _dpp, _ip])
_get_sag_prec = _sig("moniGetSagEstimatePrec", _int, [_vp, _cp, _dpp, _ip])

_transform_coord = _sig("moniTransformCoordinates", _int, [_vp, _dp, _cp, _cp, _dp])
_transform_dir = _sig("moniTransformDirection", _int, [_vp, _dp, _cp, _cp, _dp])

_get_ecws = _sig("moniGetECWSData", _int, [_vp, _cp, _dp, _dp])
_get_ecwi = _sig("moniGetECWIData", _int, [_vp, _cp, _dpp, _ip, _dpp, _ip])

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
    n = _int()
    _check(func(*args, ctypes.byref(data), ctypes.byref(n)))
    try:
        out = np.ctypeslib.as_array(data, shape=(n.value,)).copy()
    finally:
        _free_double(data)
    return out

def _as_c_doubles(values):
    """Convert a Python/NumPy 1-D sequence to a contiguous C double array + length."""
    arr = np.ascontiguousarray(values, dtype=np.float64).ravel()
    return arr.ctypes.data_as(_dp), len(arr)

# ---------------------------------------------------------------------------
# Public types
# ---------------------------------------------------------------------------

WaterRom = namedtuple("WaterRom", ["romName", "estimate", "prec"])
# estimate / prec are length-5 arrays: (Dx, Dz, bearing, slope, sag)
WireRom = namedtuple("WireRom", ["romName", "estimate", "prec"])


class Monitor:
    """Real-time monitoring estimator (wraps the C++ Monitor class)."""

    def __init__(self, inputFilePath):
        self._h = _check_ptr(_create(inputFilePath.encode("utf-8")))

    def __del__(self, _destroy=_destroy):
        if getattr(self, "_h", None):
            _destroy(self._h)
            self._h = None

    def reset(self):
        _check(_reset(self._h))

    # --- Execution ---

    def adjust(self):
        return bool(_check(_adjust(self._h)))

    def getStatus(self):
        return bool(_check(_get_status(self._h)))

    def getSigma0(self):
        out = _dbl()
        _check(_get_sigma0(self._h, ctypes.byref(out)))
        return out.value

    def writeLGCInputFile(self):
        _check(_write_input(self._h))

    # --- Observations ---

    def setActivationStatus(self, obsId, status):
        _check(_set_activation(self._h, obsId.encode("utf-8"), 1 if status else 0))

    def getActivationStatus(self, obsId):
        return bool(_check(_get_activation(self._h, obsId.encode("utf-8"))))

    def updateMeas(self, obsId, measurementVector):
        data, n = _as_c_doubles(measurementVector)
        _check(_update_meas(self._h, obsId.encode("utf-8"), data, n))

    def setObsSigma(self, obsId, sigma):
        data, n = _as_c_doubles(sigma)
        _check(_set_obs_sigma(self._h, obsId.encode("utf-8"), data, n))

    def getMeas(self, obsId):
        return _doubles(_get_meas, self._h, obsId.encode("utf-8"))

    def getEstimateResidual(self, obsName):
        return _doubles(_get_residual, self._h, obsName.encode("utf-8"))

    def getCalcMeas(self, obsName):
        return _doubles(_get_calc_meas, self._h, obsName.encode("utf-8"))

    def getObsSigma(self, obsName):
        return _doubles(_get_obs_sigma, self._h, obsName.encode("utf-8"))

    # --- Parameter control ---

    def setFixedPointParameter(self, pointName, idx, val):
        _check(_set_fixed_point(self._h, pointName.encode("utf-8"), idx, val))

    def setFixedFrameParameter(self, frameName, idx, val):
        _check(_set_fixed_frame(self._h, frameName.encode("utf-8"), idx, val))

    def setFixedSagParameter(self, sagName, idx, val):
        _check(_set_fixed_sag(self._h, sagName.encode("utf-8"), idx, val))

    def freezePointParameter(self, pointName, idx, val):
        _check(_freeze_point(self._h, pointName.encode("utf-8"), idx, val))

    def freezeFrameParameter(self, frameName, idx, val):
        _check(_freeze_frame(self._h, frameName.encode("utf-8"), idx, val))

    def freezeSagParameter(self, sagName, idx, val):
        _check(_freeze_sag(self._h, sagName.encode("utf-8"), idx, val))

    def unfreezePointParameter(self, pointName, idx):
        _check(_unfreeze_point(self._h, pointName.encode("utf-8"), idx))

    def unfreezeFrameParameter(self, frameName, idx):
        _check(_unfreeze_frame(self._h, frameName.encode("utf-8"), idx))

    def unfreezeSagParameter(self, sagName, idx):
        _check(_unfreeze_sag(self._h, sagName.encode("utf-8"), idx))

    # --- Estimation results ---

    def getPointEstimate(self, pointName, frameName=None):
        if frameName is None:
            return _doubles(_get_point_est, self._h, pointName.encode("utf-8"))
        return _doubles(_get_point_est_frame, self._h,
                        pointName.encode("utf-8"), frameName.encode("utf-8"))

    def getPointEstimatePrec(self, pointName, frameName=None):
        if frameName is None:
            return _doubles(_get_point_prec, self._h, pointName.encode("utf-8"))
        return _doubles(_get_point_prec_frame, self._h,
                        pointName.encode("utf-8"), frameName.encode("utf-8"))

    def getFrameEstimate(self, frameName):
        return _doubles(_get_frame_est, self._h, frameName.encode("utf-8"))

    def getFrameEstimatePrec(self, frameName):
        return _doubles(_get_frame_prec, self._h, frameName.encode("utf-8"))

    def getSagEstimate(self, sagName):
        return _doubles(_get_sag_est, self._h, sagName.encode("utf-8"))

    def getSagEstimatePrec(self, sagName):
        return _doubles(_get_sag_prec, self._h, sagName.encode("utf-8"))

    # --- Transformations ---

    def transformCoordinates(self, coordinateVector, frm, to):
        in3 = (ctypes.c_double * 3)(*coordinateVector)
        out3 = (ctypes.c_double * 3)()
        _check(_transform_coord(self._h, in3, frm.encode("utf-8"), to.encode("utf-8"), out3))
        return np.array(out3, dtype=np.float64)

    def transformDirection(self, directionVector, frm, to):
        in3 = (ctypes.c_double * 3)(*directionVector)
        out3 = (ctypes.c_double * 3)()
        _check(_transform_dir(self._h, in3, frm.encode("utf-8"), to.encode("utf-8"), out3))
        return np.array(out3, dtype=np.float64)

    # --- Round-of-measurement results ---

    def getECWSData(self, ecwsRomName):
        est = _dbl()
        prec = _dbl()
        _check(_get_ecws(self._h, ecwsRomName.encode("utf-8"),
                         ctypes.byref(est), ctypes.byref(prec)))
        return WaterRom(ecwsRomName, est.value, prec.value)

    def getECWIData(self, ecwiRomName):
        est = _dp()
        est_n = _int()
        prec = _dp()
        prec_n = _int()
        _check(_get_ecwi(self._h, ecwiRomName.encode("utf-8"),
                         ctypes.byref(est), ctypes.byref(est_n),
                         ctypes.byref(prec), ctypes.byref(prec_n)))
        try:
            estimate = np.ctypeslib.as_array(est, shape=(est_n.value,)).copy()
            precision = np.ctypeslib.as_array(prec, shape=(prec_n.value,)).copy()
        finally:
            _free_double(est)
            _free_double(prec)
        return WireRom(ecwiRomName, estimate, precision)
