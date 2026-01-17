firefox:
	mkdir -p build
	cp -r chrome build/firefox
	cp ./manifest.firefox.json build/firefox/manifest.json
	web-ext build -a build/ -s build/firefox --overwrite-dest --filename firefox-vicinae.zip
.PHONY: firefox
