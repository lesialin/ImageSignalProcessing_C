#include "isp.h"
extern isp_config_t g_isp_config;


void isp_fcs(uint8_t *cbcr_image,int8_t *edge_map){

    uint8_t edge;
    int32_t slop;
    int32_t gain;
    int16_t cb;
    int16_t cr;
    uint32_t image_size = g_isp_config.image_width*g_isp_config.image_height;

    slop = -(1.0/(g_isp_config.fcs_config.edge_max-g_isp_config.fcs_config.edge_min))*(1<<16);
    

    for(int i = 0; i < g_isp_config.image_height; i++){
        for(int j = 0; j < g_isp_config.image_width;j++){

            edge = abs(edge_map[i*g_isp_config.image_width+j]);
            
            cb = cbcr_image[i*g_isp_config.image_width+j];
            cr = cbcr_image[image_size + i*g_isp_config.image_width+j];

            if (edge < g_isp_config.fcs_config.edge_min){
                //not edge,  Cb Cr maintain
                cbcr_image[i*g_isp_config.image_width+j] = cb;
                cbcr_image[image_size + i*g_isp_config.image_width+j] = cr;
            
            }else if(edge >= g_isp_config.fcs_config.edge_min && edge <= g_isp_config.fcs_config.edge_max){
                // with gain 
                gain = slop * (int32_t)(g_isp_config.fcs_config.edge_max - edge);
                cb = ((gain * (cb-128)) >> 16) + 128;
                cr = ((gain * (cr-128)) >> 16) + 128;
                cb = clip_max(cb,255);
                cr = clip_max(cr,255);
                cb = clip_min(cb,0);
                cr = clip_min(cr,0);
                cbcr_image[i*g_isp_config.image_width+j] = cb;
                cbcr_image[image_size + i*g_isp_config.image_width+j] = cr;
                
                

            }else if(edge > g_isp_config.fcs_config.edge_max){
                // this is weird...why color is black in edge pixel.?
                //is edge, Cb, Cr ==0
                // dst_image[i*g_isp_config.image_width+j] = 128;
                // dst_image[image_size + i*g_isp_config.image_width+j] = 128;

                //is edge, Cb, Cr  maitain
                cbcr_image[i*g_isp_config.image_width+j] = cb;
                cbcr_image[image_size + i*g_isp_config.image_width+j] = cr;


            }

        }
    }

    

}