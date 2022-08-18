#include "isp.h"
#include <cmath>
#include <vector>
#include "opencv2/opencv.hpp"


using namespace cv;
using json = nlohmann::json;
extern isp_config_t g_isp_config;
uint8_t *gamma_table;
uint16_t g_table_size;

void isp_load_config(string filename, isp_config_t *isp_config)
{

    // uint32_t ccm_matrix[12] = {0};

    std::ifstream file(filename);
    json jfile = json::parse(file);

    isp_config->image_height = jfile["height"];
    isp_config->image_width = jfile["width"];
    isp_config->image_bits = jfile["image_bits"];
    isp_config->bayer_pattern = jfile["bayer_pattern"];
    isp_config->blc_config.bl_r = jfile["blc"]["bl_r"];
    isp_config->blc_config.bl_gr = jfile["blc"]["bl_gr"];
    isp_config->blc_config.bl_gb = jfile["blc"]["bl_gb"];
    isp_config->blc_config.bl_b = jfile["blc"]["bl_b"];
    isp_config->blc_config.alpha = jfile["blc"]["alpha"];
    isp_config->blc_config.beta = jfile["blc"]["beta"];
    isp_config->dpc_dead_thres = jfile["dpc"]["dead_thres"];
    isp_config->dpc_mode = jfile["dpc"]["mode"];
    isp_config->awb_gain.r_gain = jfile["awb_gain"]["r_gain"];
    isp_config->awb_gain.gr_gain = jfile["awb_gain"]["gr_gain"];
    isp_config->awb_gain.gb_gain = jfile["awb_gain"]["gb_gain"];
    isp_config->awb_gain.b_gain = jfile["awb_gain"]["b_gain"];
    isp_config->cfa_mode = jfile["cfa_mode"];
    isp_config->gamma = jfile["gamma"];
    isp_config->nlm_config.Ds = jfile["nlm"]["Ds"];
    isp_config->nlm_config.ds = jfile["nlm"]["ds"];
    isp_config->nlm_config.h = jfile["nlm"]["h"];
    isp_config->eeh_config.gain_min = jfile["eeh"]["gain_min"];
    isp_config->eeh_config.gain_max = jfile["eeh"]["gain_max"];
    isp_config->eeh_config.thres_min = jfile["eeh"]["thres_min"];
    isp_config->eeh_config.thres_max = jfile["eeh"]["thres_max"];
    isp_config->eeh_config.em_clip_min= jfile["eeh"]["em_clip_min"];
    isp_config->eeh_config.em_clip_max = jfile["eeh"]["em_clip_max"];


    std::vector<std::vector<float>> m;
    m = jfile["ccm"].get<std::vector<std::vector<float>>>();
    int count = 0;
    for (auto i : m)
    {
        for (auto j : i)
        {

            isp_config->ccm_matrix[count] = (float)j;
            count++;
        }
    }

    std::vector<std::vector<int16_t>> e;
    e = jfile["edge_filter"].get<std::vector<std::vector<int16_t>>>();
    count = 0;
    for (auto i : e)
    {
        for (auto j : i)
        {

            isp_config->edge_filter[count] = (int16_t)j;
            count++;
        }
    }

    
    std::vector<std::vector<uint16_t>> w0;
    w0 = jfile["bnf"]["dw"].get<std::vector<std::vector<uint16_t>>>();
    count = 0;
    for (auto i : w0)
    {
        for (auto j : i)
        {

            isp_config->bnf_config.dw[count] = (uint16_t)j;
            count++;
        }
    }

    
    std::vector<uint16_t> w1;
    w1 = jfile["bnf"]["rw"].get<std::vector<uint16_t>>();
    count = 0;
    for (auto i : w1)
    {

        isp_config->bnf_config.rw[count] = (uint16_t)i;
        count++;
    }

    std::vector<uint16_t> w2;
    w2 = jfile["bnf"]["rthres"].get<std::vector<uint16_t>>();
    count = 0;
    for (auto i : w2)
    {

        isp_config->bnf_config.rthres[count] = (uint16_t)i;
        count++;
    }

    
#if 0
    //log config
    cout << "isp pipeline config:" << endl;
    cout << "image resolution = " << isp_config->image_width << "x" << isp_config->image_height << "," << isp_config->image_bits << " bits/pixels" << endl;
    cout << "bayer_patter = " << isp_config->bayer_pattern << endl;
    cout << "gamma = " << isp_config->gamma << endl;
    cout << "dpc_threshold = " << isp_config->dpc_dead_thres << endl;
    cout << "awb gain :" << endl;
    cout << "r_gain = " << isp_config->awb_gain.r_gain << endl;
    cout << "gr_gain = " << isp_config->awb_gain.gr_gain << endl;
    cout << "gb_gain = " << isp_config->awb_gain.gb_gain << endl;
    cout << "b_gain = " << isp_config->awb_gain.b_gain << endl;
    cout << "ccm matrix:" << endl;
    cout << isp_config->ccm_matrix[0] << "," << isp_config->ccm_matrix[1] << "," << isp_config->ccm_matrix[2] << "," << isp_config->ccm_matrix[3] << endl;
    cout << isp_config->ccm_matrix[4] << "," << isp_config->ccm_matrix[5] << "," << isp_config->ccm_matrix[6] << "," << isp_config->ccm_matrix[7] << endl;
    cout << isp_config->ccm_matrix[8] << "," << isp_config->ccm_matrix[9] << "," << isp_config->ccm_matrix[10] << "," << isp_config->ccm_matrix[11] << endl;
    cout << "bnf dw:" << endl;
    cout << isp_config->bnf_config.dw[0] << "," << isp_config->bnf_config.dw[1] << "," << isp_config->bnf_config.dw[2] << "," << isp_config->bnf_config.dw[3] << "," << isp_config->bnf_config.dw[4] << endl;
    cout << isp_config->bnf_config.dw[5] << "," << isp_config->bnf_config.dw[6] << "," << isp_config->bnf_config.dw[7] << "," << isp_config->bnf_config.dw[8] << "," << isp_config->bnf_config.dw[9] << endl;
    cout << isp_config->bnf_config.dw[10] << "," << isp_config->bnf_config.dw[11] << "," << isp_config->bnf_config.dw[12] << "," << isp_config->bnf_config.dw[13] << "," << isp_config->bnf_config.dw[14] << endl;
    cout << isp_config->bnf_config.dw[15] << "," << isp_config->bnf_config.dw[16] << "," << isp_config->bnf_config.dw[17] << "," << isp_config->bnf_config.dw[18] << "," << isp_config->bnf_config.dw[19] << endl;
    cout << isp_config->bnf_config.dw[20] << "," << isp_config->bnf_config.dw[21] << "," << isp_config->bnf_config.dw[22] << "," << isp_config->bnf_config.dw[23] << "," << isp_config->bnf_config.dw[24] << endl;
    cout << "bnf rw:" << endl;
    cout << isp_config->bnf_config.rw[0] << "," << isp_config->bnf_config.rw[1] << "," << isp_config->bnf_config.rw[2] << "," << isp_config->bnf_config.rw[3]<< endl;
    cout << "bnf rthres:" << endl;
    cout << isp_config->bnf_config.rthres[0] << "," << isp_config->bnf_config.rthres[1] << "," << isp_config->bnf_config.rthres[2]<< endl;
    cout << "edge_filter:" << endl;
    cout << isp_config->edge_filter[0] << "," << isp_config->edge_filter[1] << "," << isp_config->edge_filter[2] << "," << isp_config->edge_filter[3] << "," << isp_config->edge_filter[4] << endl;
    cout << isp_config->edge_filter[5] << "," << isp_config->edge_filter[6] << "," << isp_config->edge_filter[7] << "," << isp_config->edge_filter[8] << "," << isp_config->edge_filter[9] << endl;
    cout << isp_config->edge_filter[10] << "," << isp_config->edge_filter[11] << "," << isp_config->edge_filter[12] << "," << isp_config->edge_filter[13] << "," << isp_config->edge_filter[14] << endl;
    cout << "eeh gain_min = "<<isp_config->eeh_config.gain_min<<endl;
    cout << "eeh gain_max = "<<isp_config->eeh_config.gain_max<<endl;
    cout << "eeh thres_min = "<<isp_config->eeh_config.thres_min<<endl;
    cout << "eeh thres_max = "<<isp_config->eeh_config.thres_max<<endl;
    cout << "eeh em_clip_min = "<<isp_config->eeh_config.em_clip_min<<endl;
    cout << "eeh em_clip_max = "<<isp_config->eeh_config.em_clip_max<<endl;

#endif
}

