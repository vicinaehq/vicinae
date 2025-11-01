#!/usr/bin/bash

set -euo pipefail

# Constants
REPO="vicinaehq/vicinae"
INSTALL_DIR="$HOME/.local/vicinae"
BIN_DIR="$HOME/.local/bin"
BINARY_NAME="vicinae"
TEMP_DIR="/tmp"
THEMES_USER_DIR="$HOME/.local/share/vicinae/themes"
APPLICATIONS_USER_DIR="$HOME/.local/share/applications"
SYSTEMD_SERVICE_NAME="vicinae.service"
SYSTEMD_USER_DIR="$HOME/.config/systemd/user"

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

install_themes() {
	echo "Installing themes..." >&2

	local themes_source="$INSTALL_DIR/usr/share/vicinae/themes"

	if [[ -d "$themes_source" ]]; then
		mkdir -p "$THEMES_USER_DIR"

		cp -r "$themes_source"/* "$THEMES_USER_DIR/" 2>/dev/null || true

		if [[ -n "$(ls -A "$THEMES_USER_DIR" 2>/dev/null)" ]]; then
			echo "✓ Themes installed to $THEMES_USER_DIR" >&2
		else
			echo "Note: No themes found in $themes_source" >&2
		fi
	else
		echo "Note: Themes directory not found at $themes_source" >&2
	fi
}

install_desktop_files() {
	echo "Installing desktop application files..." >&2

	local applications_source="$INSTALL_DIR/usr/share/applications"

	if [[ -d "$applications_source" ]]; then
		mkdir -p "$APPLICATIONS_USER_DIR"

		# Copy only vicinae-related desktop files
		if ls "$applications_source"/vicinae*.desktop >/dev/null 2>&1; then
			cp "$applications_source"/vicinae*.desktop "$APPLICATIONS_USER_DIR/" 2>/dev/null || true
			echo "✓ Desktop files installed to $APPLICATIONS_USER_DIR" >&2

			# Update desktop database if available
			if command -v update-desktop-database >/dev/null 2>&1; then
				update-desktop-database "$APPLICATIONS_USER_DIR" 2>/dev/null || true
				echo "✓ Desktop database updated" >&2
			fi
		else
			echo "Note: No vicinae desktop files found in $applications_source" >&2
		fi
	else
		echo "Note: Applications directory not found at $applications_source" >&2
	fi
}

install_systemd_service() {
	echo "Installing systemd user service..." >&2

	local service_source="$INSTALL_DIR/usr/lib/systemd/user/$SYSTEMD_SERVICE_NAME"

	if [[ -f "$service_source" ]]; then
		mkdir -p "$SYSTEMD_USER_DIR"

		# Copy the service file and replace vicinae with absolute path
		local service_dest="$SYSTEMD_USER_DIR/$SYSTEMD_SERVICE_NAME"
		cp "$service_source" "$service_dest"

		# Replace 'vicinae' with absolute path in ExecStart
		sed -i "s|ExecStart=vicinae|ExecStart=$BIN_DIR/$BINARY_NAME|g" "$service_dest"

		echo "✓ Systemd service installed to $service_dest" >&2

		# Reload systemd user daemon if available
		if command -v systemctl >/dev/null 2>&1; then
			systemctl --user daemon-reload 2>/dev/null || true
			echo "✓ Systemd user daemon reloaded" >&2
		fi
	else
		echo "Note: Systemd service file not found at $service_source" >&2
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

		# Symlink node binary if it exists (to avoid conflicts with system node)
		local node_path="$INSTALL_DIR/usr/bin/node"
		if [[ -f "$node_path" ]]; then
			ln -sf "$node_path" "$BIN_DIR/vicinae-node"
			echo "✓ Node.js binary symlinked to $BIN_DIR/vicinae-node" >&2
		fi

		# After successful installation, mark the AppImage for cleanup
		# (we pass the appimage_path as a second argument now)
		if [[ -n "${2:-}" ]]; then
			TEMP_FILES+=("$2")
		fi

		echo "✓ Installation completed" >&2

		# Install themes, desktop files, and systemd service
		install_themes
		install_desktop_files
		install_systemd_service
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

	if [[ -L "$BIN_DIR/vicinae-node" ]]; then
		rm -f "$BIN_DIR/vicinae-node"
		echo "✓ Removed Node.js symlink: $BIN_DIR/vicinae-node"
	fi

	if [[ -d "$THEMES_USER_DIR" ]]; then
		rm -rf "$THEMES_USER_DIR"
		echo "✓ Removed themes directory: $THEMES_USER_DIR"
	fi

	if ls "$APPLICATIONS_USER_DIR"/vicinae*.desktop >/dev/null 2>&1; then
		rm -f "$APPLICATIONS_USER_DIR"/vicinae*.desktop
		echo "✓ Removed desktop files from: $APPLICATIONS_USER_DIR"
		# Update desktop database if available
		if command -v update-desktop-database >/dev/null 2>&1; then
			update-desktop-database "$APPLICATIONS_USER_DIR" 2>/dev/null || true
		fi
	fi

	if [[ -L "$SYSTEMD_USER_DIR/$SYSTEMD_SERVICE_NAME" ]]; then
		# Stop and disable the service if it's running
		if command -v systemctl >/dev/null 2>&1; then
			systemctl --user stop "$SYSTEMD_SERVICE_NAME" 2>/dev/null || true
			systemctl --user disable "$SYSTEMD_SERVICE_NAME" 2>/dev/null || true
		fi
		rm -f "$SYSTEMD_USER_DIR/$SYSTEMD_SERVICE_NAME"
		echo "✓ Removed systemd service: $SYSTEMD_USER_DIR/$SYSTEMD_SERVICE_NAME"
		# Reload systemd user daemon
		if command -v systemctl >/dev/null 2>&1; then
			systemctl --user daemon-reload 2>/dev/null || true
		fi
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
		echo ""

		local local_bin_path="$HOME/.local/bin"
		if [[ ":$PATH:" == *":$local_bin_path:"* ]]; then
			echo "✓ ~/.local/bin is already in your PATH"
			echo "You can now run 'vicinae' from anywhere in your terminal."
		else
			echo "To use Vicinae, add ~/.local/bin to your PATH:"
			echo "  export PATH=\"\$HOME/.local/bin:\$PATH\""
			echo ""
			echo "You can add this to your shell profile (~/.bashrc, ~/.zshrc, etc.) for permanent access."
			echo "Then restart your terminal or run the export command above."
		fi

		echo ""
		echo "Check the quickstart section for your Desktop Environment at https://docs.vicinae.com"
	else
		echo "✓ Vicinae is already up to date ($installed_version)"
	fi
}

main "$@"
