#include "isp.h"

extern isp_config_t g_isp_config;
void isp_blc(uint16_t *raw_buf)
{
    uint16_t image_width;
    uint16_t image_height;
    uint16_t max_value;
    uint16_t bl_r, bl_gr, bl_gb, bl_b;
    uint16_t r, gr, gb, b;
    uint16_t alpha, beta;
    uint32_t idx;
    uint16_t delta_gr, delta_gb;

    image_width = g_isp_config.image_width;
    image_height = g_isp_config.image_height;
    max_value = (1 << g_isp_config.image_bits) - 1;
    bl_r = g_isp_config.blc_config.bl_r;
    bl_gr = g_isp_config.blc_config.bl_gr;
    bl_gb = g_isp_config.blc_config.bl_gb;
    bl_b = g_isp_config.blc_config.bl_b;

    alpha = g_isp_config.blc_config.alpha * (1 << BLC_FRACTION);
    beta = g_isp_config.blc_config.beta * (1 << BLC_FRACTION);

    for (int i = 2; i < image_height - 2; i += 2)
    {
        for (int j = 2; j < image_width - 2; j += 2)
        {
            idx = i * image_width + j;
            r = raw_buf[idx];
            gr = raw_buf[idx + 1];
            gb = raw_buf[idx + image_width];
            b = raw_buf[idx + image_width + 1];
            r = (r > bl_r) ? (r - bl_r) : 0;
            gr = (gr > bl_gr) ? (gr - bl_gr) : 0;
            gb = (gb > bl_gb) ? (gb - bl_gb) : 0;
            b = (b > bl_b) ? (b - bl_b) : 0;

            delta_gr = (r * alpha) >> BLC_FRACTION;
            delta_gb = (b * beta) >> BLC_FRACTION;
            
            raw_buf[idx] = r;
            raw_buf[idx + 1] = clip_max((gr + delta_gr), max_value);
            raw_buf[idx + image_width] = clip_max((gb + delta_gb), max_value);
            raw_buf[idx + image_width + 1] = b;
        }
    }
}