import json
import os
import shutil
import time

# import subprocess
from collections import defaultdict
from pathlib import Path

from python.convert_fits import convert_file_fits
from python.logger import initialize_logger, logger

PROJECT_DIR = Path(".")
EXECUTABLE = PROJECT_DIR / "build" / "jupiter"


def get_file_identifier(meta_path: Path):
    with open(meta_path, "r") as metadata_file:
        data = json.load(metadata_file)
        date_prefix: str = data["date_prefix"]
        filter: str = data["filter"]
    return date_prefix + "-Jupiter_" + filter


def get_camera_file(prefix: str, input_dir: Path) -> Path | None:
    camera_filename = file = next(
        (
            f
            for f in os.listdir(input_dir)
            if f.startswith(prefix) and f.endswith("CameraSettings.txt")
        ),
        None,
    )
    if not camera_filename:
        return None
    return input_dir / camera_filename


def group_files_by_prefix(folder):
    groups = defaultdict(list)

    for filename in os.listdir(folder):
        filepath = os.path.join(folder, filename)

        if os.path.isfile(filepath):
            prefix = filename[:15]
            groups[prefix].append(filename)

    return dict(groups)


def run_convert_fits_pipeline(input_dir: Path, png_dir: Path, output_dir: Path):
    start = time.perf_counter()
    fail_count = 0
    success_count = 0
    params = None
    png_meta_file = None
    stacked_png_file = None
    sharpened_png_file = None

    groups = group_files_by_prefix(png_dir)

    for prefix, files in groups.items():
        for file in files:
            filepath = str(png_dir) + "/" + file
            if file.endswith("stacked.png"):
                stacked_png_file = Path(filepath)
            if file.endswith("WV.png"):
                sharpened_png_file = Path(filepath)
            if file.endswith(".json"):
                png_meta_file = Path(filepath)

        camera_file = get_camera_file(prefix, input_dir)  # need to implement

        if not stacked_png_file:
            logger.error("could not find stacked file for " + prefix)

        files_to_process = {
            "stacked": stacked_png_file,
            "sharpened": sharpened_png_file,
        }

        for file_type, file in files_to_process.items():
            if not file:
                logger.error(f"could not find {file_type} file for {prefix}")
                continue
            try:
                params = convert_file_fits(
                    png_path=file,
                    cmr_path=camera_file,
                    output_path=output_dir,
                    meta_path=png_meta_file,
                    params=params,
                )
                success_count += 1
            except:
                logger.error(f"failed to convert {file} into unprocessed fits")
                fail_count += 1

        duration = time.perf_counter() - start
        logger.info(
            f"{success_count} files successfully converted into unproessed fits, {fail_count} failed"
        )
        logger.info(f"total duration of processL1X: {duration:.4f} seconds")
        logger.info("wrote unprocessed fits files into " + str(output_dir))


def main():

    with open(file="config/config.json", mode="r") as config_json:
        # _ = subprocess.run([str(EXECUTABLE)], check=True)

        config = json.load(config_json)
        date_string = config["date_string"]
        output_dir = Path(config["fits_output_dir"]) / date_string
        video_dir = Path(config["video_input_dir"]) / date_string  # pyright: ignore[reportAny]
        png_dir = Path(config["png_output_dir"]) / date_string

        os.makedirs(output_dir, exist_ok=True)

        initialize_logger(str(output_dir))

        logger.info("running pipeline on " + date_string)

        run_convert_fits_pipeline(
            input_dir=video_dir, png_dir=png_dir, output_dir=output_dir
        )
        shutil.copy2("config/config.json", output_dir)


if __name__ == "__main__":
    main()
