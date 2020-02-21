all: watch
update:
	git -C ./ pull
	git -C ./.pio/libdeps/watch/Smol-Device-Operating-System pull

build:
	@rm -Rf .pio/build/watch/lib324
	platformio run --environment watch
program:
	platformio run --environment watch --jobs 1 --target upload
program-ota:
	platformio run --environment watch --jobs 1 --upload-port 192.168.43.178 --target upload
monitor:
	platformio run --environment watch --jobs 1 --target monitor
watch: build program monitor

watch-data:
	platformio run --environment watch --jobs 1 --target buildfs --target uploadfs
