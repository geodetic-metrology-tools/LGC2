"""Test suite for pyLGC bindings."""

import numpy as np
from numpy.linalg import norm
import pytest
import pyLGC

LGC_FILE = "Title-Example.lgc2"
BROKEN_FILE = "Title-Example_broken.lgc2"


@pytest.fixture
def ev():
    return pyLGC.Evaluator(LGC_FILE)


# --- Loading ---

def test_load_dimensions(ev):
    idx = ev.getIndices()
    assert idx.UIndex > 0
    assert idx.EIndex > 0
    assert idx.OIndex > 0

def test_get_params_before_evaluate(ev):
    """getEstParams returns provisional values without needing evaluate()."""
    params = ev.getEstParams()
    assert len(params) == ev.getIndices().UIndex
    assert norm(params) > 0  # provisionals should be non-zero

def test_load_broken_file():
    with pytest.raises(RuntimeError, match="Errors found"):
        pyLGC.Evaluator(BROKEN_FILE)

def test_load_nonexistent_file():
    with pytest.raises(RuntimeError, match="Cannot open"):
        pyLGC.Evaluator("does_not_exist.lgc2")


# --- Parameter set/get ---

def test_set_get_round_trip(ev):
    ev.evaluate()
    params = ev.getEstParams()
    params[0] += 42.0
    ev.setParameters(params)
    assert abs(ev.getEstParams()[0] - params[0]) < 1e-10

def test_set_get_restore(ev):
    ev.evaluate()
    original = ev.getEstParams()
    ev.setParameters(original + 1.0)
    ev.setParameters(original)
    assert norm(original - ev.getEstParams(), np.inf) < 1e-10

def test_set_wrong_dimension(ev):
    with pytest.raises(RuntimeError, match="wrong dimension"):
        ev.setParameters([1.0, 2.0, 3.0])

def test_random_set_get_round_trip(ev):
    rng = np.random.default_rng(42)
    n = ev.getIndices().UIndex
    for _ in range(20):
        params_in = rng.uniform(-np.pi, np.pi, size=n)
        ev.setParameters(params_in)
        assert norm(params_in - ev.getEstParams(), np.inf) < 1e-10


# --- Evaluate ---

def test_evaluate_without_set_uses_provisionals(ev):
    ev.evaluate()
    w = ev.getMisclosure()
    A = ev.getAMatrix()
    assert len(w) > 0
    assert len(A[2]) > 0

def test_evaluate_changes_misclosure(ev):
    ev.evaluate()
    w_before = ev.getMisclosure()
    params = ev.getEstParams()
    params[0] += 1.0
    ev.setParameters(params)
    ev.evaluate()
    assert norm(w_before - ev.getMisclosure(), np.inf) > 1e-15

def test_evaluate_is_idempotent(ev):
    ev.evaluate()
    w1 = ev.getMisclosure()
    ev.evaluate()
    assert norm(w1 - ev.getMisclosure()) == 0.0

def test_multiple_set_evaluate_cycles(ev):
    n = ev.getIndices().UIndex
    for i in range(5):
        ev.setParameters([float(i) * 0.001] * n)
        ev.evaluate()
        ev.getMisclosure()


# --- Getters without evaluate raise ---

@pytest.mark.parametrize("getter", [
    "getMisclosure", "getAMatrix", "getBMatrix",
    "getInvBMatrix", "getPMatrix", "getA2Matrix",
    "getConstraintMisclosure",
])
def test_getter_without_evaluate_raises(ev, getter):
    with pytest.raises(RuntimeError, match="evaluate"):
        getattr(ev, getter)()

@pytest.mark.parametrize("getter", ["getMisclosure", "getAMatrix"])
def test_getter_after_set_without_evaluate_raises(ev, getter):
    ev.setParameters([0.0] * ev.getIndices().UIndex)
    with pytest.raises(RuntimeError, match="evaluate"):
        getattr(ev, getter)()


# --- Matrix/vector shapes ---

