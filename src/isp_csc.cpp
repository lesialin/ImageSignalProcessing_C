#include "isp.h"
#define CSC_FIXED_FRACTION_BITS 8
extern isp_config_t g_isp_config;
int16_t g_csc_mat_fixed[9];

void isp_csc_init()
{

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            g_csc_mat_fixed[i * 3 + j] = g_isp_config.csc_matrix[i * 4 + j] * (1 << CSC_FIXED_FRACTION_BITS) + 0.5;
            // printf("fixed csc[%d, %d] =  %d, float csc[%d, %d] = %.2f\n", i,j,g_csc_mat_fixed[i*3+j],i,j,g_isp_config.csc_matrix[i*4+j]);
        }
    }
}

void isp_csc(uint8_t *rgb_buf, uint8_t *yuv_buf)
{

    uint32_t idx;
    uint32_t image_size;
    float *csc_mat_f;
    csc_mat_f = g_isp_config.csc_matrix;
    image_size = g_isp_config.image_height * g_isp_config.image_width;
    isp_csc_init();

    for (int i = 0; i < g_isp_config.image_height; i++)
    {
        for (int j = 0; j < g_isp_config.image_width; j++)
        {
            idx = i * g_isp_config.image_width + j;

            // y
            yuv_buf[idx] = ((g_csc_mat_fixed[0] * (int16_t)rgb_buf[3 * idx] + g_csc_mat_fixed[1] * (int16_t)rgb_buf[3 * idx + 1] + g_csc_mat_fixed[2] * (int16_t)rgb_buf[3 * idx + 2] + (1<<(CSC_FIXED_FRACTION_BITS-1))) >> CSC_FIXED_FRACTION_BITS) + 16;
            // u
            yuv_buf[idx + image_size] = ((g_csc_mat_fixed[3] * (int16_t)rgb_buf[3 * idx] + g_csc_mat_fixed[4] * (int16_t)rgb_buf[3 * idx + 1] + g_csc_mat_fixed[5] * (int16_t)rgb_buf[3 * idx + 2] + (1<<(CSC_FIXED_FRACTION_BITS-1))) >> CSC_FIXED_FRACTION_BITS) + 128;
            // v
            yuv_buf[idx + 2 * image_size] = ((g_csc_mat_fixed[6] * (int16_t)rgb_buf[3 * idx] + g_csc_mat_fixed[7] * (int16_t)rgb_buf[3 * idx + 1] + g_csc_mat_fixed[8] * (int16_t)rgb_buf[3 * idx + 2]+ (1<<(CSC_FIXED_FRACTION_BITS-1))) >> CSC_FIXED_FRACTION_BITS) + 128;

           
        }
    }
}