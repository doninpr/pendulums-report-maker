FROM gcc:latest

# These commands copy your files into the specified directory in the image
# and set that as the working location
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp

# This command compiles your app using GCC, adjust for your source code
RUN apt-get update; \
    apt-get install -y cmake
RUN cmake -B build -S .
RUN cmake --build build

# This command runs your application, comment out this line to compile only
CMD ["./build/PendulumsReportMaker"]

LABEL Name=PendulumsReportMaker Version=0.0.1