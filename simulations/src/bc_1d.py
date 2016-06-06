from __future__ import division, print_function
import numpy as np


def _x0_d0(f_t):
    def bc(d_mat, u_vect, t):
        d_mat[:2] = np.zeros_like(d_mat[:2])
        d_mat[0, 0] = 1
        d_mat[1, 1] = 1
        u_vect[0] = 1
        u_vect[1] = f_t(t)
        return d_mat, u_vect, t
    return bc


def _x0_d1(g_t, dx):
    def bc(d_mat, u_vect, t):
        d_mat[0] = np.zeros_like(d_mat[0])
        d_mat[0, 0] = 1
        u_vect[0] = u_vect[2] - 2 * dx * g_t(t)
        return d_mat, u_vect, t
    return bc


def _x1_d0(f_t):
    def bc(d_mat, u_vect, t):
        d_mat[-2:] = np.zeros_like(d_mat[-2:])
        d_mat[-2, -2] = 1
        d_mat[-1, -1] = 1
        u_vect[-2] = f_t(t)
        u_vect[-1] = 1
        return d_mat, u_vect, t
    return bc


def _x1_d1(g_t, dx):
    def bc(d_mat, u_vect, t):
        d_mat[-1] = np.zeros_like(d_mat[-1])
        d_mat[-1, -1] = 1
        u_vect[-1] = u_vect[-3] + 2 * dx * g_t(t)
        return d_mat, u_vect, t
    return bc


class BoundaryConditions:
    def __init__(self, x0_func, x1_func, x0_order=1, x1_order=1, name=None):
        self._x0_func = x0_func
        self._x1_func = x1_func
        self.x0_order = x0_order if self._x0_func is not None else None
        self.x1_order = x1_order if self._x1_func is not None else None
        self.name = name

    def __call__(self, *args, **kwargs):
        if self._x0_func is not None and self._x1_func is not None:
            return self._x1_func(*self._x0_func(*args, **kwargs))
        elif self._x0_func is not None:
            return self._x0_func(*args, **kwargs)
        elif self._x1_func is not None:
            return self._x1_func(*args, **kwargs)
        else:
            return None


def get_bc_dirichlet(x0, x1):
    return BoundaryConditions(
        x0_func=_x0_d0(f_t=lambda t: x0),
        x1_func=_x1_d0(f_t=lambda t: x1),
        x0_order=1, x1_order=1,
        name='Dirichlet',
    )


def get_bc_neumann(x0, x1, dx):
    return BoundaryConditions(
        x0_func=_x0_d1(g_t=lambda t: x0, dx=dx),
        x1_func=_x1_d1(g_t=lambda t: x1, dx=dx),
        x0_order=2, x1_order=2,
        name='Neumann',
    )


def get_bc_mixed1(x0, x1, dx):
    return BoundaryConditions(
        x0_func=_x0_d0(f_t=lambda t: x0),
        x1_func=_x1_d1(g_t=lambda t: x1, dx=dx),
        x0_order=1, x1_order=2,
        name='Mixed I',
    )


def get_bc_mixed2(x0, x1, dx):
    return BoundaryConditions(
        x0_func=_x0_d1(g_t=lambda t: x0, dx=dx),
        x1_func=_x1_d0(f_t=lambda t: x1),
        x0_order=2, x1_order=1,
        name='Mixed II',
    )


def get_bc_time_dirichlet(x0_func, x1_func):
    return BoundaryConditions(
        x0_func=_x0_d0(f_t=x0_func),
        x1_func=_x1_d0(f_t=x1_func),
        x0_order=1, x1_order=1,
        name='Time-dependent Dirichlet',
    )


def get_bc_time_neumann(x0_func, x1_func, dx):
    return BoundaryConditions(
        x0_func=_x0_d1(g_t=x0_func, dx=dx),
        x1_func=_x1_d1(g_t=x1_func, dx=dx),
        x0_order=2, x1_order=2,
        name='Time-dependent Neumann',
    )


def get_bc_time_mixed1(x0_func, x1_func, dx):
    return BoundaryConditions(
        x0_func=_x0_d0(f_t=x0_func),
        x1_func=_x1_d1(g_t=x1_func, dx=dx),
        x0_order=1, x1_order=2,
        name='Time-dependent Mixed I',
    )


def get_bc_time_mixed2(x0_func, x1_func, dx):
    return BoundaryConditions(
        x0_func=_x0_d1(g_t=x0_func, dx=dx),
        x1_func=_x1_d0(f_t=x1_func),
        x0_order=2, x1_order=1,
        name='Time-dependent Mixed II',
    )
