#include "isp.h"
extern isp_config_t g_isp_config;


void isp_aaf(uint16_t *raw_buf)
{
    uint16_t image_height = g_isp_config.image_height;
    uint16_t image_width = g_isp_config.image_width;
    uint32_t idx;
    uint16_t arr[9] = {0};

    for (int i = 2; i < image_height - 2; i++)
    {
        for (int j = 2; j < image_width - 2; j++)
        {

            idx = i * image_width + j;

            raw_buf[idx] = ((raw_buf[idx] << 3) + raw_buf[idx - 2 * image_width - 2] + raw_buf[idx - 2 * image_width + 2] + raw_buf[idx + 2 * image_width - 2] +
                            raw_buf[idx + 2 * image_width + 2] + raw_buf[idx - 2] + raw_buf[idx + 2] + raw_buf[idx + 2 * image_width] + raw_buf[idx - 2 * image_width]) >>
                           4;
            
        }
    }
}