/*
    Copyright (C) 2016 Fredrik Johansson

    This file is part of Arb.

    Arb is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include "acb_dirichlet.h"

void
acb_dirichlet_hurwitz_precomp_init(acb_dirichlet_hurwitz_precomp_t pre,
        const acb_t s, slong A, slong K, slong N, slong prec)
{
    slong i, k;

    if (A < 1 || K < 1 || N < 1)
        abort();

    pre->A = A;
    pre->K = K;
    pre->N = N;
    pre->coeffs = _acb_vec_init(N * K);

    mag_init(&pre->err);
    acb_init(&pre->s);
    acb_set(&pre->s, s);

    acb_dirichlet_hurwitz_precomp_bound(&pre->err, s, A, K, N);

    if (mag_is_finite(&pre->err))
    {
        acb_t t, a;

        acb_init(t);
        acb_init(a);

        /* (-1)^k (s)_k / k! */
        acb_one(pre->coeffs + 0);
        for (k = 1; k < K; k++)
        {
            acb_add_ui(pre->coeffs + k, s, k - 1, prec);
            acb_mul(pre->coeffs + k, pre->coeffs + k, pre->coeffs + k - 1, prec);
            acb_div_ui(pre->coeffs + k, pre->coeffs + k, k, prec);
            acb_neg(pre->coeffs + k, pre->coeffs + k);
        }

        for (i = 1; i < N; i++)
            _acb_vec_set(pre->coeffs + i * K, pre->coeffs, K);

        /* zeta(s+k,a) where a = A + (2*i+1)/(2*N) */
        for (i = 0; i < N; i++)
        {
            acb_set_ui(a, 2 * i + 1);
            acb_div_ui(a, a, 2 * N, prec);
            acb_add_ui(a, a, A, prec);

            for (k = 0; k < K; k++)
            {
                acb_add_ui(t, s, k, prec);
                acb_hurwitz_zeta(t, t, a, prec);
                acb_mul(pre->coeffs + i * K + k,
                        pre->coeffs + i * K + k, t, prec);
            }
        }

        acb_clear(t);
        acb_clear(a);
    }
}
