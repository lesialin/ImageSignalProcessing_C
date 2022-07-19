#include "isp.h"
#include <cmath>
extern isp_config_t g_isp_config;

void isp_gac(uint16_t *rgb_buf, uint8_t *gamma_table, uint16_t table_size, uint8_t *ga_rgb_buf)
{
    uint16_t image_width;
    uint16_t image_height;
    uint32_t idx;
    uint16_t in_max_value, out_max_value;
    uint16_t value;
    uint8_t l_value, r_value;
    uint16_t l_idx, r_idx;
    uint8_t diff_r, diff_l;

    image_width = g_isp_config.image_width;
    image_height = g_isp_config.image_height;
    in_max_value = (1 << g_isp_config.image_bits) - 1;
    out_max_value = (1 << 8) - 1;

    for (int i = 0; i < image_height; i++)
    {
        for (int j = 0; j < image_width; j++)
        {
            idx = 3 * (i * image_width + j);

            // calculate r
            value = rgb_buf[idx];
            l_idx = value >> GAMMA_STEP;
            r_idx = l_idx + 1;

            l_value = gamma_table[l_idx];
            r_value = gamma_table[r_idx];
            diff_l = value - (l_idx << GAMMA_STEP);
            diff_r = (r_idx << GAMMA_STEP) - value;
            value = (l_value * diff_r + r_value * diff_l) >> GAMMA_STEP;
            ga_rgb_buf[idx] = clip_max(value, 255);

            // calculate g
            value = rgb_buf[idx + 1];
            l_idx = value >> GAMMA_STEP;
            r_idx = l_idx + 1;
            l_value = gamma_table[l_idx];
            r_value = gamma_table[r_idx];
            diff_l = value - (l_idx << GAMMA_STEP);
            diff_r = (r_idx << GAMMA_STEP) - value;
            value = (l_value * diff_r + r_value * diff_l) >> GAMMA_STEP;

            ga_rgb_buf[idx + 1] = clip_max(value, 255);

            // calculate b
            value = rgb_buf[idx + 2];
            l_idx = value >> GAMMA_STEP;
            r_idx = l_idx + 1;
            l_value = gamma_table[l_idx];
            r_value = gamma_table[r_idx];
            diff_l = value - (l_idx << GAMMA_STEP);
            diff_r = (r_idx << GAMMA_STEP) - value;
            value = (l_value * diff_r + r_value * diff_l) >> GAMMA_STEP;
            ga_rgb_buf[idx + 2] = clip_max(value, 255);
        }
    }
}