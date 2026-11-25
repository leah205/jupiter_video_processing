import json
import os
import subprocess
import time
from pathlib import Path

from python.convert_fits import run_convert_fits_batch
from python.logger import initialize_logger, logger

PROJECT_DIR = Path(".")
EXECUTABLE = PROJECT_DIR / "build" / "jupiter"


def run_pipeline():
    start = time.perf_counter()

    with open(file="config/config.json", mode="r") as config_json:
        config = json.load(config_json)
        date_string = config["date_string"]
        output_dir = Path(config["fits_output_dir"]) / date_string
        video_dir = Path(config["video_input_dir"]) / date_string  # pyright: ignore[reportAny]
        png_dir = Path(config["png_output_dir"]) / date_string
        log_file = Path(config["log_file"])

        os.makedirs(output_dir, exist_ok=True)

        initialize_logger(str(log_file))
        logger.info("")
        logger.info("")

        # run video processing cpp
        _ = subprocess.run([str(EXECUTABLE)], check=True)
        logger.info("")

        # processL1X
        run_convert_fits_batch(
            input_dir=video_dir, png_dir=png_dir, output_dir=output_dir
        )
    duration = time.perf_counter() - start
    logger.info("")
    logger.info(f"total duration of pipeline: {duration:.4f} seconds")


if __name__ == "__main__":
    run_pipeline()
