import subprocess
import os
import filecmp
import time

coduri = ["Huffman", "Arithmetic", "RLE", "Ppm", "lzw"]
teste = ["test1.txt", "test2.txt", "test3.cpp", "test4.bin", "test5.csv", "test6.txt", "test7.bmp", "test8.jpg", "test9.png", "test10.json", "test11.aac", "test12.mp3", "test13.aiff", "test14.wav", "test15.mp4", "test16.mov", "test17.avi", "test18.pbm", "test19.pbm", "test20.txt", "test21.txt", "test22.txt", "test23.txt", "test24.txt", "test25.txt"]

for cod in coduri:
    # Run the makefile for each code
    subprocess.run(["make", "-C", f"{cod}"], check=True)

    # Open the size logs for each code
    with open(f"logs/identity_logs/{cod}_log.txt", "w") as log_file, \
         open(f"logs/size_logs/{cod}_size_log.txt", "w") as size_log_file_cod:
        
        # log_file.write("Test: [test_name], Is Lossless: [True/False]")
        # size_log_file_cod.write("Test, Input Size (bytes), Compressed Size (bytes), Compression Ratio, Compression exec time (s), Decompression exec time (s)\n")

        for test in teste:
            input_path = f"input/{test}"
            compressed_path = f"output/{cod}/{test}"
            decompressed_path = f"decompressed/{cod}/{test}"

            # Compression and decompression commands for lzw
            if cod == "lzw":
                compress_command = [f"./{cod}/{cod} -c {input_path} -o {compressed_path}"]
                decompress_command = [f"./{cod}/{cod} -d {compressed_path} -o {decompressed_path}"]
            # Compression and decompression commands for RLE
            elif cod == "RLE":
                compress_command = [f"./{cod}/{cod} -c {input_path} {compressed_path}"]
                decompress_command = [f"./{cod}/{cod} -d {compressed_path} {decompressed_path}"]
            # Default compression and decompression commands
            else:
                compress_command = [f"./{cod}/{cod}Compress {input_path} {compressed_path}"]
                decompress_command = [f"./{cod}/{cod}Decompress {compressed_path} {decompressed_path}"]

            # Run the compress command and start a timer for the test
            start_compress_time = time.time()
            subprocess.run(compress_command, shell=True, check=True)
            end_compress_time = time.time()
            compress_execution_time = end_compress_time - start_compress_time

            # Run the decompress command and start another timer for the test
            start_decompress_time = time.time()
            subprocess.run(decompress_command, shell=True, check=True)
            end_decompress_time = time.time()
            decompress_execution_time = end_decompress_time - start_decompress_time

            # Compare the decompressed file with the input ones to check if the compression is lossless
            are_files_identical = filecmp.cmp(input_path, decompressed_path)

	    # Calculate the compression ratio
            input_size = os.path.getsize(input_path)
            compressed_size = os.path.getsize(compressed_path)
            compression_ratio = compressed_size / input_size

            # Write the data about the tests and the check for lossless compression in the log files
            size_log_file_cod.write(f"Test: {test}, Input size: {input_size}, Compressed size: {compressed_size}, Compression ratio: {compression_ratio:.6f}, Compression exec time: {compress_execution_time:.6f}, Decompression exec time: {decompress_execution_time:.6f}\n")
            log_file.write(f"Test: {test}, Is loseless: {are_files_identical}\n")

	    # Print in terminal a check for every test to know we are progressing
            print(f"{cod} {test} ........ Done\n(Compress: {compress_execution_time:.6f} seconds, Decompress: {decompress_execution_time:.6f} seconds, Compression Ratio: {compression_ratio:.6f})")
    
    # Clean the dependencies and binaries
    subprocess.run(["make", "-C", f"{cod}", "clean"], check=True)


