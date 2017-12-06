for build option

-- if use CUDA
-c++
add below paths to additional directory
$(CudaToolkitIncludeDir);$(cudnn)\include;

-linker
add below paths to additional directory 
$(cudnn)\lib\x64;$(CUDA_PATH)lib\$(PlatformName);

add below paths to addtional dependencies
yolo_cpp_dll.lib;



--common
-c++
add below paths to additional directory (for openCV version3)
C:\OpenCV\3.3.0\build\install\include;C:\OpenCV\3.3.0\build\install\include\opencv;C:\OpenCV\3.3.0\build\install\include\opencv2

-linker
add below paths to additional directory 
C:\OpenCV\3.3.0\build\install\x64\vc14\lib;%(AdditionalLibraryDirectories)

add below paths to addtional dependencies
opencv_world330.lib;
