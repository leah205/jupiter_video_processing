import json
import os
import shutil
import time
import warnings
from collections import defaultdict
from pathlib import Path
from typing import Any

import planetmapper
from astropy.io.fits.verify import VerifyWarning
from planetmapper.observation import Observation

from python.logger import initialize_logger, logger

First = True
params = []

warnings.filterwarnings("ignore", category=VerifyWarning)


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


def run_convert_fits_batch(input_dir: Path, png_dir: Path, output_dir: Path):
    start = time.perf_counter()
    fail_count = 0
    success_count = 0
    params = None
    png_meta_file = None
    stacked_png_file = None
    sharpened_png_file = None
    logger.info("running processL1X on: " + str(png_dir))

    groups = group_files_by_prefix(png_dir)

    for prefix, files in groups.items():
        for file in files:
            filepath = str(png_dir) + "/" + file
            if file.endswith(".png") and (not "WV" in file):
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
                # logger.error(f"could not find {file_type} file for {prefix}")
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
            except Exception as e:
                logger.error(f"failed to convert {file} into unprocessed fits: {e}")
                fail_count += 1

        shutil.copy2("config/config.json", output_dir)
    duration = time.perf_counter() - start

    logger.info(f"files processed: {success_count}")
    logger.info(f"files failed: {fail_count}")
    logger.info("wrote unprocessed L1 fits files to " + str(output_dir))
    logger.info(f"Duration of processL1X: {duration:.4f} seconds")


def create_observation(input_file, time) -> Observation:
    observation = planetmapper.Observation(
        path=str(input_file), target="jupiter", utc=time
    )
    # print("1##########observation.backplanes=",list(observation.backplanes.keys()))

    del observation.backplanes["DOPPLER"]
    del observation.backplanes["LON-CENTRIC"]
    del observation.backplanes["LAT-CENTRIC"]
    del observation.backplanes["RA"]
    del observation.backplanes["DEC"]
    del observation.backplanes["KM-X"]
    del observation.backplanes["KM-Y"]
    del observation.backplanes["RING-RADIUS"]
    del observation.backplanes["RING-LON-GRAPHIC"]
    del observation.backplanes["RING-DISTANCE"]
    del observation.backplanes["LIMB-LON-GRAPHIC"]
    del observation.backplanes["LIMB-DISTANCE"]
    del observation.backplanes["RADIAL-VELOCITY"]
    del observation.backplanes["LOCAL-SOLAR-TIME"]
    del observation.backplanes["AZIMUTH"]
    del observation.backplanes["PHASE"]
    del observation.backplanes["LIMB-LAT-GRAPHIC"]
    del observation.backplanes["ANGULAR-X"]
    del observation.backplanes["ANGULAR-Y"]
    del observation.backplanes["PIXEL-X"]
    del observation.backplanes["PIXEL-Y"]
    del observation.backplanes["DISTANCE"]
    return observation


def add_metadata(
    observation: Observation, cmr_path: Path | None, meta_path: Path | None
):
    stack_num = 0
    if meta_path:
        with open(meta_path, "r") as metadata_file:
            data = json.load(metadata_file)
            stack_num: int = data["stacked_frames"]
    else:
        logger.warning("failed to add some headers:" + str(meta_path) + " not found")

    if cmr_path:
        with open(str(cmr_path), "r") as cf:
            for line in cf:
                pair = line.strip()
                if "iOptron" in pair:
                    startIndex = pair.index("=")
                    commaIndex = pair.find(",", startIndex)
                    value1 = pair[startIndex + 4 : commaIndex]
                    value2 = pair[pair.find("=", commaIndex) + 1 :]
                    observation.append_to_header(
                        "SHRPCAP RA", formatType(value1), hierarch_keyword=False
                    )
                    observation.append_to_header(
                        "SHRPCAP Dec", formatType(value2), hierarch_keyword=False
                    )

                elif "=" in pair:
                    key = pair[: pair.index("=")]
                    value = pair[pair.index("=") + 1 :]
                    observation.append_to_header(
                        "SHRPCAP " + key, formatType(value), hierarch_keyword=False
                    )
                    if key == "Exposure" and stack_num:
                        print(value)
                        exposure_ms = 0
                        if not "ms" in value:
                            exposure_s = float(value.split("s")[0])
                            observation.header["Total_Exposure"] = (
                                str(stack_num * exposure_s) + "s"
                            )
                        else:
                            exposure_ms = float(value.split("ms")[0])
                            observation.header["Total_Exposure"] = (
                                str(stack_num * exposure_ms) + "ms"
                            )

    else:
        logger.warning("failed to add some headers:" + str(cmr_path) + " not found")
    observation.header["NUMSTACK"] = stack_num


def convert_file_fits(
    png_path: Path,
    cmr_path: Path | None,
    output_path: Path,
    meta_path: Path | None,
    params,
) -> tuple[float, float, float, float] | Any:
    fn = png_path.name
    time = fn[0:10] + "T" + fn[11:13] + ":" + fn[13:15]
    observation = create_observation(png_path, time)

    if not params:
        coords = observation.run_gui()
        params = observation.get_disc_params()
        print("######### params1=", params)
    else:
        observation.set_disc_params(params[0], params[1], params[2], params[3])

    add_metadata(observation, cmr_path, meta_path)

    output_path = output_path / "unprocessed_L1"
    os.makedirs(output_path, exist_ok=True)
    observation.save_observation(output_path / fn.replace(".png", ".fits"))
    observation.save_mapped_observation(output_path / fn.replace(".png", "map.fits"))
    return params


def formatType(value):
    """
    Converts string content to proper type
    Parameters:
    value (string): to be converted
    Returns: (float, int, or string)
    """
    if value.isdigit():
        return int(value)
    if value.replace(".", "", 1).isdigit() and value.count(".") == 1:
        return float(value)

    return value.strip()


if __name__ == "__main__":
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

        # processL1X
        run_convert_fits_batch(
            input_dir=video_dir, png_dir=png_dir, output_dir=output_dir
        )
