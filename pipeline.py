# import subprocess
import json
import os
import shutil
from pathlib import Path

from python.convert_fits import convert_file_fits
from python.logger import logger

PROJECT_DIR = Path(".")
EXECUTABLE = PROJECT_DIR / "build" / "jupiter"


def get_file_identifier(meta_path: Path):
    with open(meta_path, "r") as metadata_file:
        data = json.load(metadata_file)
        date_prefix: str = data["date_prefix"]
        filter: str = data["filter"]
    return date_prefix + "-Jupiter_" + filter


def get_camera_file(file_identifier: str, input_dir: Path):
    camera_filename = file_identifier + ".CameraSettings.txt"
    # if not (input_dir / camera_filename).exists():
    #     print("camera metdata file not found")
    print(str(input_dir / camera_filename))
    return input_dir / camera_filename


def run_convert_fits_pipeline(input_dir: Path, png_dir: Path, output_dir: Path):
    params = None
    png_meta_file = Path()
    stacked_png_file = Path()
    sharpened_png_file = Path()
    file_identifier = ""

    for folder_path in png_dir.iterdir():
        if os.path.isfile(folder_path):
            continue
        for filepath in folder_path.iterdir():
            if filepath.name.endswith("stacked.png"):
                stacked_png_file = filepath
            if filepath.name.endswith("sharpened.png"):
                sharpened_png_file = filepath
            if filepath.name.endswith(".json"):
                file_identifier = get_file_identifier(filepath)

                png_meta_file = filepath

        camera_file = get_camera_file(file_identifier, input_dir)  # need to implement

        params = convert_file_fits(
            png_path=stacked_png_file,
            cmr_path=camera_file,
            output_path=output_dir,
            meta_path=png_meta_file,
            params=params,
        )


def main():
    # _ = subprocess.run([str(EXECUTABLE)], check=True)
    print("video processing complete")
    with open(file="config.json", mode="r") as config_json:
        config = json.load(config_json)
        output_dir = Path(config["files"]["output_dir"])
        video_dir = Path(config["files"]["video_dir"])
        png_dir = Path(config["files"]["png_dir"])
        output_dir.parent.mkdir(parents=True, exist_ok=True)
        logger.info(
            "Converting files in "
            + str(video_dir)
            + " and"
            + str(dir)
            + "into unprocessed L1 fits"
        )

        run_convert_fits_pipeline(
            input_dir=video_dir,
            png_dir=png_dir,
            output_dir=output_dir,
        )
        shutil.copy2("config.json", output_dir)

        logger.info("wrote unprocessed fits files into " + str(output_dir))


if __name__ == "__main__":
    main()
