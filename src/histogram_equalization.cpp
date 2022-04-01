// uncomment or change code below depending on the python version to use the visualization_tool

/*
#define PY_SSIZE_T_CLEAN
#include "python3.8/Python.h"
*/

#include <stdint.h>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <omp.h>
#include <cmath>
#include <chrono>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

static wchar_t* charToWChar(const char* text)
{
    const size_t size = strlen(text) + 1;
    wchar_t* wText = new wchar_t[size];
    mbstowcs(wText, text, size);
    return wText;
}

int main(int argc, char *argv[])
{
    auto start_0 = std::chrono::high_resolution_clock::now(); // tracking total time spent from here including read/write

    // parsing the command line arguments 
    int input_idx;
    int output_idx;
    int log_idx;
    int num_threads = 6;
    bool viz = false;
    bool log = false;

    for(int ix=1;ix<argc;ix++)
    {
        char *pchar = argv[ix];
        switch(pchar[0])
        {
            case '-':
            {
                switch (pchar[1])
                {
                    case 'i':
                        input_idx = ix + 1;
                        break;

                    case 'o':
                        output_idx = ix + 1;
                        break;

                    case 't':
                        num_threads = (int)*argv[ix+1] - 48; //ASCII code for 0 is 48
                        break;

                    case 'v':
                        viz = true;
                        break;

                    case 'l':
                        log = true;
                        log_idx = ix + 1;
                        break;
                }
            }
        }

    }

    int width, height, channels;
    int i = 0;
    int j = 0;

    std::cout<<"number of threads set as - "<<num_threads<<std::endl;
    omp_set_num_threads(num_threads);

    // loading the reference to where our image is stored 
    uint8_t* png_image = stbi_load(argv[input_idx], &width, &height, &channels, 0);

    // printing the dimensions of the image
    std::cout<<"width of the image is - "<<width<<"\nheight of the image is - "<<height<<"\nnumber of channels in the image - "<<channels<<"\n";

    long n = height*width*channels;
    // std::vector<int> gray_img(n); // the vector onto which the individual pixel values are stored

    int histogram[256] = {0}; // the array where our histogram values are stored 

    auto start_1 = std::chrono::high_resolution_clock::now(); // tracking time spent from here 

    // using array reduction to parallelize creation of the histogram table
    // we use a static schedule because the operations in each loop is identical 
    // also the overhead cost with static scheduling is the least
    #pragma omp parallel for reduction(+:histogram) schedule(static)
    for(i=0;i<n;i++)
    {
        histogram[(int) *(png_image + i)]++;
    }

    // creating the probability distribution table 
    // we don't use parallelism here because the array is only 256 elements long 
    // the overhead costs will hurt more than any benefit we will get out parallelism 
    float pr[256] = {};
    for(i=0;i<256;i++)
    {
        pr[i] = ((float)histogram[i])/((float)n);
    }

    // creating the cummulative table and the calculating the pixel replacement values 
    float sk[256] = {};
    float cd[256] = {};

    for(i=0;i<256;i++)
    {
        for(j=0;j<i+1;j++)
            cd[i] += pr[j];

        sk[i] = 255*cd[i];
    }

    // rounding off the pixel replacement values to uint_8 from float
    int sk_round[256] = {};
    for(i=0;i<256;i++)
    {
        int temp = (int) round(sk[i]);
        if(temp>255) 
            temp = 255;
        else if(temp<0)
            temp = 0;

        sk_round[i] = temp;

    }

    // reference to our output image 
    uint8_t* normalized_image = (uint8_t*)malloc(n*sizeof(uint8_t));


    // parallely writing our replaced pixel values to output image array
    #pragma omp parallel shared(sk_round,png_image) private(i)
    {
        #pragma omp for schedule(static)
        for(i=0;i<n;i++)
        {
            *(normalized_image + i) = (uint8_t) sk_round[(int) *(png_image + i)];
        }
    }

    auto stop_0 = std::chrono::high_resolution_clock::now(); // time taken without writing image to memory

    // writing output image array to disk
    stbi_write_png(argv[output_idx],width,height,channels,normalized_image,width*channels);

    auto stop_1 = std::chrono::high_resolution_clock::now(); // time take with writing image to memory included 

    auto duration_0 = std::chrono::duration_cast<std::chrono::milliseconds>(stop_0 - start_1);
    auto duration_1 = std::chrono::duration_cast<std::chrono::milliseconds>(stop_1 - start_0);
    
    std::cout<<"time taken checkpoint 0 - "<<duration_0.count()<<"\n";
    std::cout<<"time taken checkpoint 1 - "<<duration_1.count()<<"\n";

    // for logging and visualization
    std::vector<int> x(256);
    std::vector<int> y_var(256);
    std::vector<int> y_norm(256);

    if(log)
    {
        for(i=0;i<256;i++)
        {
            x[i] = i;
            y_var[i] = histogram[i];
            y_norm[sk_round[i]] = histogram[i];
        }

        std::ofstream outdata;
        outdata.open(argv[log_idx]);
        if( !outdata)
        {
            std::cerr<<"Error: file could not be opened\n";
            exit(1);
        }

        for(i=0;i<256;i++)
            outdata<<x[i]<<" "<<y_var[i]<<" "<<y_norm[i]<<std::endl;

        outdata.close();
    }

    //calling our python visualization tool
    if(viz)
    {
        FILE* file;
        int argno;
        wchar_t* argval[6];

        argno = 7;
        argval[0] = charToWChar("visualization_tool.py");

        argval[1] = charToWChar("--log");
        argval[2] = charToWChar(argv[log_idx]);

        argval[3] = charToWChar("--inimg");
        argval[4] = charToWChar(argv[input_idx]);

        argval[5] = charToWChar("--outimg");
        argval[6] = charToWChar(argv[output_idx]);

        // uncomment code below to launch the visualization_tool from c++. Dependencies issues may arise if you are not on the correct python version 
        
        /*
        Py_SetProgramName(argval[0]);
        Py_Initialize();

        PySys_SetArgv(argno,argval);
        file = fopen("visualization_tool.py","r");
        PyRun_SimpleFile(file,"visualization_tool.py");
        Py_Finalize();
        */

    }   

    return 0;
}