CC = gcc
CILKCC = /usr/local/OpenCilk-9.0.1-Linux/bin/clang


BUILD_DIR := ./linux_build
SRC_DIRS := ./src

# Colors
GREEN = \033[1;32m
RED = \033[1;31m
NC = \033[0m

# Directories
LIBRARIES_SRC := $(shell find $(SRC_DIRS)/libraries $(SRC_DIRS)/ext_libraries -name '*.c')
LIBRARIES_SRC := $(LIBRARIES_SRC:%=$(BUILD_DIR)/%.o)

SERIAL_SRC := $(shell find $(SRC_DIRS)/Serial -name '*.c') $(SRC_DIRS)/serial_main.c
SERIAL_SRC := $(SERIAL_SRC:%=$(BUILD_DIR)/%.o)

PTHREAD_SRC := $(shell find $(SRC_DIRS)/Pthreads -name '*.c') $(SRC_DIRS)/pthread_main.c
PTHREAD_SRC := $(PTHREAD_SRC:%=$(BUILD_DIR)/%.o)

OPENMP_SRC := $(shell find $(SRC_DIRS)/OpenMP -name '*.c') $(SRC_DIRS)/openmp_main.c
OPENMP_SRC := $(OPENMP_SRC:%=$(BUILD_DIR)/%.o)

OPENCILK_SRC := $(shell find $(SRC_DIRS)/OpenCilk -name '*.c') $(SRC_DIRS)/opencilk_main.c
OPENCILK_SRC := $(OPENCILK_SRC:%=$(BUILD_DIR)/%.o)


# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS := $(INC_FLAGS) -O3 -w


all: pre_build build_serial build_pthread build_openmp build_opencilk post_build


pre_build:
	@echo "$(GREEN)Building all...$(NC)"


post_build:
	@echo "$(GREEN)Build all successful!!$(NC)"


build_serial: $(SERIAL_SRC) $(LIBRARIES_SRC)
	@echo "    $(GREEN)Building Serial binary...$(NC)"
	@$(CC) $(CFLAGS) -o $(BUILD_DIR)/serial.out $(SERIAL_SRC) $(LIBRARIES_SRC)
	@echo "    $(GREEN)Build finished successfully!$(NC)"
	@echo


build_pthread: $(PTHREAD_SRC) $(LIBRARIES_SRC)
	@echo "    $(GREEN)Building pthread binary...$(NC)"
	@$(CC) $(CFLAGS) -o $(BUILD_DIR)/pthread.out $(PTHREAD_SRC) $(LIBRARIES_SRC) -lpthread
	@echo "    $(GREEN)Build finished successfully!$(NC)"
	@echo

build_openmp: $(OPENMP_SRC) $(LIBRARIES_SRC)
	@echo "    $(GREEN)Building OpenMP binary...$(NC)"
	@$(CC) $(CFLAGS) -o $(BUILD_DIR)/openmp.out $(OPENMP_SRC) $(LIBRARIES_SRC) -fopenmp
	@echo "    $(GREEN)Build finished successfully!$(NC)"
	@echo


build_opencilk: $(OPENCILK_SRC) $(LIBRARIES_SRC)
	@echo "    $(GREEN)Building OpenCilk binary...$(NC)"
	@$(CILKCC) $(CFLAGS) -o $(BUILD_DIR)/opencilk.out $(OPENCILK_SRC) $(LIBRARIES_SRC) -fcilkplus
	@echo "    $(GREEN)Build finished successfully!$(NC)"
	@echo


# Cutom target to build opencilk files with the clang compiler
$(BUILD_DIR)/$(SRC_DIRS)/OpenCilk/%.c.o: $(SRC_DIRS)/OpenCilk/%.c
	@mkdir -p $(dir $@)
	@$(CILKCC) $(CFLAGS) -c $< -o $@ -fcilkplus


$(BUILD_DIR)/$(SRC_DIRS)/opencilk_main.c.o: $(SRC_DIRS)/opencilk_main.c
	@mkdir -p $(dir $@)
	@$(CILKCC) $(CFLAGS) -c $< -o $@ -fcilkplus


$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@


run_serial: build_serial
	@echo run forrest run



.PHONY: clean
clean:
	@echo "$(RED)Clearing build directory...$(NC)"
	@rm -rf $(BUILD_DIR)
	@echo "$(GREEN)Done!$(NC)"
