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
CMAKE		=	cmake
MAKE		=	make

# Nom des exécutables (adapte selon ce que tu as dans tes CMakeLists.txt)
SERVER_EXE	=	rtype_server
CLIENT_EXE	=	rtype_client

all:	$(BUILD_DIR)/$(SERVER_EXE) $(BUILD_DIR)/$(CLIENT_EXE)
	@echo "$(GREEN)Compilation terminée !$(NC)"
	@echo "$(CYAN)→ Serveur : $(BUILD_DIR)/$(SERVER_EXE)$(NC)"
	@echo "$(CYAN)→ Client  : $(BUILD_DIR)/$(CLIENT_EXE)$(NC)"

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
	@rm -rf $(BUILD_DIR)
	@echo "$(YELLOW)Suppression totale du dossier build$(NC)"

re:	fclean all

# Bonus : lance le serveur directement
run_server: all
	@echo "$(GREEN)Lancement du serveur...$(NC)"
	@$(BUILD_DIR)/src/server/$(SERVER_EXE)

# Bonus : lance le client
run_client: all
	@echo "$(GREEN)Lancement du client...$(NC)"
	@$(BUILD_DIR)/src/client/$(CLIENT_EXE)

.PHONY: all clean fclean re run_server run_client