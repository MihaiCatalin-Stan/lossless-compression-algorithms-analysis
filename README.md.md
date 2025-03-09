###### Birladeanu Raluca-Monica & Stan Mihai-Catalin
# Lossless Data Compression Algorithms

## Used Algorithms
* Huffman Coding
* Arithmetic Coding
* Prediction by Partial Matching (PPM)
* Run-Length Encoding (RLE)
* Lempel-Ziv-Welch (LZW)

## Hierarchy

In this project you can find the following:
* Lossless Data Compression.pdf - Detailed documentation about the project, containing analytic and practical data about the used algorithms.
* input/ - Folder that contains the 25 input tests. Further information about them can be found inside the aforementioned pdf.
* output/`algorithm_name` - Folders that contain the compressed 25 tests for each algorithm individually.
* compressed/`algorithm_name` - Folders that contain the decompressed tests for each algorithm.
* run_test.py - Python script to run all the tests on all the algorithms at once.
* logs/ - Folder that contains two subfolders with relevant data:
    * identity_logs/ - Contains short text files for each algorithm containing the result of the identity check after the decompression phase
    * size_logs/ - Contains text files for each algorithm containing relevant data about the results of each test, such as: `input_size`, `compressed_size`, `compression_ratio`, `compression_exec_time` and `decompression_exec_time`.
* Huffman/ - Folder that contains the code for the Huffman algorithm for compression and decompression, along with a makefile.
* Arithmetic/ - Folder that contains the code for the Arithmetic Coding algorithm for compression and decompression, along with a makefile.
* PPM/ - Folder that contains the code for the Prediction by Partial Matching algorithm for compression and decompression, along with a makefile.
* RLE/ - Folder that contains the code for the Run-Length Encoding algorithm, and a makefile for the code.
* lzw/ - Folder that contains the code for the Lempel-Ziv-Welch algorithm, and a makefile.

## Usage
To run the tests for the previously mentioned algorithms you can use the following command:
    
    python3 ./run_tests.py 

The script will run the 25 tests found in the input folder on each studied algorithm. It also checks if the decompressed data is the same as the input and writes data_logs with relevant information about the compression and decompression process.

## References
The algorithms can be found at the following links:
* Huffman: https://github.com/nayuki/Reference-Huffman-coding
* Arithmetic Coding & PPM: https://github.com/nayuki/Reference-arithmetic-coding
* RLE: https://gitlab.com/cam900/RLE
* LZW: https://github.com/eloj/lzw-eddy
