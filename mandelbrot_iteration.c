#include "mandelbrot_iteration.h"
#ifdef __APPLE__
    #include <arm_neon.h>
#endif
#include <stdint.h>

uint16_t mandelbrot_iterations(Complex c, uint16_t max_iterations)
{
    Complex z = {0.0, 0.0};
    uint16_t iterations = 0;
    while (z.real * z.real + z.imag * z.imag <= 4.0 && iterations < max_iterations)
    {
        double temp = z.real * z.real - z.imag * z.imag + c.real;
        z.imag = 2.0 * z.real * z.imag + c.imag;
        z.real = temp;
        iterations++;
    }

    return iterations;
}

Complex calculate_complex(int x, int y, Zoom *zoom)
{
    Complex c;
    c.real = (x - zoom->width / 2.0) / zoom->factor + zoom->offset_x;
    c.imag = (y - zoom->height / 2.0) / zoom->factor + zoom->offset_y;
    return c;
}

void calculate_rect(Rectangle rect, uint16_t *result, Zoom *zoom, uint16_t max_iterations)
{
    for (int y = rect.tl.y; y < rect.br.y; y++)
    {
        for (int x = rect.tl.x; x < rect.br.x; x++)
        {
            Complex c = calculate_complex(x, y, zoom);
            result[x + y * zoom->width] = mandelbrot_iterations(c, max_iterations);
        }
    }
}

#ifdef __APPLE__
void calculate_rect_simd_neon_double(Rectangle rect, uint16_t *result, Zoom *zoom, uint16_t max_iterations)
{
    const double center_x = zoom->width / 2.0;
    const double center_y = zoom->height / 2.0;
    const double inv_factor = 1.0 / zoom->factor;

    const float64x2_t vinv_factor = vdupq_n_f64(inv_factor);
    const float64x2_t voffset_x = vdupq_n_f64(zoom->offset_x);
    const float64x2_t voffset_y = vdupq_n_f64(zoom->offset_y);
    const float64x2_t vcenter_x = vdupq_n_f64(center_x);
    const float64x2_t vcenter_y = vdupq_n_f64(center_y);
    const float64x2_t vfour = vdupq_n_f64(4.0);
    const float64x2_t vtwo = vdupq_n_f64(2.0);

    for (int y = rect.tl.y; y < rect.br.y; y++)
    {
        const float64x2_t vy = vdupq_n_f64((double)y);
        const float64x2_t c_imag = vfmaq_f64(voffset_y, vsubq_f64(vy, vcenter_y), vinv_factor);

        int x = rect.tl.x;

        for (; x <= rect.br.x - 2; x += 2)
        {
            const double x_vals[2] = {(double)x, (double)(x + 1)};
            const float64x2_t vx = vld1q_f64(x_vals);
            const float64x2_t c_real = vfmaq_f64(voffset_x, vsubq_f64(vx, vcenter_x), vinv_factor);

            float64x2_t z_real = vdupq_n_f64(0.0);
            float64x2_t z_imag = vdupq_n_f64(0.0);
            uint16_t iterations[2] = {0, 0};
            uint64x2_t active_mask = vdupq_n_u64(0xFFFFFFFFFFFFFFFFULL);

            for (int iter = 0; iter < max_iterations; iter++)
            {
                if (vgetq_lane_u64(active_mask, 0) == 0 && vgetq_lane_u64(active_mask, 1) == 0)
                    break;

                float64x2_t z_real_sq = vmulq_f64(z_real, z_real);
                float64x2_t z_imag_sq = vmulq_f64(z_imag, z_imag);
                float64x2_t magnitude_sq = vaddq_f64(z_real_sq, z_imag_sq);

                uint64x2_t not_escaped = vcleq_f64(magnitude_sq, vfour);
                active_mask = vandq_u64(active_mask, not_escaped);

                if (vgetq_lane_u64(active_mask, 0) != 0)
                    iterations[0]++;
                if (vgetq_lane_u64(active_mask, 1) != 0)
                    iterations[1]++;

                float64x2_t temp_real = vfmsq_f64(c_real, z_imag_sq, vdupq_n_f64(1.0));
                temp_real = vaddq_f64(temp_real, z_real_sq);
                float64x2_t temp_imag = vfmaq_f64(c_imag, vmulq_f64(z_real, z_imag), vtwo);

                z_real = vbslq_f64(vreinterpretq_u64_f64(active_mask), temp_real, z_real);
                z_imag = vbslq_f64(vreinterpretq_u64_f64(active_mask), temp_imag, z_imag);
            }

            result[x + y * zoom->width] = iterations[0];
            result[(x + 1) + y * zoom->width] = iterations[1];
        }

        for (; x < rect.br.x; x++)
        {
            Complex c = calculate_complex(x, y, zoom);
            result[x + y * zoom->width] = mandelbrot_iterations(c, max_iterations);
        }
    }
}
#endif

void calculate_rect_with_period_check(Rectangle rect, uint16_t *result, Zoom *zoom, uint16_t max_iterations)
{
    const double center_x = zoom->width / 2.0;
    const double center_y = zoom->height / 2.0;
    const double inv_factor = 1.0 / zoom->factor;

    for (int y = rect.tl.y; y < rect.br.y; y++)
    {
        const double c_imag = (y - center_y) * inv_factor + zoom->offset_y;

        for (int x = rect.tl.x; x < rect.br.x; x++)
        {
            const double c_real = (x - center_x) * inv_factor + zoom->offset_x;

            double zr = 0.0, zi = 0.0;
            double old_zr = 0.0, old_zi = 0.0;
            uint16_t iter = 0;
            uint16_t period = 1;
            uint16_t check = 3;

            while (iter < max_iterations)
            {
                double zr2 = zr * zr;
                double zi2 = zi * zi;

                if (zr2 + zi2 > 4.0)
                    break;

                zi = 2.0 * zr * zi + c_imag;
                zr = zr2 - zi2 + c_real;
                iter++;

                if (iter == check)
                {
                    if (zr == old_zr && zi == old_zi)
                    {
                        iter = max_iterations;
                        break;
                    }
                    old_zr = zr;
                    old_zi = zi;
                    check = iter + period;
                    period *= 2;
                }
            }

            result[x + y * zoom->width] = iter;
        }
    }
}