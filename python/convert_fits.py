import os
import sys

import planetmapper

import python.helpers as H

First = True
params = []


def convert_png_fits(obskey, input_path, output_path):
    sys.path.append("../processes")

    # path="../Data_Samples/" + obskey + "/"
    # path = input_path + "/" + obskey + "/"
    path = input_path + "/" + obskey + "/"
    l1Files = os.listdir(path)
    # print(path)
    # print(l1Files)
    obs_map = H.getL1AProcessingFiles(l1Files)
    print(obs_map)

    # planetmapper.set_kernel_path("~/Jupiter")

    # outputs fits into unprocessed_l1 file
    def createFits(file_list, camera_obs_list, obs):
        global First
        global params
        for i, fn in enumerate(file_list):
            time = fn[0:10] + "T" + fn[11:13] + ":" + fn[13:15]
            observation = planetmapper.Observation(
                path + fn, target="jupiter", utc=time
            )
            # print("1##########observation.backplanes=",list(observation.backplanes.keys()))
            params
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
            # print("******************")
            # print("2##########observation.backplanes=",observation.backplanes.keys())

            if First:
                coords = observation.run_gui()
                params = observation.get_disc_params()
                print("######### params1=", params)
                First = False
            else:
                observation.set_disc_params(params[0], params[1], params[2], params[3])

            # observation.add_header_metadata()
            # observation.append_to_header('HEIRARCH SHRPCAP '+'TESTKEY','This is a test',hierarch_keyword=False)
            # filetype = fn[fn.index('_') + 1: fn.index('-')]

            # populate header with camera metadata
            camera_file = camera_obs_list[i]

            with open(path + camera_file, "r") as cf:
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

            dir_path = output_path + "/" + obskey + "/" + obs + "/unprocessed_L1/"
            os.makedirs(dir_path, exist_ok=True)
            observation.save_observation(dir_path + "/" + fn.replace(".png", ".fits"))
            observation.save_mapped_observation(
                dir_path + "/" + fn.replace(".png", "map.fits")
            )

    # creates fits files for each observation
    for obs in obs_map:
        file_list = obs_map[obs]
        camera_obs_list = H.getCameraObservations(l1Files)["data"][obs]
        createFits(file_list, camera_obs_list, obs)
        First = False


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