def test_matrix_shapes(ev):
    ev.evaluate()
    idx = ev.getIndices()

    A = ev.getAMatrix()
    assert (A[3], A[4]) == (idx.EIndex, idx.UIndex)
    assert len(A[2]) > 0

    B = ev.getBMatrix()
    assert (B[3], B[4]) == (idx.EIndex, idx.OIndex)
    assert len(B[2]) > 0

    invB = ev.getInvBMatrix()
    assert (invB[3], invB[4]) == (idx.OIndex, idx.EIndex)
    assert len(invB[2]) > 0

    P = ev.getPMatrix()
    assert (P[3], P[4]) == (idx.OIndex, idx.OIndex)
    assert len(P[2]) > 0

    A2 = ev.getA2Matrix()
    assert (A2[3], A2[4]) == (idx.CIndex, idx.UIndex)

def test_vector_lengths(ev):
    ev.evaluate()
    idx = ev.getIndices()
    assert len(ev.getMisclosure()) == idx.EIndex
    assert len(ev.getConstraintMisclosure()) == idx.CIndex


# --- Point and frame lookup ---

def test_point_lookup(ev):
    pt = ev.getPoint("H4.XBPF.22716.E")
    assert pt.getName() == "H4.XBPF.22716.E"
    assert len(pt.getRelativeUnknIndices()) == 3
    assert pt.getFirstUidx() >= 0

def test_frame_lookup(ev):
    frame = ev.getFrame("testFrame")
    assert frame.getName() == "testFrame"
    assert len(frame.getRelativeUnknIndices()) == 4
    assert frame.getFirstUidx() >= 0

def test_root_frame_is_fixed(ev):
    root = ev.getFrame("ROOT")
    assert len(root.getRelativeUnknIndices()) == 0

def test_invalid_point_raises(ev):
    with pytest.raises(RuntimeError, match="not found"):
        ev.getPoint("DOES_NOT_EXIST")

def test_invalid_frame_raises(ev):
    with pytest.raises(RuntimeError, match="not found"):
        ev.getFrame("DOES_NOT_EXIST")


# --- Obs index mapping ---

def test_obs_index_to_line_number(ev):
    mapping = ev.getObsIndexToLineNumber()
    assert len(mapping) > 0

def test_obs_line_numbers_monotonic(ev):
    """Consecutive obs indices map to non-decreasing line numbers.

    This holds for Title-Example.lgc2 where observations appear in order;
    it may not hold for all input files in general.
    """
    mapping = ev.getObsIndexToLineNumber()
    keys = sorted(mapping.keys())
    for i in range(len(keys) - 1):
        line_curr = mapping[keys[i]]
        line_next = mapping[keys[i + 1]]
        assert line_next >= line_curr


# --- tryLGCSolve ---

def test_solve(ev):
    ok, solution = ev.tryLGCSolve()
    assert ok
    assert len(solution) == ev.getIndices().UIndex

def test_getter_after_solve_requires_evaluate(ev):
    ev.tryLGCSolve()
    with pytest.raises(RuntimeError, match="evaluate"):
        ev.getAMatrix()
    ev.evaluate()
    A = ev.getAMatrix()
    assert len(A[2]) > 0

def test_solve_then_set_solution_then_evaluate(ev):
    ok, sol = ev.tryLGCSolve()
    ev.setParameters(sol)
    ev.evaluate()
    assert len(ev.getMisclosure()) > 0
    assert len(ev.getAMatrix()[2]) > 0

def test_solve_twice_no_reset(ev):
    ok1, sol1 = ev.tryLGCSolve()
    ok2, sol2 = ev.tryLGCSolve()
    assert ok1 and ok2
    assert norm(sol1 - sol2, np.inf) < 1.0

def test_solve_twice_with_reset(ev):
    params_initial = ev.getEstParams()
    _, sol1 = ev.tryLGCSolve()
    ev.setParameters(params_initial)
    _, sol2 = ev.tryLGCSolve()
    assert norm(sol1 - sol2, np.inf) < 1e-8

def test_point_frame_after_solve(ev):
    ev.tryLGCSolve()
    assert ev.getPoint("H4.XBPF.22716.E").getName() == "H4.XBPF.22716.E"
    assert ev.getFrame("testFrame").getName() == "testFrame"

def test_perturb_after_solve(ev):
    _, sol = ev.tryLGCSolve()
    sol[0] += 10.0
    ev.setParameters(sol)
    ev.evaluate()
    assert norm(ev.getMisclosure(), np.inf) > 1e-10


# --- getEstVector ---

