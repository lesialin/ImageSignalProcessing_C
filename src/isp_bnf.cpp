#include "isp.h"
/* Bilateral Noise Filter*/

extern isp_config_t g_isp_config;

void isp_bnf(uint8_t *src_image, uint8_t *dst_image)
{

    uint8_t kernel_size = 5;
    uint8_t half_kernel_size = kernel_size >> 1;
    uint16_t *cw = (uint16_t *)malloc(sizeof(uint16_t) * (kernel_size * kernel_size));      //color weight
    uint16_t *weights = (uint16_t *)malloc(sizeof(uint16_t) * (kernel_size * kernel_size)); // bnf weight
    uint8_t center_value, value, diff_value;
    uint32_t weight_sum;
    uint32_t value_sum;
    for (int i = 0; i < g_isp_config.image_height - kernel_size; i++)
    {
        for (int j = 0; j < g_isp_config.image_width - kernel_size; j++)
        {

            center_value = src_image[(i + half_kernel_size) * g_isp_config.image_width + (j + half_kernel_size)];
            for (int m = 0; m < kernel_size; m++)
            {
                for (int n = 0; n < kernel_size; n++)
                {
                    value = src_image[(i + m) * g_isp_config.image_width + (j + n)];
                    diff_value = center_value > value ? (center_value - value) : (value - center_value);
                    if (diff_value >= g_isp_config.bnf_config.rthres[0])
                    {

                        cw[n * kernel_size + n] = g_isp_config.bnf_config.rw[0];
                    }
                    else if ((diff_value < g_isp_config.bnf_config.rthres[0]) && (diff_value >= g_isp_config.bnf_config.rthres[1]))
                    {
                        cw[n * kernel_size + n] = g_isp_config.bnf_config.rw[1];
                    }
                    else if ((diff_value < g_isp_config.bnf_config.rthres[1]) && (diff_value >= g_isp_config.bnf_config.rthres[2]))
                    {
                        cw[n * kernel_size + n] = g_isp_config.bnf_config.rw[2];
                    }
                    else if (diff_value < g_isp_config.bnf_config.rthres[2])
                    {
                        cw[n * kernel_size + n] = g_isp_config.bnf_config.rw[3];
                    }
                }
            }
            //mutiply color weight and space wieght
            weight_sum = 0;
            value_sum = 0;
            for (int m = 0; m < kernel_size; m++)
            {
                for (int n = 0; n < kernel_size; n++)
                {
                    weights[m * kernel_size + n] = g_isp_config.bnf_config.dw[m * kernel_size + n] * cw[m * kernel_size + n];
                    value_sum += weights[m * kernel_size + n] * src_image[(i + m) * g_isp_config.image_width + (j + n)];
                    weight_sum += weights[m * kernel_size + n];
                }
            }

            dst_image[(i + half_kernel_size) * g_isp_config.image_width + (j + half_kernel_size)] = value_sum/weight_sum;
        }
    }
}

  