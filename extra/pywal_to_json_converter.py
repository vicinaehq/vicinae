#!/usr/bin/env python3
"""
Pywal → Vicinae Theme Converter

This script converts Pywal-generated color schemes into a Vicinae-compatible
theme format. It is designed to run automatically whenever a new Pywal theme
is generated, ensuring consistent dynamic theming across applications.

Author: Your Name
"""

import os
import json
import logging
from typing import Any, Dict


# Configure logging for enterprise-grade observability
logging.basicConfig(
    level=logging.INFO,
    format="[%(asctime)s] [%(levelname)s] %(message)s",
    datefmt="%Y-%m-%d %H:%M:%S"
)


def convert_pywal_to_vicinae(pywal_path: str, output_path: str) -> bool:
    """
    Convert a Pywal colors.json file to a Vicinae-compatible theme.

    Args:
        pywal_path (str): Path to the Pywal `colors.json` file.
        output_path (str): Destination file path for the Vicinae theme JSON.

    Returns:
        bool: True if conversion was successful, False otherwise.
    """
    try:
        # Ensure paths are expanded for ~ (home directory)
        pywal_path = os.path.expanduser(pywal_path)
        output_path = os.path.expanduser(output_path)

        # Load Pywal color definitions
        with open(pywal_path, "r", encoding="utf-8") as f:
            pywal: Dict[str, Any] = json.load(f)

        # Map Pywal colors to Vicinae theme format
        vicinae_theme: Dict[str, Any] = {
            "version": "1.0.0",
            "appearance": "dark",
            "icon": "./dynamic-theme.png",
            "name": "Pywal Dynamic",
            "description": "Automatically generated from Pywal",
            "palette": {
                "background": pywal["special"]["background"],
                "foreground": pywal["special"]["foreground"],
                "blue": pywal["colors"]["color4"],
                "green": pywal["colors"]["color2"],
                "magenta": pywal["colors"]["color5"],
                "orange": pywal["colors"]["color11"],
                "purple": pywal["colors"]["color13"],
                "red": pywal["colors"]["color1"],
                "yellow": pywal["colors"]["color3"],
                "cyan": pywal["colors"]["color6"],
            },
        }

        # Ensure output directory exists
        os.makedirs(os.path.dirname(output_path), exist_ok=True)

        # Write theme file (overwrites if already exists)
        with open(output_path, "w", encoding="utf-8") as f:
            json.dump(vicinae_theme, f, indent=2)

        logging.info("Successfully updated Vicinae theme at %s", output_path)
        return True

    except FileNotFoundError:
        logging.error("Pywal colors.json file not found at %s", pywal_path)
    except KeyError as e:
        logging.error("Missing expected key in Pywal colors.json: %s", e)
    except json.JSONDecodeError:
        logging.error("Invalid JSON format in Pywal file: %s", pywal_path)
    except Exception as e:
        logging.exception("Unexpected error during theme conversion: %s", e)

    return False


if __name__ == "__main__":
    # Default paths (can be parameterized for CLI usage)
    pywal_file = "~/.cache/wal/colors.json"
    vicinae_theme_file = "~/.config/vicinae/themes/pywal-theme.json"

    if convert_pywal_to_vicinae(pywal_file, vicinae_theme_file):
        logging.info("Theme updated successfully.")
    else:
        logging.warning("Theme update failed. Ensure Pywal has generated colors.")

