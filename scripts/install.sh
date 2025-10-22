#!/usr/bin/bash

set -euo pipefail

# Constants
REPO="vicinaehq/vicinae"
INSTALL_DIR="$HOME/.local/vicinae"
BIN_DIR="$HOME/.local/bin"
BINARY_NAME="vicinae"
TEMP_DIR="/tmp"

TEMP_FILES=()
PRESERVE_FILES=()

cleanup() {
	for file in "${TEMP_FILES[@]}"; do
		if [[ -e "$file" ]]; then
			rm -rf "$file"
		fi
	done
	# PRESERVE_FILES are kept for future runs and only cleaned up after successful installation
}

trap cleanup EXIT ERR

renderIcon() {
	cols=$(tput cols)

	icon=$(
		cat <<'EOF'
............
.....    ..........
...       .............
...       .................
...        ...................
..        ....................
..        ....................
..        ....................
...        ...................
..        ...................
...       .................
...       .............
....     ..........
.............
EOF
	)

	tagline="vicinae - A focused launcher for your desktop — native, fast, extensible"

	while IFS= read -r line; do
		line_len=${#line}
		padding=$(((cols - line_len) / 2))
		printf "%*s%s\n" "$padding" "" "$line"
	done <<<"$icon"

	echo

	tagline_len=${#tagline}
	padding=$(((cols - tagline_len) / 2))
	printf "%*s%s\n\n" "$padding" "" "$tagline"
}

check_dependencies() {
	echo "Checking dependencies..."

	if ! command -v curl >/dev/null 2>&1; then
		echo "Error: curl is required but not installed."
		echo "Please install curl and try again."
		exit 1
	fi

	if ! command -v jq >/dev/null 2>&1; then
		echo "Error: jq is required but not installed."
		echo "Please install jq and try again."
		exit 1
	fi

	echo "✓ All dependencies found"
}

get_latest_release_info() {
	echo "Fetching latest release info from GitHub..." >&2
	local api_url="https://api.github.com/repos/$REPO/releases/latest"

	local response
	response=$(curl -s "$api_url")

	local tag_name
	tag_name=$(echo "$response" | jq -r '.tag_name')

	if [[ "$tag_name" == "null" || -z "$tag_name" ]]; then
		echo "Error: Failed to fetch latest version from GitHub API" >&2
		exit 1
	fi

	local appimage_name
	appimage_name=$(echo "$response" | jq -r '.assets[] | select(.name | contains("AppImage")) | .name')

	if [[ "$appimage_name" == "null" || -z "$appimage_name" ]]; then
		echo "Error: Failed to find AppImage asset in latest release" >&2
		exit 1
	fi

	echo "✓ Latest version: $tag_name" >&2
	echo "✓ AppImage asset: $appimage_name" >&2
	echo "$tag_name|$appimage_name"
}

get_installed_version() {
	if [[ ! -f "$BIN_DIR/$BINARY_NAME" ]]; then
		echo "none"
		return
	fi

	local version_output
	if version_output=$("$BIN_DIR/$BINARY_NAME" version 2>/dev/null); then
		local version
		version=$(echo "$version_output" | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -1)
		if [[ -n "$version" ]]; then
			echo "v$version"
		else
			echo "unknown"
		fi
	else
		echo "unknown"
	fi
}

compare_versions() {
	local installed="$1"
	local latest="$2"

	if [[ "$installed" == "none" || "$installed" == "unknown" ]]; then
		return 0 # Need to install
	fi

	local installed_clean="${installed#v}"
	local latest_clean="${latest#v}"

	if [[ "$installed_clean" != "$latest_clean" ]]; then
		return 0 # Need to update
	fi

	return 1 # Already up to date
}

download_appimage() {
	local version="$1"
	local appimage_name="$2"
	local download_path="$TEMP_DIR/$appimage_name"
	local download_url="https://github.com/$REPO/releases/download/$version/$appimage_name"

	if [[ -f "$download_path" && -x "$download_path" ]]; then
		echo "✓ Using cached AppImage: $download_path" >&2
		PRESERVE_FILES+=("$download_path")
		echo "$download_path"
		return 0
	fi

	echo "Downloading Vicinae $version..." >&2
	echo "Asset: $appimage_name" >&2
	echo "URL: $download_url" >&2

	if curl -L --progress-bar "$download_url" -o "$download_path"; then
		chmod +x "$download_path"
		PRESERVE_FILES+=("$download_path") # Keep for future runs
		echo "✓ Download completed: $download_path" >&2
		echo "$download_path"
	else
		echo "Error: Failed to download AppImage" >&2
		exit 1
	fi
}

extract_appimage() {
	local appimage_path="$1"
	local extract_dir="$INSTALL_DIR.extract"

	echo "Extracting AppImage..." >&2

	rm -rf "$extract_dir"

	mkdir -p "$extract_dir"

	cd "$extract_dir"

	echo "This may take a moment..." >&2
	"$appimage_path" --appimage-extract >"$TEMP_DIR/appimage-extract.log" 2>&1
	local extract_status=$?

	if [[ $extract_status -eq 0 ]]; then
		if [[ -d "squashfs-root" ]]; then
			mv squashfs-root/* squashfs-root/.[!.]* . 2>/dev/null || true
			rmdir squashfs-root 2>/dev/null || rm -rf squashfs-root
			echo "✓ Extraction completed" >&2
			echo "$extract_dir"
		else
			echo "Error: squashfs-root directory not found after extraction" >&2
			ls -la >&2
			exit 1
		fi
	else
		echo "Error: Failed to extract AppImage (exit code: $extract_status)" >&2
		echo "Note: Make sure you have a compatible AppImage runtime installed" >&2
		exit 1
	fi
}

install_vicinae() {
	local extract_dir="$1"

	echo "Installing Vicinae..." >&2

	mkdir -p "$BIN_DIR"

	local binary_path="$extract_dir/usr/bin/$BINARY_NAME"
	if [[ ! -f "$binary_path" ]]; then
		binary_path="$extract_dir/AppRun"
	fi

	if [[ -f "$binary_path" ]]; then
		rm -rf "$INSTALL_DIR"

		mv "$extract_dir" "$INSTALL_DIR"

		binary_path="${binary_path/$extract_dir/$INSTALL_DIR}"

		ln -sf "$binary_path" "$BIN_DIR/$BINARY_NAME"

		# After successful installation, mark the AppImage for cleanup
		# (we pass the appimage_path as a second argument now)
		if [[ -n "${2:-}" ]]; then
			TEMP_FILES+=("$2")
		fi

		echo "✓ Installation completed" >&2
	else
		echo "Error: Vicinae binary not found in extracted files" >&2
		echo "Looking in: $extract_dir" >&2
		ls -la "$extract_dir/" 2>&1 | head -10 >&2
		exit 1
	fi
}

uninstall_vicinae() {
	echo "Uninstalling Vicinae..."

	if [[ -d "$INSTALL_DIR" ]]; then
		rm -rf "$INSTALL_DIR"
		echo "✓ Removed installation directory: $INSTALL_DIR"
	else
		echo "No installation directory found"
	fi

	if [[ -L "$BIN_DIR/$BINARY_NAME" ]]; then
		rm -f "$BIN_DIR/$BINARY_NAME"
		echo "✓ Removed binary symlink: $BIN_DIR/$BINARY_NAME"
	else
		echo "No binary symlink found"
	fi

	echo "✓ Vicinae has been uninstalled"
}

show_usage() {
	echo "Usage: $0 [OPTIONS]"
	echo ""
	echo "Options:"
	echo "  --uninstall    Uninstall Vicinae"
	echo "  --help, -h     Show this help message"
	echo ""
	echo "Without options, the script will install or update Vicinae to the latest version."
}

main() {
	renderIcon

	case "${1:-}" in
	--uninstall)
		uninstall_vicinae
		exit 0
		;;
	--help | -h)
		show_usage
		exit 0
		;;
	"")
		# No arguments, proceed with install/update
		;;
	*)
		echo "Error: Unknown option '$1'"
		show_usage
		exit 1
		;;
	esac

	check_dependencies

	local release_info
	release_info=$(get_latest_release_info)

	local latest_version
	local appimage_name
	IFS='|' read -r latest_version appimage_name <<<"$release_info"

	local installed_version
	installed_version=$(get_installed_version)

	echo "Installed version: $installed_version"
	echo "Latest version: $latest_version"

	# Check if update is needed
	if compare_versions "$installed_version" "$latest_version"; then
		if [[ "$installed_version" == "none" ]]; then
			echo "Installing Vicinae for the first time..."
		else
			echo "Updating Vicinae from $installed_version to $latest_version..."
		fi

		local appimage_path
		appimage_path=$(download_appimage "$latest_version" "$appimage_name")

		local extract_dir
		extract_dir=$(extract_appimage "$appimage_path")

		install_vicinae "$extract_dir" "$appimage_path"

		echo ""
		echo "🎉 Vicinae $latest_version has been successfully installed!"
		echo "You can now run 'vicinae' from anywhere in your terminal."
	else
		echo "✓ Vicinae is already up to date ($installed_version)"
	fi
}

main "$@"
