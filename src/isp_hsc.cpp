#include "isp.h"
#include <math.h>

#define PI 3.14159265
#define HSC_SAT_FRACTION_BIT 6
#define COS_SIN_FRATION_BIT 6

extern isp_config_t g_isp_config;

void isp_hsc(uint8_t *cbcr_image)
{
    uint32_t image_size;
    uint16_t image_width, image_height;
    int16_t fixed_saturation = g_isp_config.hsc_config.saturation_gain * (1 << HSC_SAT_FRACTION_BIT);
    int16_t cb, cr;
    double hue_offset = g_isp_config.hsc_config.hue_offset;
    double cos_theta, sin_theta;
    int8_t fixed_cos_theta, fixed_sin_theta;

    image_width = g_isp_config.image_width;
    image_height = g_isp_config.image_height;
    image_size = image_width * image_height;

    cos_theta = cos(hue_offset / 180 * PI);
    sin_theta = sin(hue_offset / 180 * PI);

    fixed_cos_theta = cos_theta * (1 << COS_SIN_FRATION_BIT);
    fixed_sin_theta = sin_theta * (1 << COS_SIN_FRATION_BIT);

    for (int i = 0; i < image_height; i++)
    {
        for (int j = 0; j < image_width; j++)
        {
            cb = cbcr_image[i * image_width + j];
            cr = cbcr_image[image_size + i * image_width + j];
            cb = (((cb - 128) * fixed_cos_theta + (cr - 128) * fixed_sin_theta) >> COS_SIN_FRATION_BIT) + 128;
            cr = (((cr - 128) * fixed_cos_theta + (cb - 128) * fixed_sin_theta) >> COS_SIN_FRATION_BIT) + 128;

            cb = clip_max(cb,255);
            cb = clip_min(cb,0);
            cr = clip_max(cr,255);
            cr = clip_min(cr,0);

            cb = ((fixed_saturation*(cb -128)) >> HSC_SAT_FRACTION_BIT) +128;
            cr = ((fixed_saturation*(cr -128)) >> HSC_SAT_FRACTION_BIT) +128;

            cb = clip_max(cb,255);
            cb = clip_min(cb,0);
            cr = clip_max(cr,255);
            cr = clip_min(cr,0);

            cbcr_image[i * image_width + j] = cb;
            cbcr_image[image_size + i * image_width + j] = cr;


        }
    }
}