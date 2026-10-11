CC              := cc
CFLAGS          := -Wall -Wextra -MMD -MP
CPPFLAGS        := -Icli/src/headers \
	-I daemon/src/headers \
	-I common/ipc \
	-I common/containers/vector \
	-I common/containers/indexmap
LDFLAGS         :=

CLI             := taskmasterctl
DAEMON          := taskmasterd
COMMON_SO       := libtaskmaster_common.so

BUILD_DIR       := build
CLI_BUILD       := $(BUILD_DIR)/cli
DAEMON_BUILD    := $(BUILD_DIR)/daemon
COMMON_BUILD    := $(BUILD_DIR)/common

CLI_SRCS        := cli/src/main.c

# pollfd.c and commands.c are not listed because the current daemon sources
# include them directly from server and supervisor translation units.
DAEMON_SRCS     := daemon/src/logger/logger.c \
	daemon/src/main.c \
	daemon/src/parser/parser.c \
	daemon/src/program/program.c \
	daemon/src/server/pollable.c \
	daemon/src/server/server.c \
	daemon/src/server/server_setup.c \
	daemon/src/supervisor/supervisor.c

COMMON_SRCS     := common/containers/indexmap/indexmap.c \
	common/containers/indexmap/indexmap_setup.c \
	common/containers/indexmap/strmap_setup.c \
	common/containers/vector/vector_get.c \
	common/containers/vector/vector_mod.c \
	common/containers/vector/vector_setup.c \
	common/containers/vector/vector_str.c \
	common/ipc/ipc.c

CLI_OBJS        := $(patsubst cli/%.c,$(CLI_BUILD)/%.o,$(CLI_SRCS))
DAEMON_OBJS     := $(patsubst daemon/%.c,$(DAEMON_BUILD)/%.o,$(DAEMON_SRCS))
COMMON_OBJS     := $(patsubst common/%.c,$(COMMON_BUILD)/%.o,$(COMMON_SRCS))
DEPS            := $(CLI_OBJS:.o=.d) $(DAEMON_OBJS:.o=.d) $(COMMON_OBJS:.o=.d)

.DEFAULT_GOAL   := all

all: $(CLI) $(DAEMON)

cli: $(CLI)

daemon: $(DAEMON)

common: $(COMMON_SO)

$(CLI): $(CLI_OBJS) $(COMMON_SO)
	$(CC) $(LDFLAGS) $(CLI_OBJS) -L. -Wl,--no-as-needed \
		-ltaskmaster_common -Wl,-rpath,'$$ORIGIN' -o $@

$(DAEMON): $(DAEMON_OBJS) $(COMMON_SO)
	$(CC) $(LDFLAGS) $(DAEMON_OBJS) -L. -Wl,--no-as-needed \
		-ltaskmaster_common -lyaml -Wl,-rpath,'$$ORIGIN' -o $@

$(COMMON_SO): $(COMMON_OBJS)
	$(CC) -shared $(LDFLAGS) $^ -o $@

$(CLI_BUILD)/%.o: cli/%.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(DAEMON_BUILD)/%.o: daemon/%.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(COMMON_BUILD)/%.o: common/%.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -fPIC -c $< -o $@

clean: clean-cli clean-daemon clean-common

clean-cli:
	$(RM) -r $(CLI_BUILD)

clean-daemon:
	$(RM) -r $(DAEMON_BUILD)

clean-common:
	$(RM) -r $(COMMON_BUILD)

# Underscored aliases are kept for the common 42-style target spelling.
clean_cli: clean-cli

clean_daemon: clean-daemon

clean_common: clean-common

fclean: clean
	$(RM) $(CLI) $(DAEMON) $(COMMON_SO)
	@rmdir $(BUILD_DIR) 2>/dev/null || true

fclean-cli: clean-cli
	$(RM) $(CLI)
	@rmdir $(BUILD_DIR) 2>/dev/null || true

fclean-daemon: clean-daemon
	$(RM) $(DAEMON)
	@rmdir $(BUILD_DIR) 2>/dev/null || true

fclean-common: clean-common
	$(RM) $(COMMON_SO)
	@rmdir $(BUILD_DIR) 2>/dev/null || true

fclean_cli: fclean-cli

fclean_daemon: fclean-daemon

fclean_common: fclean-common

re: fclean all

re-cli: fclean-cli cli

re-daemon: fclean-daemon daemon

-include $(DEPS)

.PHONY: all cli daemon common clean clean-cli clean-daemon clean-common \
	clean_cli clean_daemon clean_common fclean fclean-cli fclean-daemon \
	fclean-common fclean_cli fclean_daemon fclean_common re re-cli re-daemon