void isp_init()
{
    uint16_t in_max_value, out_max_value;
    float gamma;

    // init gamma table
    in_max_value = (1 << g_isp_config.image_bits);
    out_max_value = (1 << 8);
    gamma = g_isp_config.gamma;

    uint8_t *s_gamma_table;

    g_table_size = in_max_value >> GAMMA_STEP;

    gamma_table = (uint8_t *)malloc(sizeof(uint8_t) * g_table_size);
    for (int i = 0; i < g_table_size; i++)
    {
        gamma_table[i] = (uint8_t)(pow((float)(i * (1 << GAMMA_STEP) + (1 << (GAMMA_STEP - 1))) / in_max_value, gamma) * out_max_value);
        //printf("gamma_table[%d] = %d\n",i, gamma_table[i]);
    }
}

void isp_raw_run(uint16_t *raw_buf, uint8_t *rgb_buf)
{

    uint32_t image_size;
    uint16_t *rgb_10b_buf;
    uint16_t max_value, table_size;

    image_size = g_isp_config.image_height * g_isp_config.image_width;
    rgb_10b_buf = (uint16_t *)malloc(sizeof(uint16_t) * image_size * 3);
    // dead pixel correction
    isp_dpc(raw_buf);
    // black level correction
    isp_blc(raw_buf);
    //anti-aliasing filter
    isp_aaf(raw_buf);
    // awb gain
    isp_awb_gain(raw_buf);
    // cfa interpolation
    isp_cfa(raw_buf, rgb_10b_buf);
    // color correction matrix
    isp_ccm(rgb_10b_buf);
    // gamma correction
    max_value = (1 << g_isp_config.image_bits);
    table_size = max_value >> GAMMA_STEP;
    isp_gac(rgb_10b_buf, gamma_table, table_size, rgb_buf);

    free(rgb_10b_buf);
}

