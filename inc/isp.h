#pragma once

#if defined(_MSC_VER)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#include <stdint.h>
#include <iostream>
#include <string>
#include <fstream>
#include "nlohmann/json.hpp"

#define GAIN_FRACTION_BITS 6 //awb gain control
#define BLC_FRACTION 14      //black level compenstation
#define CCM_FRACTION 10      // color conversion matrix
#define GAMMA_STEP 2         // gamma table

#define clip_max(x, value) x > value ? value : x
#define clip_min(x, value) x < value ? value : x

using namespace std;

typedef struct
{
    float r_gain;
    float gr_gain;
    float gb_gain;
    float b_gain;

} awb_gain_t;

typedef struct
{
    uint16_t bl_r;
    uint16_t bl_gr;
    uint16_t bl_gb;
    uint16_t bl_b;
    float alpha;
    float beta;

} blc_cfg_t;

typedef struct
{
    uint8_t Ds;
    uint8_t ds;
    uint8_t h;

} nlm_cfg_t;

typedef struct
{
    uint16_t dw[25];
    uint16_t rw[4];
    uint16_t rthres[3];

} bnf_cfg_t;

typedef struct
{

    uint16_t gain_min;
    uint16_t gain_max;
    uint16_t thres_min;
    uint16_t thres_max ;
    int16_t em_clip_min;
    int16_t em_clip_max;

} eeh_cfg_t;

typedef struct{
    uint8_t edge_min;
    uint8_t edge_max;

} fcs_cfg_t;

typedef struct{

    uint8_t hue_offset;
    float saturation_gain;

} hsc_cfg_t;


typedef struct{

    uint8_t brightness;
    float contrast;

} bcc_cfg_t;

typedef struct
{
    // image resolution
    uint32_t image_width;
    uint32_t image_height;
    uint16_t image_bits;
    // bayer pattern, ex rggb
    string bayer_pattern;
    // black level correction
    blc_cfg_t blc_config;
    // dead pixel correction
    uint16_t dpc_dead_thres;
    string dpc_mode;
    // awb gain
    awb_gain_t awb_gain;
    // color correction matrix
    float ccm_matrix[12];
    //cfa mode
    string cfa_mode;
    // gamma correction parameter
    float gamma;
    //non local mean denoise
    nlm_cfg_t nlm_config;
    //bilateral noise filter
    bnf_cfg_t bnf_config;
    //edge filter
    int16_t edge_filter[15];
    //edge enhancement
    eeh_cfg_t eeh_config;
    //false color suppression
    fcs_cfg_t fcs_config;
    //hue staturation control
    hsc_cfg_t hsc_config;
    //brightness contrast control
    bcc_cfg_t bcc_config;

} isp_config_t;



EXPORT void isp_init();
EXPORT void isp_raw_run(uint16_t *raw_buf, uint8_t *rgb_buf);
EXPORT void isp_yuv_run(uint8_t *rgb_buf, uint8_t *yuv_buf);
EXPORT void isp_load_config(string filename, isp_config_t *isp_config);
//raw domain
void isp_blc(uint16_t *raw_buf);
void isp_dpc(uint16_t *raw_buf);
void isp_aaf(uint16_t *raw_buf);
void isp_awb_gain(uint16_t *raw_buf);
void isp_ccm(uint16_t *rgb_buf);
void isp_cfa(uint16_t *raw_buf, uint16_t *rgb_buf);
void isp_gac(uint16_t *rgb_buf, uint8_t *gamma_table, uint16_t table_size, uint8_t *ga_rgb_buf);

//color space conversion
void isp_csc(uint8_t *rgb_buf, uint8_t *yuv_buf);

//yuv domain
void isp_nlm(uint8_t *src_y_image,uint8_t *dst_y_image);
void isp_bnf(uint8_t *src_y_image, uint8_t *dst_y_image);
void isp_eeh(uint8_t *y_image, int8_t *edge_map);
void isp_bcc(uint8_t* y_image);
void isp_fcs(uint8_t *cbcr_image,int8_t *edge_map);
void isp_hsc(uint8_t *cbcr_image);


