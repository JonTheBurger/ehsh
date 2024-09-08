################################################################################
# Project
################################################################################
.DEFAULT_GOAL := help
.PHONY: ${MAKECMDGOALS}
RED := \033[1;31m
BLU := \033[36m
RST := \033[0m

################################################################################
# Variables
################################################################################
## CFLAGS: [""] Additional flags to pass to the C compiler
## CXXFLAGS: [""] Additional flags to pass to the C++ compiler
## LDFLAGS: [""] Additional flags to pass to the linker
## VERBOSE: [0] Set to a value > 0 to increase verbosity

################################################################################
# Goals
################################################################################
help: ## Shows this message
	@printf '${RED}Usage:\n  ${RST}${BLU}make [<VARIABLE>=<value>] <goal>\n${RST}'
	@printf '${RED}Targets:\n${RST}'
	@cat ${MAKEFILE_LIST} | awk -F'(:|##|])\\s*' '/[^:]*:[^:]+##\s.*$$/ {printf "  ${BLU}%-18s${RST} %s\n", $$1, $$3}'
	@printf '${RED}Variables:\n${RST}'
	@cat ${MAKEFILE_LIST} | awk -F'(:|##|])\\s*' '/##\s*[A-Z_]+:.*$$/ {printf "  ${BLU}%-18s ${RED}%s]${RST} %s\n", $$2, $$3, $$4}'

clean: ## Deletes the build dir
	cmake -E rm -rf build

distclean: ## Resets the repo back to its state at checkout
	git clean -xdff

debug: ## Builds the Debug configuration
	cmake --workflow --preset=debug

test: ## Runs tests
	cmake --workflow --preset=test

cov: ## Runs tests with coverage
	cmake --workflow --preset=cov

format: ## Reformats code
	cmake --workflow --preset=format

lint: ## Runs static analysis tools
	cmake --workflow --preset=lint

install: ## Installs the library 
	cmake --workflow --preset=install

update: ## Updates dependent files after updating set(PROJECT_VERSION ...)
	echo TODO: Version

hooks: ## Commands to run as git hooks
	git clang-format
