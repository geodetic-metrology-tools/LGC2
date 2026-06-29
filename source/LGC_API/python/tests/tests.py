# SPDX-FileCopyrightText: CERN
#
# SPDX-License-Identifier: GPL-3.0-or-later

"""Test suite for the pyMonitoring monitoring bindings.

Two layers:
  * Smoke / shape tests on a realistic project file (exercise the call surface
    and check vector shapes / qualitative behaviour).
  * Deterministic value tests mirroring the C++ LGC_API tests (testAPI.cpp),
    using the same minimal fixtures with known closed-form expected results.
"""

import math

import numpy as np
import pytest
import pyMonitoring

# Realistic project file (smoke / shape layer)
LGC_FRAS = "LGC_Fras_2025_04_08.lgc"
POINT = "BEAM_SCT.GTAP.A1LX.E"
FRAME = "RSTB_SCT.GTAP.A1LX"
OBS = "GTAP.A1LX.B_INT"

# Minimal deterministic fixtures (value layer, shared with the C++ tests)
MINIMAL = "minimalTest.lgc2"
FIXEDPARAMS = "manipulateFixedParameters.lgc2"
SAG = "minimalSag.lgc"
STRING = "vivienString.lgc"


# ===========================================================================
# Smoke / shape tests (realistic project file)
# ===========================================================================

@pytest.fixture
def moni():
    return pyMonitoring.Monitoring(LGC_FRAS)


def test_load_nonexistent_file():
    with pytest.raises(RuntimeError):
        pyMonitoring.Monitoring("does_not_exist.lgc")


def test_adjust_succeeds(moni):
    assert moni.adjust() is True
    assert moni.getStatus() is True


def test_sigma0_positive_after_adjust(moni):
    moni.adjust()
    assert moni.getSigma0() > 0.0


def test_point_estimate_shape(moni):
    moni.adjust()
    est = moni.getPointEstimate(POINT)
    prec = moni.getPointEstimatePrec(POINT)
    assert len(est) == 3
    assert len(prec) == len(est)


def test_point_estimate_in_frame(moni):
    moni.adjust()
    est = moni.getPointEstimate(POINT)
    est_frame = moni.getPointEstimate(POINT, FRAME)
    assert len(est_frame) == 3
    # the in-frame coordinates differ from the global ones
    assert np.linalg.norm(est - est_frame) > 0.0


def test_frame_estimate(moni):
    moni.adjust()
    est = moni.getFrameEstimate(FRAME)
    assert len(est) > 0
    assert len(moni.getFrameEstimatePrec(FRAME)) == len(est)


def test_get_meas(moni):
    moni.adjust()
    meas = moni.getMeas(OBS)
    assert len(meas) > 0


def test_update_meas_changes_sigma0(moni):
    moni.adjust()
    sigma0_before = moni.getSigma0()
    meas = moni.getMeas(OBS)
    moni.updateMeas(OBS, meas + 1e-3)
    moni.adjust()
    # a perturbed observation should not give exactly the same fit
    assert moni.getSigma0() != sigma0_before


def test_calc_meas_equals_meas_plus_residual(moni):
    moni.adjust()
    meas = moni.getMeas(OBS)
    residual = moni.getEstimateResidual(OBS)
    calc = moni.getCalcMeas(OBS)
    assert np.allclose(calc, meas + residual)


def test_activation_round_trip(moni):
    moni.adjust()
    assert moni.getActivationStatus(OBS) is True
    moni.setActivationStatus(OBS, False)
    assert moni.getActivationStatus(OBS) is False
    moni.adjust()
    # a deactivated observation has zero residual
    assert np.allclose(moni.getEstimateResidual(OBS), 0.0)


def test_reset_recovers_after_blunder(moni):
    moni.adjust()
    obs_sigma = moni.getObsSigma(OBS)
    moni.updateMeas(OBS, np.full(len(obs_sigma), 1e6))
    try:
        moni.adjust()
    except RuntimeError:
        pass
    moni.reset()
    assert moni.adjust() is True


# ===========================================================================
# Deterministic value tests (mirror of source/LGC_Tests/sources/testAPI.cpp)
# ===========================================================================

