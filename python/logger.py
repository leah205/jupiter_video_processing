import logging

logging.basicConfig(
    filename="LogFile.txt",
    level=logging.INFO,
    format="%(asctime)s | %(levelname)s | %(message)s",
    datefmt="%Y-%m-%d %H:%M:%S",
    filemode="a",
)


logger = logging.getLogger(__name__)
