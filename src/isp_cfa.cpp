#include "isp.h"
extern isp_config_t g_isp_config;

void isp_cfa_malvar(uint16_t *raw_buf, uint16_t image_width, uint16_t image_height, uint16_t max_value, string bayer_pattern, uint16_t *rgb_buf)
{
    if (bayer_pattern.compare("rggb") == 0)
    {
        int count = 0;
        uint64_t idx, base_idx;
        int32_t r, g, b;

        for (int i = 2; i < image_height - 2; i += 2)
        {
            for (int j = 2; j < image_width - 2; j += 2)
            {

                base_idx = i * image_width + j;
                // debayer red idx pixel
                idx = base_idx;

                r = raw_buf[idx];
                g = 4 * raw_buf[idx] - raw_buf[idx - 2 * image_width] - raw_buf[idx - 2] - raw_buf[idx + 2 * image_width] - raw_buf[idx + 2] +
                    2 * (raw_buf[idx + image_width] + raw_buf[idx + 1] + raw_buf[idx - image_width] + raw_buf[idx - 1]);
                b = 6 * raw_buf[idx] - ((3 * (raw_buf[idx - 2 * image_width] + raw_buf[idx - 2] + raw_buf[idx + 2 * image_width] + raw_buf[idx + 2])) >> 1) +
                    2 * (raw_buf[idx - image_width - 1] + raw_buf[idx - image_width + 1] + raw_buf[idx + image_width - 1] + raw_buf[idx + image_width + 1]);

                r = clip_min(r, 0);
                g = clip_min(g, 0);
                b = clip_min(b, 0);

                g = g >> 3;
                b = b >> 3;

                rgb_buf[3 * idx] = clip_max(r, max_value);
                rgb_buf[3 * idx + 1] = clip_max(g, max_value);
                rgb_buf[3 * idx + 2] = clip_max(b, max_value);

                // debayer gr idx pixel
                idx = base_idx + 1;
                r = 5 * raw_buf[idx] - raw_buf[idx - 2] - raw_buf[idx - image_width - 1] - raw_buf[idx + image_width - 1] -
                    raw_buf[idx - image_width + 1] - raw_buf[idx + image_width + 1] - raw_buf[idx + 2] +
                    ((raw_buf[idx - 2 * image_width] + raw_buf[idx + 2 * image_width]) >> 1) + 4 * (raw_buf[idx - 1] + raw_buf[idx + 1]);

                g = raw_buf[idx];
                b = 5 * raw_buf[idx] - raw_buf[idx - 2 * image_width] - raw_buf[idx - image_width - 1] - raw_buf[idx - image_width + 1] -
                    raw_buf[idx + 2 * image_width] - raw_buf[idx + image_width - 1] - raw_buf[idx + image_width + 1] +
                    ((raw_buf[idx - 2] + raw_buf[idx + 2]) >> 1) + 4 * (raw_buf[idx - image_width] + raw_buf[idx + image_width]);

                r = clip_min(r, 0);
                g = clip_min(g, 0);
                b = clip_min(b, 0);

                r = r >> 3;
                b = b >> 3;

                rgb_buf[3 * idx] = clip_max(r, max_value);
                rgb_buf[3 * idx + 1] = clip_max(g, max_value);
                rgb_buf[3 * idx + 2] = clip_max(b, max_value);

                // debayer gb idx pixel
                idx = base_idx + image_width;
                r = 5 * raw_buf[idx] - raw_buf[idx - 2 * image_width] - raw_buf[idx - image_width - 1] - raw_buf[idx - image_width + 1] -
                    raw_buf[idx + 2 * image_width] - raw_buf[idx + image_width - 1] - raw_buf[idx + image_width + 1] +
                    ((raw_buf[idx - 2] - raw_buf[idx + 2]) >> 1) + 4 * (raw_buf[idx - image_width] + raw_buf[idx + image_width]);

                g = raw_buf[idx];

                b = 5 * raw_buf[idx] - raw_buf[idx - 2] - raw_buf[idx - image_width - 1] - raw_buf[idx + image_width - 1] - raw_buf[idx - image_width + 1] -
                    raw_buf[idx + image_width + 1] - raw_buf[idx + 2] +
                    +((raw_buf[idx - 2 * image_width] + raw_buf[idx + 2 * image_width]) >> 1) + 4 * (raw_buf[idx - 1] + raw_buf[idx + 1]);

                r = clip_min(r, 0);
                g = clip_min(g, 0);
                b = clip_min(b, 0);

                r = r >> 3;
                b = b >> 3;

                rgb_buf[3 * idx] = clip_max(r, max_value);
                rgb_buf[3 * idx + 1] = clip_max(g, max_value);
                rgb_buf[3 * idx + 2] = clip_max(b, max_value);

                // debayer blue idex pixel
                idx = base_idx + image_width + 1;
                r = 6 * raw_buf[idx] - ((3 * (raw_buf[idx - 2 * image_width] + raw_buf[idx - 2] + raw_buf[idx + 2 * image_width] + raw_buf[idx + 2])) >> 1) +
                    2 * (raw_buf[idx - image_width - 1] + raw_buf[idx - image_width + 1] + raw_buf[idx + image_width - 1] + raw_buf[idx + image_width + 1]);

                g = 4 * raw_buf[idx] - raw_buf[idx - 2 * image_width] - raw_buf[idx - 2] - raw_buf[idx + 2 * image_width] - raw_buf[idx + 2] +
                    2 * (raw_buf[idx + image_width] + raw_buf[idx + 1] + raw_buf[idx - image_width] + raw_buf[idx - 1]);

                b = raw_buf[idx];

                r = clip_min(r, 0);
                g = clip_min(g, 0);
                b = clip_min(b, 0);

                r = r >> 3;
                g = g >> 3;

                rgb_buf[3 * idx] = clip_max(r, max_value);
                rgb_buf[3 * idx + 1] = clip_max(g, max_value);
                rgb_buf[3 * idx + 2] = clip_max(b, max_value);
            }
        }
    }
    else
    {

        cout << "Oppos! Bayer pattern " << g_isp_config.bayer_pattern << " not implemented yet..." << endl;
        cout << "bye bye QQ~" << endl;
    }
}

void isp_cfa(uint16_t *raw_buf, uint16_t *rgb_buf)
{

    uint16_t image_width;
    uint16_t image_height;
    uint16_t max_value;
    string bayer_pattern;
    string cfa_mode;

    bayer_pattern = g_isp_config.bayer_pattern;
    cfa_mode = g_isp_config.cfa_mode;
    image_width = g_isp_config.image_width;
    image_height = g_isp_config.image_height;
    max_value = (1 << g_isp_config.image_bits) - 1;

    if (cfa_mode.compare("malvar") == 0)
    {
        isp_cfa_malvar(raw_buf, image_width, image_height, max_value, bayer_pattern, rgb_buf);
    }
    else
    {
        cout << "not implement other cfa method yet, bye~"<<endl;
        
    }
}