#include "isp.h"
extern isp_config_t g_isp_config;


void isp_ccm(uint16_t *rgb_buf)
{
    uint16_t image_width;
    uint16_t image_height;
    float *ccm_matrix;
    int16_t fixed_ccm_matrix[12];
    uint16_t max_value;
    uint32_t idx;
    int32_t r, g, b;

    ccm_matrix = g_isp_config.ccm_matrix;
    image_width = g_isp_config.image_width;
    image_height = g_isp_config.image_height;
    max_value = (1 << g_isp_config.image_bits) - 1;

    for (int i = 0; i < 12; i++)
    {
        fixed_ccm_matrix[i] = (int16_t)(ccm_matrix[i] * (1 << CCM_FRACTION));
    }

    for (int i = 0; i < image_height; i++)
    {
        for (int j = 0; j < image_width; j++)
        {
            idx = 3 * (i * image_width + j);
            r = (int16_t)rgb_buf[idx] * fixed_ccm_matrix[0] + (int16_t)rgb_buf[idx + 1] * fixed_ccm_matrix[1] + (int16_t)rgb_buf[idx + 2] * fixed_ccm_matrix[2] + fixed_ccm_matrix[3];
            g = (int16_t)rgb_buf[idx] * fixed_ccm_matrix[4] + (int16_t)rgb_buf[idx + 1] * fixed_ccm_matrix[5] + (int16_t)rgb_buf[idx + 2] * fixed_ccm_matrix[6] + fixed_ccm_matrix[7];
            b = (int16_t)rgb_buf[idx] * fixed_ccm_matrix[8] + (int16_t)rgb_buf[idx + 1] * fixed_ccm_matrix[9] + (int16_t)rgb_buf[idx + 2] * fixed_ccm_matrix[10] + fixed_ccm_matrix[11];

            r = clip_min(r, 0);
            g = clip_min(g, 0);
            b = clip_min(b, 0);

            rgb_buf[idx] = r >> CCM_FRACTION;
            rgb_buf[idx + 1] = g >> CCM_FRACTION;
            rgb_buf[idx + 2] = b >> CCM_FRACTION;
        }
    }

   
}