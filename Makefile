##
## EPITECH PROJECT, 2025
## r-type-mirror
## File description:
## Makefile
##

# Couleurs pour le style
GREEN		=	\033[0;32m
YELLOW		=	\033[0;33m
CYAN		=	\033[0;36m
NC			=	\033[0m

# Chemins
BUILD_DIR	=	./build
BIN_DIR		=	./binaries
CMAKE		=	cmake
MAKE		=	make

# Nom des exécutables (adapte selon ce que tu as dans tes CMakeLists.txt)
SERVER_EXE	=	rtype_server
CLIENT_EXE	=	rtype_client

all:
	@$(MAKE) -C $(BUILD_DIR) --no-print-directory
	mkdir -p $(BIN_DIR)
	cp $(BUILD_DIR)/src/server/$(SERVER_EXE) $(BIN_DIR)/
	cp $(BUILD_DIR)/src/client/$(CLIENT_EXE) $(BIN_DIR)/

build:	$(BUILD_DIR)/$(SERVER_EXE) $(BUILD_DIR)/$(CLIENT_EXE)
	@echo "$(GREEN)Compilation terminée !$(NC)"
	@echo "$(CYAN)→ Serveur : $(BUILD_DIR)/$(SERVER_EXE)$(NC)"
	@echo "$(CYAN)→ Client  : $(BUILD_DIR)/$(CLIENT_EXE)$(NC)"
	mkdir -p $(BIN_DIR)
	cp $(BUILD_DIR)/$(SERVER_EXE) $(BIN_DIR)/
	cp $(BUILD_DIR)/$(CLIENT_EXE) $(BIN_DIR)/

$(BUILD_DIR)/$(SERVER_EXE) $(BUILD_DIR)/$(CLIENT_EXE): $(BUILD_DIR)/Makefile
	@echo "$(YELLOW)Compilation en cours...$(NC)"
	@$(MAKE) -C $(BUILD_DIR) --no-print-directory

$(BUILD_DIR)/Makefile:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) .. -G "Unix Makefiles"

clean:
	@$(MAKE) -C $(BUILD_DIR) clean --no-print-directory 2>/dev/null || true
	@echo "$(YELLOW)Nettoyage des objets...$(NC)"

fclean: clean
	@find $(BUILD_DIR) -mindepth 1 ! -name Makefile -delete 2>/dev/null || true
	@echo "$(YELLOW)Suppression du dossier build (sauf Makefile)$(NC)"
	@rm -rf $(BIN_DIR)
	@echo "$(YELLOW)Suppression totale du dossier binaries$(NC)"

re:	fclean all

# Bonus : lance le serveur directement
run_server: all
	@echo "$(GREEN)Lancement du serveur...$(NC)"
	@$(BUILD_DIR)/src/server/$(SERVER_EXE)

# Bonus : lance le client
run_client: all
	@echo "$(GREEN)Lancement du client...$(NC)"
	@$(BUILD_DIR)/src/client/$(CLIENT_EXE)

.PHONY: all clean fclean re run_server run_client build