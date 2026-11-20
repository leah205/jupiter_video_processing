import logging

logger = logging.getLogger("pipeline")


def initialize_logger(output_dir: str) -> None:

    logger.setLevel(logging.INFO)

    handler = logging.FileHandler(output_dir + "/LogFile.txt", mode="a")

    handler.setLevel(logging.INFO)

    handler.setFormatter(
        logging.Formatter(
            "%(asctime)s | %(levelname)s | %(message)s", datefmt="%Y-%m-%d %H:%M:%S"
        )
    )
    logger.addHandler(handler)
