override PROJECT = $(shell git config remote.origin.url | xargs basename | cut -d '.' -f1)
override HEAD = $(shell git rev-parse HEAD)

PYTHON_PACKAGES ?= chordnovarw
APP_NAME ?= chordnovarw

# Determine OS. Currently only support Mac.
UNAME_S := $(shell uname -s) # FIXME: This may fail on Windows

override MAKE = $(shell which make)
override CMAKE = $(shell which cmake)

.PHONY: all
all: usage

.PHONY: usage
usage:
	@echo "\033[1m\033[93mBuild System\033[0m"
	@echo
	@echo "\033[93mFrequently used workflow\033[0m"
	@echo
	@echo "    make build"
	@echo "        \033[90m- build ChordNova Re-Write \033[0m"
	@echo
	@echo "    make doc"
	@echo "        \033[90m- Generate documentation using doxygen \033[0m"
	@echo
	@echo "    make test"
	@echo "        \033[90m- try a specific test \033[0m"
	@echo
	@echo "\033[95mConstants\033[0m"
	@echo "\033[90m"
	@echo "    PROJECT=\"${PROJECT}\" # project name"
	@echo "    HEAD=\"${HEAD}\" # git hash of repo"
	@echo "\033[0m"

.PHONY: doc
doc:
	@doxygen Doxyfile

.PHONY: test
test:
	@${CMAKE} --build ./cmake-build-debug --target chord_nova_gtest -j 6
	@./cmake-build-debug/maintest/chord_nova_gtest --gtest_color=no

.PHONY: test-rw
test-rw:
	@${CMAKE} --build ./cmake-build-debug --target chord_nova_rw_gtest -j 6
	@./cmake-build-debug/test/chord_nova_rw_gtest --gtest_color=no
