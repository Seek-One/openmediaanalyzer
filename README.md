OpenMediaAnalyzer
============

OpenMediaAnalyzer is a video stream analysis tool available on Windows, Linux and Mac OS X.

- Github: https://github.com/Seek-One/openmediaanalyzer
- Developer website: https://www.seek-one.fr/
- Bugs reporting: https://github.com/Seek-One/openmediaanalyzer/issues

Features
--------
- Compatibility with the H264 and H265 norms.
- Parsing of metadata-related NAL units : 
    - VPS (H265), SPS, PPS units
    - Slice headers
    - AUD (H264), SEI (partial, H264) units
- Field values of the parsable units
- Semantic and structural analysis of the parsable units
- Timeline view of decoded frames
- Image view of valid decoded frames
- Stream loading through files
- Live stream loading through HTTP (with metrics)

License
-------

This program is licensed under the terms of the GNU GENERAL PUBLIC LICENSE Version 3.

Requirements
------------

- CMake
- Qt version 6
- FFmpeg
- ImageMagick
