from pathlib import Path

from python.convert_fits import convert_png_fits

PROJECT_DIR = Path(".")
EXECUTABLE = PROJECT_DIR / "build" / "jupiter"


def main():
    # _ = subprocess.run([str(EXECUTABLE)], check=True)
    print("video processing complete")

    png_path = "output"
    # png_files = os.listdir(png_path + "/20260318UT")
    # fileMap: dict[Any, Any] = H.getL1AProcessingFiles(png_files)
    output_path = "FITS"
    convert_png_fits("20260318UT", png_path, output_path)
    print("completed convertsion to unprocessed L1 fits")


if __name__ == "__main__":
    main()
