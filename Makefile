update:
	pio run -t clean
	git -C ./ pull
	git -C ./.pio/libdeps/watch/Smol-Device-Operating-System pull
clear:
	clear
clean:
	-astyle -r "*.hpp" "*.cpp" "*.h"
	find . -type f -name '*.orig' -delete
prepare: clear install-hooks update clean clear

install-hooks:
	cp -v .githooks/* .git/hooks/

build-t-watch:
	@rm -Rf .pio/build/watch/lib324
	pio run -e watch
program-t-watch:
	pio run -e watch -t upload
program-t-watch-ota:
	pio run -e watch --upload-port 192.168.43.178 -t upload
monitor-t-watch:
	pio run -e watch -t monitor
t-watch: prepare build-t-watch program-t-watch monitor-t-watch

t-watch-data:
	pio run -e watch -t buildfs -t uploadfs

build-m5watch:
	pio run -e m5stackcplus
program-m5watch:
	pio run -e m5stackcplus -t upload
monitor-m5watch:
	pio run -e m5stackcplus -t monitor

m5watch: prepare build-m5watch program-m5watch monitor-m5watch