# Installation Guide

## Prerequisites

Make sure you have the following installed on your system:
- Python 3.6 or higher
- `pip` (Python package installer)
- `virtualenv` (optional but recommended)

## Steps to Install

1. **Clone the repository:**
```
   git clone https://github.com/yourusername/yourrepository.git
   cd yourrepository
```
2. **Create a virtual enviroment**
```
python -m venv venv
source venv/bin/activate
```
3. Install conan for dependencies, generate profile and build

```
pip install conan
conan profile detect --force
conan install . --output-folder=build --build=missing
```
4. **Compile and build system**

```
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=./build/Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```
5. **Run the shell**

```
./ShellProject
```
