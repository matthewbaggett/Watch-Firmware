update:
	pio lib update
	git -C ./ pull
	-git -C ./.pio/libdeps/watch/Smol-Device-Operating-System pull
	-git -C ./.pio/libdeps/m5stack/Smol-Device-Operating-System pull
clear:
	clear
clean:
	-astyle -r "*.hpp" "*.cpp" "*.h"
	find . -type f -name '*.orig' -delete
prepare: clear install-hooks update clean clear

# Development related
install-hooks:
	cp -v .githooks/* .git/hooks/

# T-watch related
clean-t-watch:
	pio run -e watch -t clean
build-t-watch:
	@rm -Rf .pio/build/watch/lib324
	pio run -e watch
program-t-watch-data:
	pio run -e watch -t buildfs -t uploadfs
program-t-watch:
	pio run -e watch -t upload -t monitor
t-watch: clean-t-watch prepare build-t-watch program-t-watch-data program-t-watch


# M5StackC related.
clean-m5watch:
	pio run -e m5stackcplus -t clean
build-m5watch:
	pio run -e m5stackcplus
program-m5watch-data:
	pio run -e m5stackcplus -t buildfs -t uploadfs
program-m5watch:
	pio run -e m5stackcplus -t upload -t monitor
m5watch: clean-m5watch prepare build-m5watch program-m5watch-data program-m5watch