void isp_yuv_run(uint8_t *rgb_buf, uint8_t *yuv_buf)
{

    uint8_t* y_nlm_image = (uint8_t *)malloc(sizeof(uint8_t) * g_isp_config.image_height * g_isp_config.image_width);
    uint8_t* y_bnf_image = (uint8_t *)malloc(sizeof(uint8_t) * g_isp_config.image_height * g_isp_config.image_width);
    uint8_t* y_eeh_image = (uint8_t *)malloc(sizeof(uint8_t) * g_isp_config.image_height * g_isp_config.image_width);
    uint8_t* y_dege_map = (uint8_t *)malloc(sizeof(uint8_t) * g_isp_config.image_height * g_isp_config.image_width);

    isp_csc(rgb_buf, yuv_buf);
    isp_nlm(yuv_buf,y_nlm_image);
    isp_bnf(y_nlm_image,y_bnf_image);
    isp_eeh(y_bnf_image,y_dege_map,y_eeh_image);


    Mat mat_image_1(g_isp_config.image_height, g_isp_config.image_width, CV_8UC1, y_nlm_image);
    Mat mat_image_2(g_isp_config.image_height, g_isp_config.image_width, CV_8UC1, y_bnf_image);
    Mat mat_image_3(g_isp_config.image_height, g_isp_config.image_width, CV_8UC1, y_eeh_image);
    
    imwrite("nlm.png",mat_image_1);
    imwrite("bnf.png",mat_image_2);
    imwrite("eeh.png",mat_image_3);

    free(y_nlm_image);
    free(y_bnf_image);
    free(y_eeh_image);
    free(y_dege_map);

}