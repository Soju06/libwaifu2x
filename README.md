# waifu2x-lib
이 프로젝트는 [waifu2x-ncnn-vulkan](https://github.com/nihui/waifu2x-ncnn-vulkan)에서 포크되었습니다.

|    System     | C/C++ | Csharp | Python ❌ |
| :-----------: | :---: | :----: | :------: |
| Windows 32bit |   ❌   |   ❌    |    --    |
| Windows 64bit |   ✅   |   ✅    |    --    |
|  Linux 32bit  |   ❌   |   ❌    |    --    |
|  Linux 64bit  |   ✅   |   ❌    |    --    |
|     MacOS     |   ⚠️   |   ❌    |    --    |

파이썬은 추후에 추가될 예정입니다.

## Download
| Language | System  | x86  |                 x64                 |
| :------: | :-----: | :--: | :---------------------------------: |
|  C/C++   | Windows |  ❌   |           libwaifu2x.dll            |
|  C/C++   |  Linux  |  ❌   |            libwaifu2x.so            |
|  Csharp  | Windows |  ❌   | libwaifu2x.NET.dll + libwaifu2x.dll |
|  Csharp  |  Linux  |  ❌   |                  ❌                  |
|  Python  | Windows |  --  |                 --                  |
|  Python  |  Linux  |  --  |                 --                  |



## Example

### C++

```cpp
{
    int scale = 2;
    int noise = 1;
    bool tta_mode = false;
    // 프로세서를 가져옵니다.
    auto* processor = Waifu2xProcessor::get(Waifu2xProcessors::Auto);
    // 모델 파일을 읽습니다.
    auto* model = Waifu2xModel::load(Waifu2xModels::CUnet, processor, noise, scale, tta_mode);\

    Waifu2x waifu2x(processor);
    // 모델을 로드합니다.
    waifu2x.loadModel(model);
    // 이미지를 로드합니다.
    auto* input = Waifu2xMat::load(PATH("A:\test\m1.png"));
    // 이미지 프로세싱
    auto* output = waifu2x.process(input, scale);
    // 이미지 저장
    output->save(PATH("A:\test\m1-waifu2x.png"), Waifu2xImageFormat::Png);

    delete input;
    delete output;
}
// waifu2x.nodes_processor가 false이면 waifu2x가 제거될때 같이 제거됩니다. (기본값: false)
// waifu2x.nodes_model이 false이면 waifu2x가 제거될때 같이 제거됩니다. (기본값: false)
```



### C#

```c#
using libwaifu2x;

int scale = 2;
int noise = 1;
bool tta_mode = false;

// 프로세서를 가져옵니다.
using (var processor = Waifu2xProcessor.Get(Waifu2xProcessors.Auto)) {
    // 모델파일을 읽습니다.
    using var model = Waifu2xModel.Load(Waifu2xModels.CUnet, processor, noise, scale, tta_mode);
    
    using var waifu2x = new Waifu2x(processor);
    // 모델을 로드합니다.
    waifu2x.LoadModel(model);
    // 이미지를 로드합니다.
    using var input = Waifu2xMat.Load("A:\test\m1.png");
    // 이미지 프로세싱
    using var output = waifu2x.Process(input, scale);
    // 이미지 저장
    output.Save("A:\test\m1-waifu2x.png", Waifu2xImageFormat::Png);
}
```



#### C# - 빠른 이미지 프로세싱, ``Waifu2xMat`` to ``System.Drawing.Bitmap``

```c#
Bitmap MatToBitmap(Waifu2xMat mat) {
    var bmp = new Bitmap(mat.Width, mat.Height);
    var em = mat.ElementSize;
    var data = bmp.LockBits(new(0, 0, bmp.Width, bmp.Height), 
                                   ImageLockMode.ReadWrite, em == 4 ? PixelFormat.Format32bppArgb : PixelFormat.Format24bppRgb);
    var pdata = mat.GetPixelData(true);
    Marshal.Copy(pdata, 0, data.Scan0, pdata.Length);
    bmp.UnlockBits(data);
    return bmp;
}
```

stb 라이브러리를 사용하지 않고 C# 네이티브 비트맵으로 로드합니다.



## How to Build

### Build from Colab

[here](https://colab.research.google.com/drive/18C_V_OW0d6IpUbrheTClL2HKYhhEdx7f?usp=sharing)

### Common

1. Vulkan SDK를 설치합니다.

   Windows

   ```sh
   https://vulkan.lunarg.com/ 에서 해당 SDK를 설치합니다.
   ```

   Linux (Ubuntu)

   ```sh
   sudo apt install libvulkan-dev
   ```

### Add cmake library

2. 해당 프로젝트로 접근합니다.

   ```sh
   cd myproject
   ```

   

3. 소스코드를 복제합니다.

   소스코드 복제, 서브모듈을 추가합니다.

   ```sh
   git clone https://github.com/Soju06/libwaifu2x.git
   cd libwaifu2x
   git submodule update --init --recursive
   ```

   

   다음과 같이 프로젝트를 설정합니다.

   ```sh
   myproject
     ├─CMakeLists.txt
     ├─libwaifu2x
     │  └─src
     └─main.cpp
   ```

   

4. CMake 설정을 편집합니다.

   다음의 코드를 삽입하세요.

   ```sh
   add_subdirectory(./libwaifu2x/src/ ./libwaifu2x)
   # ...
   # add_executable( ... )
   # ...
   target_include_directories({PROJECT NAME} PRIVATE ./libwaifu2x/src/)
   target_link_libraries({PROJECT NAME} libwaifu2x)
   ```

   

   모델 파일을 출력 폴더에 추가하려면 다음의 코드를 삽입하세요.

   ```sh
   file(COPY ${WAIFU2X_MODEL_DIR} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
   ```

   

   1. CMake 변수

      - WAIFU2X_LINK_LIBRARIES

        libwaifu2x의 링크 라이브러리 리스트입니다.

      - WAIFU2X_MODEL_DIR

        libwaifu2x의 모델 폴더 경로입니다.

        

###  Build a library

2. 소스 코드를 복제합니다.

   ```sh
   git clone https://github.com/Soju06/libwaifu2x.git
   cd libwaifu2x
   git submodule update --init --recursive
   ```

   

3. CMake 빌드합니다.

   ```sh
   cd src
   mkdir out_build
   cd out_build
   cmake ../ -DCMAKE_BUILD_TYPE=Release -DWAIFU2X_BUILD_TYPE:STRING=dynamic_library
   cmake --build .
   ```

   

   1. 빌드 옵션

      - WAIFU2X_BUILD_TYPE

        빌드 옵션을 설정합니다.

        ``executable``, ``dynamic_library``, ``static_library`` 의 값을 사용할 수 있습니다.

      - WAIFU2X_COPY_MODEL_FILE

        모델 파일을 출력 폴더에 복사할지 여부입니다.

        ``true``, ``false`` 의 값을 사용할 수 있습니다,

      - USE_STATIC_MOLTENVK

        MacOS에서 vulkan 라이브러리 링크를 피하기 위한 옵션입니다.

        ``ON``, ``OFF`` 의 값을 사용할 수 있습니다.

        
