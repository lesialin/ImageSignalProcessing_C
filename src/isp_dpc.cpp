#include "isp.h"
#include <cmath>
extern isp_config_t g_isp_config;
void isp_dpc(uint16_t *raw_buf)
{
    uint16_t image_width;
    uint16_t image_height;
    uint16_t max_value;
    uint8_t dead_thres;
    bool is_dead = false;
    string dpc_mode;
    uint16_t nieghbor_value[8] = {0};
    uint16_t center_value, double_center_value;
    uint16_t diff_value;
    uint16_t dv, dh, ddr, ddl;
    uint16_t neighbor_sum[4]; // order: dv, dh, ddr, ddl;

    image_width = g_isp_config.image_width;
    image_height = g_isp_config.image_height;
    max_value = (1 << g_isp_config.image_bits) - 1;
    dead_thres = g_isp_config.dpc_dead_thres;
    dpc_mode = g_isp_config.dpc_mode;

    uint32_t idx;
    for (int i = 2; i < image_height - 2; i++)
    {
        for (int j = 2; j < image_width - 2; j++)
        {
            is_dead = true;
            idx = i * image_width + j;
            // dead pixel detection
            center_value = raw_buf[idx];
            nieghbor_value[0] = raw_buf[idx - 2 * image_width - 2];
            nieghbor_value[1] = raw_buf[idx - 2 * image_width];
            nieghbor_value[2] = raw_buf[idx - 2 * image_width + 2];
            nieghbor_value[3] = raw_buf[idx - 2];
            nieghbor_value[4] = raw_buf[idx + 2];
            nieghbor_value[5] = raw_buf[idx + 2 * image_width - 2];
            nieghbor_value[6] = raw_buf[idx + 2 * image_width];
            nieghbor_value[7] = raw_buf[idx + 2 * image_width + 2];

            for (int n = 0; n < 8; n++)
            {
                diff_value = nieghbor_value[n] > center_value ? (nieghbor_value[n] - center_value) : (center_value - nieghbor_value[n]);
                if (diff_value > dead_thres)
                {
                    is_dead = is_dead & 1;
                }
                else
                {
                    is_dead = is_dead & 0;
                }
            }

            // if(is_dead){
            //     cout << "center = "<< center_value <<endl;
            //     for (int n = 0; n < 8 ; n++){
            //         printf("value = %d diff = %d\n",nieghbor_value[n], abs(nieghbor_value[n]-center_value));
            //     }
            // }

            // dead pixel compenstation
            if (is_dead)
            {
                double_center_value = center_value << 1;
                if (dpc_mode.compare("gradient") == 0)
                {

                    // vertical gradient
                    neighbor_sum[0] = nieghbor_value[1] + nieghbor_value[6];
                    dv = double_center_value > neighbor_sum[0] ? (double_center_value - neighbor_sum[0]) : (neighbor_sum[0] - double_center_value);

                    // horizontal gradient
                    neighbor_sum[1] = nieghbor_value[3] + nieghbor_value[4];
                    dh = double_center_value > neighbor_sum[1] ? (double_center_value - neighbor_sum[1]) : (neighbor_sum[1] - double_center_value);

                    // diagonal right gradient
                    neighbor_sum[2] = nieghbor_value[2] + nieghbor_value[5];
                    ddr = double_center_value > neighbor_sum[2] ? (double_center_value - neighbor_sum[2]) : (neighbor_sum[2] - double_center_value);

                    // diagonal left gradient
                    neighbor_sum[3] = nieghbor_value[0] + nieghbor_value[7];
                    ddl = double_center_value > neighbor_sum[3] ? (double_center_value - neighbor_sum[3]) : (neighbor_sum[3] - double_center_value);

                    uint16_t min_value = 9999;
                    if (dv < min_value)
                    {
                        min_value = dv;
                    }

                    if (dh < min_value)
                    {
                        min_value = dh;
                    }

                    if (ddr < min_value)
                    {
                        min_value = ddr;
                    }

                    if (ddl < min_value)
                    {
                        min_value = ddl;
                    }

                    if (min_value == dv)
                    {
                        center_value = (neighbor_sum[0] + 1) >> 1;
                        raw_buf[idx] = clip_max(center_value, max_value);
                    }

                    if (min_value == dh)
                    {
                        center_value = (neighbor_sum[1] + 1) >> 1;
                        raw_buf[idx] = clip_max(center_value, max_value);
                    }

                    if (min_value == ddr)
                    {
                        center_value = (neighbor_sum[2] + 1) >> 1;
                        raw_buf[idx] = clip_max(center_value, max_value);
                    }

                    if (min_value == ddl)
                    {
                        center_value = (neighbor_sum[3] + 1) >> 1;
                        raw_buf[idx] = clip_max(center_value, max_value);
                    }
                }
                else if (dpc_mode.compare("mean") == 0)
                {
                    center_value = (nieghbor_value[1] + nieghbor_value[3] + nieghbor_value[4] + nieghbor_value[6]) >> 2;
                    raw_buf[idx] = clip_max(center_value, max_value);
                }
                else
                {
                    cout << "Dead pixel mode " << dpc_mode << " not implement yet!" << endl;
                    cout << "You can try \"mean\" or \"gradient\" mode." << endl;
                }
            }
        }
    }
}