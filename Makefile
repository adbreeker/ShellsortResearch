# Makefile for ShellsortResearch Project
# Author: GitHub Copilot
# Date: January 24, 2026

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -fopenmp

# Project settings
TARGET = ShellsortResearch
MAIN_SOURCE = ShellsortResearchMain.cpp
HEADERS = Components/Shellsort.hpp Components/ShellsortComparisions.hpp Components/FilesManagement.hpp Components/SearchingAlgorithms/GeneticAlgorithm_v1.hpp Components/SearchingAlgorithms/GeneticAlgorithm_v2.hpp Components/SearchingAlgorithms/GeneticAlgorithm_v3.hpp Components/SearchingAlgorithms/GeneticAlgorithm_v4.hpp Components/SearchingAlgorithms/ArtificialBeeColony.hpp Components/SearchingAlgorithms/CuckooSearch.hpp

# Directories
RESULTS_DIR = Results
BACKUP_DIR = Results/Backups
DATE = $(shell date +%Y-%m-%d_%H-%M-%S)

# Default target
.PHONY: all compile run backup clear clean help

all: compile

# Compilation with OpenMP
compile: $(TARGET)

$(TARGET): $(MAIN_SOURCE) $(HEADERS)
	@echo "Compiling $(TARGET)..."
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(MAIN_SOURCE)
	@echo "Compilation successful!"

# Run target - checks for executable and compiles if needed
run: compile
	echo "Running $(TARGET)..."; 
	./$(TARGET); 

# Backup target - copies Results to timestamped backup (excluding Backups folder)
backup:
	@echo "Creating backup of results..."
	@if [ ! -d "$(RESULTS_DIR)" ]; then \
		echo "Warning: $(RESULTS_DIR) directory not found!"; \
		exit 1; \
	fi
	@mkdir -p $(BACKUP_DIR)/$(DATE)
	@find $(RESULTS_DIR) -maxdepth 1 -type f -exec cp {} $(BACKUP_DIR)/$(DATE)/ \;
	@echo "Backup created at $(BACKUP_DIR)/$(DATE)/"
	@echo "Backup contains:"
	@ls -la $(BACKUP_DIR)/$(DATE)/

# Clear/Clean target - removes build artifacts
clear: clean

clean:
	@echo "Cleaning build artifacts..."
	@rm -f $(TARGET)
	@rm -f *.o *.obj
	@rm -f *.exe
	@rm -f *.ilk *.pdb
	@if [ -d "x64" ]; then rm -rf x64; fi
	@if [ -d "Shellsor.3279f0e8" ]; then rm -rf Shellsor.3279f0e8; fi
	@echo "Clean complete!"

# Help target
help:
	@echo "Available targets:"
	@echo "  compile       - Compile with OpenMP support"
	@echo "  run           - Run the program (compiles if needed)"
	@echo "  backup        - Backup Results folder to Backups/{timestamp}"
	@echo "  clear/clean   - Remove build artifacts"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make compile && make run"
	@echo "  make run"
	@echo "  make backup"
	@echo "  make clear"

# Dependencies
$(TARGET): $(HEADERS)