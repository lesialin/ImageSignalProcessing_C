#include "isp.h"
extern isp_config_t g_isp_config;
void isp_csc(uint8_t *rgb_buf, uint8_t*yuv_buf){

    uint32_t idx;
    uint32_t image_size;
    image_size = g_isp_config.image_height*g_isp_config.image_width;

    for(int i= 0;i < g_isp_config.image_height;i++){
        for(int j = 0; j< g_isp_config.image_width;j++){
            idx = i*g_isp_config.image_width + j;

            //y 
            yuv_buf[idx] = 0.257*rgb_buf[3*idx] + 	0.504*rgb_buf[3*idx+1] + 0.098*rgb_buf[3*idx+2] + 16;
            //u
            yuv_buf[idx+image_size] = -0.148*rgb_buf[3*idx]	-0.291*rgb_buf[3*idx+1] + 0.439*rgb_buf[3*idx+2] + 128;
            //v
            yuv_buf[idx+2*image_size] = 0.439*rgb_buf[3*idx] -0.368*rgb_buf[3*idx+1] -0.071*rgb_buf[3*idx+2] + 128;


        }

    }

}