def test_simple_api_use():
    """Mirror of testAPI test<1>: estimates, (de)activation, updateMeas,
    calcMeas, frame precisions and setObsSigma on minimalTest.lgc2."""
    m = pyMonitoring.Monitoring(MINIMAL)
    m.adjust()

    sigma_before = m.getSigma0()
    assert np.allclose(m.getFrameEstimate("testFrame"), [1, 2, 3, 0, 0, 0, 1], atol=1e-12)
    assert np.allclose(m.getPointEstimate("P2"), [0, 0, 0], atol=1e-12)

    # deactivate one of P2's two observations -> P2 fits the remaining one exactly
    m.setActivationStatus("testObs2", False)
    m.adjust()
    sigma_after_deact = m.getSigma0()
    assert np.allclose(m.getPointEstimate("P2"), [1, 1, 1], atol=1e-8)

    # reactivate -> back to the balanced solution
    m.setActivationStatus("testObs2", True)
    m.adjust()
    sigma_after_react = m.getSigma0()
    assert np.allclose(m.getPointEstimate("P2"), [0, 0, 0], atol=1e-8)

    assert abs(sigma_before - sigma_after_react) < 1e-6
    # the single remaining observation can be fitted with zero residual
    assert abs(sigma_after_deact) < 1e-9

    # change the OBSXYZ observation and re-estimate
    m.updateMeas("testObs1", np.array([11.0, 13.0, 19.0]))
    m.adjust()
    assert np.allclose(m.getFrameEstimate("testFrame"), [11, 13, 19, 0, 0, 0, 1], atol=1e-12)

    # both OBSXYZ observations of P2 must produce the same calculated measurement
    assert np.allclose(m.getCalcMeas("testObs2"), m.getCalcMeas("testObs3"), atol=1e-12)

    # frame translation precisions come from the OBSXYZ precisions
    assert np.allclose(m.getFrameEstimatePrec("testFrame"),
                       [0.001, 0.001, 0.001, 0, 0, 0, 0], atol=1e-12)

    # on-the-fly change of OBSXYZ precisions, then check it propagates
    new_sigma = np.array([0.12345, 0.23456, 0.34567])
    m.setObsSigma("testObs1", new_sigma)
    assert np.allclose(m.getObsSigma("testObs1"), new_sigma, atol=1e-12)
    m.adjust()
    assert np.allclose(m.getFrameEstimatePrec("testFrame"),
                       [*new_sigma, 0, 0, 0, 0], atol=1e-12)


def test_manipulate_fixed_parameters():
    """Mirror of testAPI test<2>: setFixedFrameParameter / setFixedPointParameter."""
    m = pyMonitoring.Monitoring(FIXEDPARAMS)
    m.adjust()
    assert np.allclose(m.getFrameEstimate("testFrame"), [0, 0, 0, 0, 0, 0, 1], atol=1e-12)

    for j in range(7):
        m.setFixedFrameParameter("testFrame", j, 0.1 * j)
        m.adjust()
        assert abs(m.getFrameEstimate("testFrame")[j] - 0.1 * j) < 1e-12

    for j in range(3):
        m.setFixedPointParameter("P2", j, float(j))
        m.adjust()
        assert abs(m.getPointEstimate("P2")[j] - j) < 1e-12


def test_freeze_frame_parameter():
    """Mirror of testAPI test<3>: freeze / unfreeze frame parameters + errors."""
    m = pyMonitoring.Monitoring(MINIMAL)
    m.adjust()
    assert np.allclose(m.getFrameEstimate("testFrame"), [1, 2, 3, 0, 0, 0, 1], atol=1e-12)

    m.freezeFrameParameter("testFrame", 0, 5.0)
    m.adjust()
    assert abs(m.getFrameEstimate("testFrame")[0] - 5.0) < 1e-12

    m.unfreezeFrameParameter("testFrame", 0)
    m.adjust()
    assert abs(m.getFrameEstimate("testFrame")[0] - 1.0) < 1e-12

    # freezing a fixed variable is an error
    with pytest.raises(RuntimeError):
        m.freezeFrameParameter("testFrame", 3, 1.0)

    # unfreezing a variable that is not frozen is an error
    with pytest.raises(RuntimeError):
        m.unfreezeFrameParameter("testFrame", 4)


def test_freeze_point_parameter():
    """Mirror of testAPI test<4>: freeze / unfreeze point parameters + precision + errors."""
    m = pyMonitoring.Monitoring(MINIMAL)
    m.adjust()
    assert np.allclose(m.getPointEstimate("P2"), [0, 0, 0], atol=1e-12)

    m.freezePointParameter("P2", 0, 5.0)
    m.adjust()
    assert abs(m.getPointEstimate("P2")[0] - 5.0) < 1e-12

    prec = m.getPointEstimatePrec("P2")
    assert prec[0] == 0.0          # frozen component -> zero precision
    assert prec[1] != 0.0
    assert prec[2] != 0.0

    m.unfreezePointParameter("P2", 0)
    m.adjust()
    assert abs(m.getPointEstimate("P2")[0]) < 1e-8

    # freezing a fixed (CALA) point is an error
    with pytest.raises(RuntimeError):
        m.freezePointParameter("P1", 1, 1.0)

    # unfreezing a variable that is no longer frozen is an error
    with pytest.raises(RuntimeError):
        m.unfreezePointParameter("P2", 0)


