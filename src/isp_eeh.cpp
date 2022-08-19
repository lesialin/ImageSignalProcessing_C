#include "isp.h"
extern isp_config_t g_isp_config;

int8_t emlut(int16_t value)
{
    int32_t lut = 0;
    int8_t out_lut = 0;
    if (value < -g_isp_config.eeh_config.thres_max)
    {
        // value < -x2
        lut = g_isp_config.eeh_config.gain_max * value;
    }
    else if (value >= -g_isp_config.eeh_config.thres_max && value < -g_isp_config.eeh_config.thres_min)
    {
        // -x2 <value < -x1, not edge
        lut = 0;
    }
    else if (value >= -g_isp_config.eeh_config.thres_min && value < g_isp_config.eeh_config.thres_min)
    {
        //-x1 < value < x1, is noise
        lut = -g_isp_config.eeh_config.gain_min * value;
    }
    else if (value >= g_isp_config.eeh_config.thres_min && value < g_isp_config.eeh_config.thres_max)
    {
        // x1 <value < -x2, not edge
        lut = 0;
    }
    else if (value >= g_isp_config.eeh_config.thres_max)
    {
        // value > x2
        lut = g_isp_config.eeh_config.gain_max * value;
    }
    lut = lut >> 8;
    out_lut = lut > g_isp_config.eeh_config.em_clip_max ? g_isp_config.eeh_config.em_clip_max : lut;
    out_lut = lut < g_isp_config.eeh_config.em_clip_min ? g_isp_config.eeh_config.em_clip_min : lut;

    return out_lut;
}

void create_edge_map(uint8_t* src_image, int8_t* edge_map){

    int edge_filter_h = 3;
    int edge_filter_w = 5;
    int half_edge_filter_h = edge_filter_h >> 1;
    int half_edge_filter_w = edge_filter_w >> 1;

    int16_t edge_value, value_sum;
    int8_t value;

    for (int i = 0; i < g_isp_config.image_height - edge_filter_h; i++)
    {
        for (int j = 0; j < g_isp_config.image_width - edge_filter_w; j++)
        {
            value_sum = 0;
            for (int m = 0; m < edge_filter_h; m++)
            {
                for (int n = 0; n < edge_filter_w; n++)
                {
                    value_sum += g_isp_config.edge_filter[m * edge_filter_w + n] * (int16_t)src_image[(i + m) * g_isp_config.image_width + (j + n)];
                }
            }

            value = value_sum >> 3;

            edge_map[(i + half_edge_filter_h) * g_isp_config.image_width + (j + half_edge_filter_w)] = value;

            
        }
    }



}

void isp_eeh(uint8_t *y_image, int8_t *edge_map)
{

    int edge_filter_h = 3;
    int edge_filter_w = 5;
    int half_edge_filter_h = edge_filter_h >> 1;
    int half_edge_filter_w = edge_filter_w >> 1;
            
    int8_t value;
    int8_t lut;
    int16_t new_value;

    create_edge_map(y_image, edge_map);

    for (int i = 0; i < g_isp_config.image_height - edge_filter_h; i++)
    {
        for (int j = 0; j < g_isp_config.image_width - edge_filter_w; j++)
        {
        
            value = edge_map[(i + half_edge_filter_h) * g_isp_config.image_width + (j + half_edge_filter_w)];

            lut = emlut(value);

            new_value = y_image[(i + half_edge_filter_h) * g_isp_config.image_width + (j + half_edge_filter_w)] + lut;
            new_value = clip_max(new_value,255);
            new_value = clip_min(new_value,0);

            y_image[(i + half_edge_filter_h) * g_isp_config.image_width + (j + half_edge_filter_w)] = new_value;
            
        }
    }
}