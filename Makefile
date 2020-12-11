update:
	git -C ./ pull
	git -C ./.pio/libdeps/watch/Smol-Device-Operating-System pull
clear:
	clear
clean:
	-astyle "src/*.hpp" "src/*.cpp" "src/*.h" "src/hardware/*.h"
	@-rm **/*.orig
prepare: clear update clean clear

install-hooks:
	cp -v .githooks/* .git/hooks/

build-watch:
	@rm -Rf .pio/build/watch/lib324
	pio run -e watch
program-watch:
	pio run -e watch -t upload
program-watch-ota:
	pio run -e watch --upload-port 192.168.43.178 -t upload
monitor-watch:
	pio run -e watch -t monitor
watch: prepare build-watch program-watch monitor-watch

watch-data:
	pio run -e watch -t buildfs -t uploadfs

build-m5watch:
	pio run -e m5stackcplus
program-m5watch:
	pio run -e m5stackcplus -t upload
monitor-m5watch:
	pio run -e m5stackcplus -t monitor

m5watch: prepare build-m5watch program-m5watch monitor-m5watch