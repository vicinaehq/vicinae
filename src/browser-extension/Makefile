firefox:
	mkdir -p build
	cp -r chrome build/firefox
	cp ./manifest.firefox.json build/firefox/manifest.json
	web-ext build -a build/ -s build/firefox --overwrite-dest --filename firefox-vicinae.zip
.PHONY: firefox

chrome:
	mkdir -p build
	cd chrome && zip -r vicinae.zip *
	mv chrome/vicinae.zip build/
.PHONY: chrome
