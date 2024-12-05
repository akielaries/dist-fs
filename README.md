# dist-fs
This is a distributed filesystem (with the distributed part coming later). The goal of this project is for 
anyone to mount their own drive to a Linux-based device and treat the drive as storage, run a webserver that
serves a frontend, and upload, download, and delete files like a traditional file hosting utility like Dropbox.
This was created to replace my (and Chicothebear's) need for sharing files via iMessage, email, and Dropbox. 

> [!WARNING] Don't run this on drives that have important data, this will erase them and step all over the
> existing data living on that drive.

Easy build with CMake:
```
mkdir build && cd build
cmake ../
make
```

# Usage
```
Usage: ./dist-fs [OPTIONS]
Options:
  -u, --upload <file>      Upload the specified file to the SSD
  -d, --download <file>    Download the specified file from the SSD
  -D, --delete <file>      Delete the specified file from the SSD
  -l, --list               List all files on the SSD
  -S, --ssd_echo <pattern> Perform an echo test on the SSD with a specified hex pattern (up to 16 bytes)
  -r, --reset <offset> <size> Reset a section of the SSD starting at the specified offset with the given size

Examples:
  ./dist-fs -u example.wav  # upload
  ./dist-fs -d example.wav  # download
  ./dist-fs -D example.wav  # delete
  ./dist-fs --ssd_echo ABABABAB
  ./dist-fs -r 1024 512

Note: <file> must be specified for upload, download, and delete operations.
```

# How it works
The filesystem is relatively simple and naive. There is a max of `1024` files that this software can keep
track of. The beginning of the drive is used for the metadatable that keeps track of files: 
```c
typedef struct {
  char filename[256]; // name
  off_t start_offset; // offset on ssd
  size_t size;        // file size in bytes
} ssd_metadata_t;
```
The first 278528 bytes which is `sizeof(ssd_metadata_t) * max files` is reserved for the metadata table
and looks like the following:
```
$ hexdump -s 0x0 -C -n 512 /dev/disk/by-id/usb-Seagate_Slim_SL_NA710NYN-0:0
00000000  2f 68 6f 6d 65 2f 61 6b  69 65 6c 2f 34 5f 79 6f  |/home/akiel/4_yo|
00000010  75 5f 72 6f 75 67 68 32  5f 73 65 72 65 6e 69 74  |u_rough2_serenit|
00000020  79 2e 77 61 76 00 00 00  00 00 00 00 00 00 00 00  |y.wav...........|
00000030  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
00000100  00 40 04 00 00 00 00 00  ee 0c 81 01 00 00 00 00  |.@..............|
00000110  2f 68 6f 6d 65 2f 61 6b  69 65 6c 2f 34 5f 79 6f  |/home/akiel/4_yo|
00000120  75 5f 72 6f 75 67 68 32  5f 73 65 72 65 6e 69 74  |u_rough2_serenit|
00000130  79 2e 6d 34 61 00 00 00  00 00 00 00 00 00 00 00  |y.m4a...........|
00000140  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
```
Our file name `/home/akiel/4_you_rough2_serenity.wav` corresponds to the following hex characters. Keep in mind endianness matters!
```
2f 68 6f 6d 65 2f 61 6b  69 65 6c 2f 34 5f 79 6f
75 5f 72 6f 75 67 68 32  5f 73 65 72 65 6e 69 74
79 2e 77 61 76
```
The offset of the file data in the drive is located at `0x44000`, using the above hexdump corresponds to:
```
00 40 04 00
```
And finally the size of the data in bytes is 25234670 bytes or `1810CEE` in hex:
```
ee 0c 81 01
```
