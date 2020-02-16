all: watch
watch:
	#platformio run --environment TTGO_WATCH --silent --target clean
	platformio run --environment watch --jobs 1 --target upload --target monitor