def test_getEstVector_free_point(ev):
    """Fully free point: setParameters propagates through to the point's getEstVector."""
    pt = ev.getPoint("H4.XBPF.22716.E")
    assert len(pt.getRelativeUnknIndices()) == 3

    est_before = pt.getEstVector()
    assert len(est_before) == 3

    params = ev.getEstParams()
    uidx = pt.getFirstUidx()
    delta = 100.0
    params[uidx:uidx+3] += delta
    ev.setParameters(params)
    assert norm(pt.getEstVector() - est_before - delta, np.inf) < 1e-10


def test_getEstVector_partial_point(ev):
    """Partially free point (Z fixed): only X,Y change after setParameters."""
    pt = ev.getPoint("partialPt")
    assert len(pt.getRelativeUnknIndices()) == 2  # X and Y free, Z fixed

    est_before = pt.getEstVector()
    assert len(est_before) == 3

    params = ev.getEstParams()
    uidx = pt.getFirstUidx()
    delta = 50.0
    params[uidx:uidx+2] += delta
    ev.setParameters(params)

    est_after = pt.getEstVector()
    # X and Y should change by delta
    assert norm(est_after[:2] - est_before[:2] - delta, np.inf) < 1e-10
    # Z should not change
    assert abs(est_after[2] - est_before[2]) < 1e-10


def test_getEstVector_fixed_point(ev):
    """Fully fixed (CALA) point: getEstVector returns values, none change after setParameters."""
    pt = ev.getPoint("EHN1.ST.H4-4.")
    assert len(pt.getRelativeUnknIndices()) == 0

    est_before = pt.getEstVector()
    assert len(est_before) == 3

    ev.setParameters(ev.getEstParams() + 99.0)

    assert norm(pt.getEstVector() - est_before, np.inf) < 1e-10


# --- B matrix is negative identity ---

def test_B_is_negative_identity(ev):
    """B matrix should be -I for parametric observation models."""
    ev.evaluate()
    rows, cols, vals, nr, nc = ev.getBMatrix()
    assert nr == nc  # square
    # all entries on diagonal
    assert np.array_equal(rows, cols)
    # all values are -1
    assert np.allclose(vals, -1.0)
    # one entry per row
    assert len(vals) == nr


# --- Solution quality ---

def test_solve_reduces_misclosure(ev):
    """Solving should reduce the misclosure significantly.

    The provisional coordinates in Title-Example.lgc2 are intentionally
    perturbed so that the initial misclosure is large and the reduction
    after solving is clearly visible.
    """
    ev.evaluate()
    w_before = norm(ev.getMisclosure())
    ok, sol = ev.tryLGCSolve()
    assert ok
    ev.setParameters(sol)
    ev.evaluate()
    w_after = norm(ev.getMisclosure())
    assert w_after / w_before < 1e-5


# --- Derivative verification ---

def test_finite_difference_matches_analytic_A(ev):
    """Finite-difference Jacobian should agree with the analytic A matrix."""
    ev.evaluate()
    A_fd = ev.getFiniteDifferenceA(1e-7)
    rows, cols, vals, nr, nc = ev.getAMatrix()
    A_analytic = np.zeros((nr, nc))
    A_analytic[rows, cols] = vals
    assert norm(A_fd - A_analytic, np.inf) < 0.1


# --- Frame getEstVector ---

def test_getEstVector_free_frame(ev):
    """Free frame: setParameters propagates through to the frame's getEstVector."""
    frame = ev.getFrame("testFrame")
    rel = frame.getRelativeUnknIndices()
    assert len(rel) == 4

    est_before = frame.getEstVector()
    params = ev.getEstParams()
    uidx = frame.getFirstUidx()
    delta = 0.01
    params[uidx:uidx+len(rel)] += delta
    ev.setParameters(params)

    est_after = frame.getEstVector()
    # free components should change by delta
    assert norm(est_after[rel] - est_before[rel] - delta, np.inf) < 1e-10

def test_getEstVector_fixed_frame(ev):
    """Fixed frame (ROOT): getEstVector unchanged after setParameters."""
    root = ev.getFrame("ROOT")
    assert len(root.getRelativeUnknIndices()) == 0
    est_before = root.getEstVector()
    ev.setParameters(ev.getEstParams() + 99.0)
    assert norm(root.getEstVector() - est_before, np.inf) < 1e-10
