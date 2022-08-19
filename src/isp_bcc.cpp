#include "isp.h"
#define BBC_CONTRAST_FRACTION_BIT 6

extern isp_config_t g_isp_config;

void isp_bcc(uint8_t *y_image)
{

    uint8_t fixed_contrast_gain = g_isp_config.bcc_config.contrast * (1 << BBC_CONTRAST_FRACTION_BIT);
    uint8_t brightness = g_isp_config.bcc_config.brightness;
    uint8_t y_mean = 127;
    uint16_t value;
    uint16_t image_width, image_height;
    image_width = g_isp_config.image_width;
    image_height = g_isp_config.image_height;

    for (int i = 0; i < image_height; i++)
    {
        for (int j = 0; j < image_width; j++)
        {
            value = y_image[i*image_width + j] + brightness;
            value = clip_max(value,255);
            value = clip_min(value,0);
            
            
            if(value > y_mean){
                value =  y_mean + (uint8_t)((fixed_contrast_gain*(value-(uint8_t)y_mean))>>BBC_CONTRAST_FRACTION_BIT);

            }else{
               
                value =  y_mean - (uint8_t)((fixed_contrast_gain*((uint8_t)y_mean-value))>>BBC_CONTRAST_FRACTION_BIT);
            }

           
            value = clip_max(value,255);
            value = clip_min(value,0);
            y_image[i*image_width + j] = value;
         
            
            
        }

    }
}