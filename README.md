# Video Processing Repository for Jupiter Atmospheric Structure Research

## Description

This repository contains scripts to process, align, and stack .avi scientific videos of Jupiter into .png files. Its purpose is to provide software to automate the video processing for multiple scientific observations of Jupiter

## Usage

To retrieve the code with its dependencies, run : `git clone --recurse-submodules <REPO_URL>`
The pipeline can be compiled with the `make` command and run with the `./main` command. The pipeline requires.

### Setting up dependencies

Inside of the external/wavelib directory run `cmake .` and then `make`

### building the project

In the root directory run:

cmake -S . -B build
cmake --build build -j

## Methodology

The image processing pipeline consists of four main steps:

- selecting frames to stack
- aligning these frames to a reference frame
- stacking frames
- sharpening

### Selecting frames to stack

The processing pipeline selects the 25% sharpest frames to stack. In order to select these frames, it uses the average gradient magnitude calculated over the surface of Jupiter. Higher average gradient magnitude corresponds to a higher contrasst image.

### Aligning frames

Once we the frames to stack are selected, the next step is to align them. The reference frame is chosen as the sharpest (highest average gradient magnitude) frame. All other frames are aligned so that their centroid sits at the same coordinates as the centroid of this reference frame.

### Stacking Frames

The frames are then stacked by computing the average of the selected frames. The stacked output is 16-bit.

### Post-processing

The background noise is then computed as the average pixel intensity of all pixels not on the disk. This value is subtracted from the image

### Sharpening

Sharpening is performed by doing wavelet decomposition on the output and sharpening at certain spatial scales. We use the Daubechies-4 wavelet decomposition as default. We also use a stationary wavelet transform to prevent image translation from affecting sharpening.

## Tools

- OpenCV
