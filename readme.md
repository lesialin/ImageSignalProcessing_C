# ISP Image Signal Processor in C

This repo is implementation of ISP in c function, basically it is  re-implemented from [openISP](https://github.com/cruxopen/openISP) in python. I currently complete raw domain function,  the yellow block function as follows  figure.



![](image/isp_pipeline.png)

**Raw domain function**

- [x] Dead Pixel Correction

- [x] Black Level Compensation

- [ ] Len Shading Correction

- [ ] Anti-aliasing Noise Filter

- [x] AWB gain Control

- [x] CFA interpolation

  only implement rggb bayer pattern

- [x] Color Correction

  (only with no conversion matrix test, I need test sample with ccm matrix)

- [x] Gamma Correction 

  piecewise LUT mode 

(If you have more test sample, welcome to offer me, so that I can make the function more robust)

### Usage

build exe file, 

```
mkdir build/
cd build 
cmake ..
cmake --build .
```

then isp_pipeline.exe will in prebuilt/ folder

test isp pipeline

```shell
isp_pipeline.exe test.raw
```

#### Configuration

The configuration can be a adjusted as input raw image.

 In the config file , parameters are floats which are converted to fixed point type in C function to make operation in integer type.

**config.json**

```json
{
    "width": 1920,
    "height": 1080,
    "image_bits": 10,
    "blc":{
        "bl_r":0,
        "bl_gr":0,
        "bl_gb":0,
        "bl_b":0,
        "alpha":0.0,
        "beta":0.0
    },
    "dpc":{
        "dead_thres" : 30,
        "mode":"gradient" 
    },
    "bayer_pattern": "rggb", 
    "cfa_mode": "malvar",
    "awb_gain": {
        "r_gain": 1.5,
        "gr_gain": 1.0,
        "gb_gain": 1.0,
        "b_gain": 1.1
    },
    "ccm": [
        [1.0,0.0,0.0,0.0],
        [0.0,1.0,0.0,0.0],
        [0.0,0.0,1.0,0.0]
    ],
    "gamma": 0.8
}

```