def test_sag_parameters():
    """Mirror of testAPI test<8>: sag estimate/precision, freeze and setFixed."""
    m = pyMonitoring.Monitoring(SAG)
    m.adjust()
    assert np.allclose(m.getPointEstimate("left"), [0.007, -1.0, 0.003], atol=1e-9)
    assert np.allclose(m.getPointEstimate("middle"), [0.003, 0.0, 0.001], atol=1e-9)
    assert np.allclose(m.getPointEstimate("right"), [0.007, 1.0, 0.003], atol=1e-9)
    # sag1 parameters: VS, VC, RS, RC
    assert np.allclose(m.getSagEstimate("sag1"), [0.001, 0.002, 0.003, 0.004], atol=1e-9)

    # freeze the vertical curvature (index 1) to zero
    m.freezeSagParameter("sag1", 1, 0.0)
    m.adjust()
    left = m.getPointEstimate("left")
    middle = m.getPointEstimate("middle")
    right = m.getPointEstimate("right")
    assert abs(left[2] - middle[2]) < 1e-9
    assert abs(left[2] - right[2]) < 1e-9
    assert abs(m.getSagEstimate("sag1")[1]) < 1e-12

    # equal-height observations -> vertical sag estimated at 0 with recognizable precision
    m.updateMeas("leftObs", np.array([0.007, -1.0, 0.0]))
    m.updateMeas("middleObs", np.array([0.003, 0.0, 0.0]))
    m.updateMeas("rightObs", np.array([0.007, 1.0, 0.0]))
    m.adjust()
    assert abs(m.getSagEstimate("sag1")[0]) < 1e-9
    # determined by 3 z-observations each with precision 0.1234mm -> 0.0001234m / sqrt(3)
    assert abs(m.getSagEstimatePrec("sag1")[0] - 0.0001234 / math.sqrt(3)) < 1e-9

    # setFixed on the second sag element shifts the dependent points deterministically
    assert np.allclose(m.getPointEstimate("P1"), [0, 0, 0], atol=1e-9)
    assert np.allclose(m.getPointEstimate("P2"), [0, 1, 0], atol=1e-9)
    m.setFixedSagParameter("sag2", 0, 1.0)
    m.setFixedSagParameter("sag2", 1, 2.0)
    m.setFixedSagParameter("sag2", 2, 3.0)
    m.setFixedSagParameter("sag2", 3, 4.0)
    m.adjust()
    assert np.allclose(m.getPointEstimate("P1"), [3, 0, 1], atol=1e-9)
    assert np.allclose(m.getPointEstimate("P2"), [7, 1, 3], atol=1e-9)


def test_transformations():
    """Mirror of testAPI test<10>: forward/backward transform invariance."""
    m = pyMonitoring.Monitoring(STRING)
    frm = "RSTR_STRING.B1M.LQXFE.1SF"
    to = "RSTR_STRING.B1M.LQXFG.2SF"

    coord = np.array([1.0, 2.0, 3.0])
    fwd = m.transformCoordinates(coord, frm, to)
    back = m.transformCoordinates(fwd, to, frm)
    assert np.linalg.norm(coord - back) < 1e-8

    direction = np.array([1.0, 2.0, 3.0])
    fwd_d = m.transformDirection(direction, frm, to)
    back_d = m.transformDirection(fwd_d, to, frm)
    assert np.linalg.norm(direction - back_d) < 1e-8


def test_reset_after_failure():
    """Mirror of testAPI test<11>: a blunder makes adjust() raise; reset() recovers."""
    m = pyMonitoring.Monitoring(STRING)
    m.updateMeas("LQXFE.1SF.Q_WPS", np.array([10.0, 10.0]))
    with pytest.raises(RuntimeError):
        m.adjust()
    m.reset()
    assert m.adjust() is True
    sigma0 = m.getSigma0()
    assert 0.5 < sigma0 < 2.0


def test_disable_wire_sensor():
    """Mirror of testAPI test<12>: deactivating a wire sensor zeroes its residual."""
    m = pyMonitoring.Monitoring(STRING)
    m.adjust()
    wire_id = "LQXFE.1SF.Q_WPS"
    assert np.linalg.norm(m.getEstimateResidual(wire_id)) > 1e-12
    assert m.getActivationStatus(wire_id) is True

    m.setActivationStatus(wire_id, False)
    assert m.getActivationStatus(wire_id) is False
    m.adjust()
    assert np.linalg.norm(m.getEstimateResidual(wire_id)) < 1e-12
