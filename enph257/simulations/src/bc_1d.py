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


def x0_d0_discontinuous(f_t, t_stop):
    def bc(d_mat, u_vect, t):
        d_mat[:2] = np.zeros_like(d_mat[:2])
        d_mat[0, 0] = 1
        d_mat[1, 1] = 1
        u_vect[0] = 1
        if t < t_stop:
            u_vect[1] = f_t(t)
        return d_mat, u_vect, t
    return bc


def x0_d1_discontinuous(g_t, dx, t_stop):
    def bc(d_mat, u_vect, t):
        d_mat[0] = np.zeros_like(d_mat[0])
        d_mat[0, 0] = 1
        if t < t_stop:
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


def simple_heat_diffusion_x0(
        power_fn, dt, dx, denom, k_c, area, perimeter, u_amb):
    def bc(d_mat, u_vect, t):
        diff = dt*k_c/denom * (area/(2*dx) - perimeter/2)
        d_mat[0, 0] = 1
        d_mat[1, 1] += diff
        u_vect[0] = 0
        u_vect[1] += power_fn(t)*(dt/dx)/denom + diff * u_amb[1]
        return d_mat, u_vect, t
    return bc


def simple_heat_diffusion_x1(
        dt, dx, denom, k_c, area, perimeter, u_amb):
    def bc(d_mat, u_vect, t):
        diff = dt*k_c/denom * (area/(2*dx) - perimeter/2)
        d_mat[-1, -1] = 1
        d_mat[-2, -2] += diff
        u_vect[-1] = 0
        u_vect[-2] += diff * u_amb[-2]
        return d_mat, u_vect, t
    return bc


def simple_heat_diffusion2_x0(
        power_fn, dt, dx, denom, k_c, area, perimeter, u_prev, u_amb):
    def bc(d_mat, u_vect, t):
        diff = (dt*k_c/denom * (area/(2*dx) - perimeter/2) *
                (u_prev[1] - u_amb[1]))
        d_mat[0, 0] = 1
        u_vect[0] = 0
        u_vect[1] += power_fn(t)*(dt/dx)/denom + diff
        return d_mat, u_vect, t
    return bc


def simple_heat_diffusion2_x1(
        dt, dx, denom, k_c, area, perimeter, u_prev, u_amb):
    def bc(d_mat, u_vect, t):
        diff = (dt*k_c/denom * (area/(2*dx) - perimeter/2) *
                (u_prev[-2] - u_amb[-2]))
        d_mat[-1, -1] = 1
        u_vect[-1] = 0
        u_vect[-2] += diff
        return d_mat, u_vect, t
    return bc


class BoundaryConditions:
    def __init__(self, x0_func, x1_func, x0_order=1, x1_order=1, name=None):
        self._x0_func = x0_func
        self._x1_func = x1_func
        self.x0_order = x0_order if self._x0_func is not None else None
        self.x1_order = x1_order if self._x1_func is not None else None
        self.name = name

    def __call__(self, *args):
        if self._x0_func is not None and self._x1_func is not None:
            return self._x1_func(*self._x0_func(*args))
        elif self._x0_func is not None:
            return self._x0_func(*args)
        elif self._x1_func is not None:
            return self._x1_func(*args)
        else:
            return None


def get_bc_simple_heat_diffusion(
        power_fn, dt, dx, denom, k_c, area, perimeter, u_amb):
    fx0 = simple_heat_diffusion_x0(
        power_fn=power_fn, dt=dt, dx=dx, denom=denom, k_c=k_c,
        area=area, perimeter=perimeter, u_amb=u_amb,
    )
    fx1 = simple_heat_diffusion_x1(
        dt=dt, dx=dx, denom=denom, k_c=k_c,
        area=area, perimeter=perimeter, u_amb=u_amb,
    )
    return BoundaryConditions(
        x0_func=fx0, x1_func=fx1,
        name='Simple heat-diffusion BC'
    )


