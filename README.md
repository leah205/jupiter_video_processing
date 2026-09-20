# Video Processing Repository for Jupiter Atmospheric Structure Research

## Description

This repository contains scripts to process, align, and stack .avi scientific videos of Jupiter into .png files. Its purpose is to provide software to automate the video processing for multiple scientific observations of Jupiter

## Usage

The pipeline can be compiled with the `make` command and run with the `./main` command.

## Methodology

The image processing pipeline consists of four main steps:

- selecting frames to stack
- aligning these frames to a reference frame
- stacking frames
- sharpening/denoising final result

## Tools

- OpenCV
