#include <math.h>
#include "isp.h"
extern isp_config_t g_isp_config;

void integrate_gray(uint8_t *image, uint16_t image_width, uint16_t image_height, uint32_t *image_out)
{

    image_out[0] = image[0];
    //integrate first row
    for (int j = 1; j < image_width; j++)
    {
        image_out[j] = image_out[j - 1] + image[j];
    }

    //integrate first col
    for (int i = 1; i < image_height; i++)
    {
        image_out[i * image_width] = image_out[(i - 1) * image_width] + image[i * image_width];
    }

    for (int i = 1; i < image_height; i++)
    {
        for (int j = 1; j < image_width; j++)
        {

            image_out[i * image_width + j] = image_out[(i - 1) * image_width + j] + image_out[i * image_width + (j - 1)] -
                                             image_out[(i - 1) * image_width + (j - 1)] + image[i * image_width + j];
        }
    }
}

void isp_nlm(uint8_t *src_y_image,uint8_t *dst_y_image)
{

    uint16_t image_height, image_width;
    uint16_t blk_x0, blk_x1, blk_y0, blk_y1;
    uint16_t sblk_x0, sblk_x1, sblk_y0, sblk_y1;
    uint32_t *integ_image;
    int64_t blk_sum, sblk_sum;
    uint8_t table_size;
    float *weight_table;
    float distance, weight, k;
    uint8_t Ds, ds, h;
    
    Ds = g_isp_config.nlm_config.Ds;
    ds = g_isp_config.nlm_config.ds;
    h = g_isp_config.nlm_config.h;

    image_width = g_isp_config.image_width;
    image_height = g_isp_config.image_height;

    table_size = 2 * (Ds - ds) + 1;

    //d = 1.0 / ((2 * ds + 1) * (2 * ds + 1));
    //TOOD: convert to fixed-point
    k = -1.0 / ((2 * ds + 1) * (2 * ds + 1) * h * h);

    integ_image = (uint32_t *)malloc(image_width * image_height * sizeof(uint32_t));

    weight_table = (float *)malloc(table_size * table_size * sizeof(float));

    integrate_gray(src_y_image, image_width, image_height, integ_image);

    for (int i = Ds; i < image_height - Ds; i++)
    {
        for (int j = Ds; j < image_width - Ds; j++)
        {
            blk_x0 = j - ds;
            blk_x1 = j + ds;
            blk_y0 = i - ds;
            blk_y1 = i + ds;

            blk_sum = integ_image[blk_y0 * image_width + blk_x0] + integ_image[blk_y1 * image_width + blk_x1] -
                      integ_image[blk_y0 * image_width + blk_x1] - integ_image[blk_y1 * image_width + blk_x0];

            float weight_max = 0;
            float weight_sum = 0;
            uint16_t idx = 0;
            for (int m = -Ds + ds; m < Ds - ds; m++)
            {
                for (int n = -Ds + ds; n < Ds - ds; n++)
                {
                    sblk_x0 = j + n - ds;
                    sblk_x1 = j + n + ds;
                    sblk_y0 = i + m - ds;
                    sblk_y1 = i + m + ds;
                    sblk_sum = integ_image[sblk_y0 * image_width + sblk_x0] + integ_image[sblk_y1 * image_width + sblk_x1] -
                               integ_image[sblk_y0 * image_width + sblk_x1] - integ_image[sblk_y1 * image_width + sblk_x0];

                    // n = -distance/h^2
                    // n = - (d*(blk_sum - sblk_sum)*(blk_sum - sblk_sum))/h^2
                    // n = - (blk_sum - sblk_sum)*(blk_sum - sblk_sum)/((2 * ds + 1) * (2 * ds + 1))*h*h
                    // n = k * (blk_sum - sblk_sum)*(blk_sum - sblk_sum)
                    // distance = d * pow((blk_sum - sblk_sum), 2);
                    // weight = pow(2.718, (-distance / (h * h)));

                    //TODO: use taylor series to improve 
                    weight = pow(2.718, k * (blk_sum - sblk_sum) * (blk_sum - sblk_sum));

                    if (weight > weight_max)
                    {
                        weight_max = weight;
                    }
                    weight_sum += weight;

                    weight_table[idx] = weight;
                    idx++;
                }
            }
            weight_table[(Ds - ds + 1) * table_size + Ds - ds + 1] = weight_max;

            float value = 0.0;
            idx = 0;
            for (int m = -Ds + ds; m < Ds - ds; m++)
            {
                for (int n = -Ds + ds; n < Ds - ds; n++)
                {
                    value += weight_table[idx] * src_y_image[(i + m) * image_width + (j + n)];
                    idx++;
                }
            }
            value /= weight_sum;
            if (value > 255)
            {
                value = 255;
            }

            dst_y_image[i * image_width + j] = (uint8_t)value;
        }
    }

    free(integ_image);
}