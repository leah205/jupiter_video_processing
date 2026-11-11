import json
import os
from pathlib import Path
from typing import Any

import planetmapper
from planetmapper.observation import Observation

from python.logger import logger

First = True
params = []


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


def add_metadata(observation: Observation, cmr_path: Path, meta_path: Path):
    stack_num = 0
    if meta_path.exists():
        with open(meta_path, "r") as metadata_file:
            data = json.load(metadata_file)
            stack_num: int = data["stacked_frames"]
    else:
        logger.warning("failed to add some headers:" + str(meta_path) + " not found")

    if cmr_path.exists():
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
                        exposure_ms = float(value.split("ms")[0])
                        observation.append_to_header(
                            keyword="EXPOSURE",
                            value=str(stack_num * exposure_ms) + "ms",
                        )
    else:
        logger.warning("failed to add some headers:" + str(cmr_path) + " not found")

    observation.append_to_header(keyword="NUMSTACK", value=(stack_num))


def convert_file_fits(
    png_path: Path, cmr_path: Path, output_path: Path, meta_path: Path, params
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
