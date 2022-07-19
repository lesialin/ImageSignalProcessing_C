#include "isp.h"
extern isp_config_t g_isp_config;

void isp_awb_gain(uint16_t *raw_buf)
{

    uint16_t image_height, image_width;
    uint16_t max_value;
    uint32_t idx;
    uint32_t tmp;
    uint8_t r_gain, gr_gain, gb_gain, b_gain;
    string bayer_pattern;

    image_width = g_isp_config.image_width;
    image_height = g_isp_config.image_height;
    max_value = (1 << g_isp_config.image_bits) - 1;
    bayer_pattern = g_isp_config.bayer_pattern;

    r_gain = (uint8_t)(g_isp_config.awb_gain.r_gain * (1 << GAIN_FRACTION_BITS));
    gr_gain = (uint8_t)(g_isp_config.awb_gain.gr_gain * (1 << GAIN_FRACTION_BITS));
    gb_gain = (uint8_t)(g_isp_config.awb_gain.gb_gain * (1 << GAIN_FRACTION_BITS));
    b_gain = (uint8_t)(g_isp_config.awb_gain.b_gain * (1 << GAIN_FRACTION_BITS));

    if (bayer_pattern.compare("rggb") == 0)
    {
        for (uint16_t i = 0; i < image_height; i += 2)
        {
            for (uint16_t j = 0; j < image_width; j += 2)
            {
                idx = i * image_width + j;
                tmp = (r_gain * raw_buf[idx]) >> GAIN_FRACTION_BITS;
                raw_buf[idx] = clip_max(tmp, max_value);
                idx += 1;
                tmp = (gr_gain * raw_buf[idx]) >> GAIN_FRACTION_BITS;
                raw_buf[idx] = clip_max(tmp, max_value);
                idx += image_width;
                tmp = (b_gain * raw_buf[idx]) >> GAIN_FRACTION_BITS;
                raw_buf[idx] = clip_max(tmp, max_value);
                idx -= 1;
                tmp = (gb_gain * raw_buf[idx]) >> GAIN_FRACTION_BITS;
                raw_buf[idx] = clip_max(tmp, max_value);
            }
        }
    }
    else
    {
        cout << "-----------awb gain control--------------" << endl;
        cout << "Oppos! Bayer pattern " << g_isp_config.bayer_pattern << " not implemented yet..." << endl;
        cout << "bye bye QQ~" << endl;
    }
}