LGC2 is working both on windows (.exe) and Linux (.elf).
For windows, an installer is tipically available.
The executables are self sufficient, they can be moved or copied wherever.

LGC2 can be called using command line, as follow: 

```text
Usage:
    lgc.exe -i inputPath [-o [outputPath]] [-n [maxIterations]] [-D [logFilePath]]
    -----
    -i or -I: argument always needed to set the input path  
              inputPath: absolute or relative input path
    -o or -O: option to set the output path
              outputPath: absolute or relative output path
    -n or -N: option to set the maximal number of iterations for the adjustment process (by default 80). Available from v2.02 version.
    -d or -D: (for developers) option to activate DEBUG level for additional Log files 
              logFilePath: absolute or relative Log file path (when not defined, input file name with "*.log2" extension). Available from v2.02 version.
```

If the relative path is given for the input path, the current directory is used

If no path or the relative path is given for the output , the input directory is used. The file is created at the same place.

The LGC
NB: existing files with the same name will be overwritten.