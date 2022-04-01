# Histogram-Equalization

The source files are provided in ```./src``` and the header files in ```./include``` 
Initially the sampe pictures and the histogram equalized output of the first sample image was in ```./test``` 
Currently it contains a single histogram equalized output of the first image

You can build the histogram_equalization.cpp file as -  ```g++ -o ./build/histeq_noviz -fopenmp ./src/histogram_equalization.cpp```

(if you would like to launch the visualization tool from cpp itself use the makefile after uncommenting the necessary parts in histogram_equalization.cpp file)

Two prebuilt executables are provided in ```./build``` 

Use the histeq_noviz executable as - 

    The flags used by this executable are - 

        -i -> file path to the input image 
        -o -> file path to the output image
        -t -> set the number of threads for multithreading
        -l -> file path to the log texts (mainly just used for visualizing the histogram of the input and output images)

Example usage - ```./histeq_noviz -i ../test/sample1.png -o ../test/smpout1_4tdx.png -t 4 -l ../logs/smpout1_4tdx.txt```

Use the histeq executable as - 

    The flags used by this executable are 

        -i -> file path to the input image 
        -o -> file path to the output image
        -t -> set the number of threads for multithreading
        -l -> file path to the log text file (mainly just used for visualizing the histogram of the input and output images)
        -v -> launch the visualization_tool.py 

Example usage - ```./histeq -i ../test/sample1.png -o ../test/smpout1_4tdx.png -t 4 -l ../logs/smpout1_4tdx.txt -v```

Use the visualization_tool.py script as - 

    The command line args used by this script are - 

        --log -> file path to the log text file
        --inimg -> file path to the input image 
        --outimg -> file path to the output image

Example usage - ```python3 visualization_tool.py --inimg ../test/sample1.png --outimg ../test/smpout1_4tdx.png --log ../logs/smpout1_4tdx.txt```