def get_bc_simple_heat_diffusion2(
        power_fn, dt, dx, denom, k_c, area, perimeter, u_prev, u_amb):
    fx0 = simple_heat_diffusion2_x0(
        power_fn=power_fn, dt=dt, dx=dx, denom=denom, k_c=k_c,
        area=area, perimeter=perimeter, u_prev=u_prev, u_amb=u_amb,
    )
    fx1 = simple_heat_diffusion2_x1(
        dt=dt, dx=dx, denom=denom, k_c=k_c,
        area=area, perimeter=perimeter, u_prev=u_prev, u_amb=u_amb,
    )
    return BoundaryConditions(
        x0_func=fx0, x1_func=fx1,
        name='Simple heat-diffusion BC'
    )


def identity():
    def _identity(*args):
        return args
    return BoundaryConditions(x0_func=_identity(), x1_func=_identity())


def get_bc_dirichlet(x0, x1):
    fx0 = _x0_d0(f_t=lambda t: x0) if x0 is not None else None
    fx1 = _x1_d0(f_t=lambda t: x1) if x1 is not None else None
    return BoundaryConditions(
        x0_func=fx0, x1_func=fx1, x0_order=1, x1_order=1,
        name='Dirichlet',
    )


def get_bc_neumann(x0, x1, dx):
    fx0 = _x0_d1(g_t=lambda t: x0, dx=dx) if x0 is not None else None
    fx1 = _x1_d1(g_t=lambda t: x1, dx=dx) if x1 is not None else None
    return BoundaryConditions(
        x0_func=fx0, x1_func=fx1, x0_order=2, x1_order=2,
        name='Neumann',
    )


def get_bc_mixed1(x0, x1, dx):
    fx0 = _x0_d0(f_t=lambda t: x0) if x0 is not None else None
    fx1 = _x1_d1(g_t=lambda t: x1, dx=dx) if x1 is not None else None
    return BoundaryConditions(
        x0_func=fx0, x1_func=fx1, x0_order=1, x1_order=2,
        name='Mixed I',
    )


def get_bc_mixed2(x0, x1, dx):
    fx0 = _x0_d1(g_t=lambda t: x0, dx=dx) if x0 is not None else None
    fx1 = _x1_d0(f_t=lambda t: x1) if x1 is not None else None
    return BoundaryConditions(
        x0_func=fx0, x1_func=fx1, x0_order=2, x1_order=1,
        name='Mixed II',
    )


def get_bc_time_dirichlet(x0_func, x1_func):
    fx0 = _x0_d0(f_t=x0_func) if x0_func is not None else None
    fx1 = _x1_d0(f_t=x1_func) if x1_func is not None else None
    return BoundaryConditions(
        x0_func=fx0, x1_func=fx1, x0_order=1, x1_order=1,
        name='Time-dependent Dirichlet',
    )


def get_bc_time_neumann(x0_func, x1_func, dx):
    fx0 = _x0_d1(g_t=x0_func, dx=dx) if x0_func is not None else None
    fx1 = _x1_d1(g_t=x1_func, dx=dx) if x1_func is not None else None
    return BoundaryConditions(
        x0_func=fx0, x1_func=fx1, x0_order=2, x1_order=2,
        name='Time-dependent Neumann',
    )


def get_bc_time_mixed1(x0_func, x1_func, dx):
    fx0 = _x0_d0(f_t=x0_func) if x0_func is not None else None
    fx1 = _x1_d1(g_t=x1_func, dx=dx) if x1_func is not None else None
    return BoundaryConditions(
        x0_func=fx0, x1_func=fx1, x0_order=1, x1_order=2,
        name='Time-dependent Mixed I',
    )


def get_bc_time_mixed2(x0_func, x1_func, dx):
    fx0 = _x0_d1(g_t=x0_func, dx=dx) if x0_func is not None else None
    fx1 = _x1_d0(f_t=x1_func) if x1_func is not None else None
    return BoundaryConditions(
        x0_func=fx0, x1_func=fx1, x0_order=2, x1_order=1,
        name='Time-dependent Mixed II',
    )
