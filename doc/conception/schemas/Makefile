# Définition des variables
SOURCE_DIR_GENERAL := anime_uml_generated
SOURCE_DIR_DETAILED_SP := anime_uml_detailed_sp_generated
SOURCE_DIR_DETAILED_SS := anime_uml_detailed_ss_generated
SOURCE_DIR_DETAILED_AOP := anime_uml_detailed_aop_generated
SOURCE_EXT := puml
DEST_EXT := plantuml

# Récupération de la liste des fichiers à renommer
SOURCE_FILES_GENERAL := $(wildcard $(SOURCE_DIR_GENERAL)/*.$(SOURCE_EXT))
SOURCE_FILES_DETAILED_SP := $(wildcard $(SOURCE_DIR_DETAILED_SP)/*.$(SOURCE_EXT))
SOURCE_FILES_DETAILED_SS := $(wildcard $(SOURCE_DIR_DETAILED_SS)/*.$(SOURCE_EXT))
SOURCE_FILES_DETAILED_AOP := $(wildcard $(SOURCE_DIR_DETAILED_AOP)/*.$(SOURCE_EXT))

all:
	$(MAKE) -C anime_uml_generated
	$(MAKE) -C anime_uml_detailed_sp_generated
	$(MAKE) -C anime_uml_detailed_ss_generated
	$(MAKE) -C anime_uml_detailed_aop_generated
	$(MAKE) -f $(MAKEFILE_LIST) -f Makefile rename_general
	$(MAKE) -f $(MAKEFILE_LIST) -f Makefile rename_detailed_sp
	$(MAKE) -f $(MAKEFILE_LIST) -f Makefile rename_detailed_ss
	$(MAKE) -f $(MAKEFILE_LIST) -f Makefile rename_detailed_aop

rename_general: $(patsubst %.$(SOURCE_EXT),%.$(DEST_EXT),$(SOURCE_FILES_GENERAL))

%.$(DEST_EXT): %.$(SOURCE_EXT)
	mv $< $@

rename_detailed_sp: $(patsubst %.$(SOURCE_EXT),%.$(DEST_EXT),$(SOURCE_FILES_DETAILED_SP))

%.$(DEST_EXT): %.$(SOURCE_EXT)
	mv $< $@

rename_detailed_ss: $(patsubst %.$(SOURCE_EXT),%.$(DEST_EXT),$(SOURCE_FILES_DETAILED_SS))

%.$(DEST_EXT): %.$(SOURCE_EXT)
	mv $< $@

rename_detailed_aop: $(patsubst %.$(SOURCE_EXT),%.$(DEST_EXT),$(SOURCE_FILES_DETAILED_AOP))

%.$(DEST_EXT): %.$(SOURCE_EXT)
	mv $< $@

clean:
	$(MAKE) -C anime_uml_generated clean
	$(MAKE) -C anime_uml_detailed_sp_generated clean
	$(MAKE) -C anime_uml_detailed_ss_generated clean
	$(MAKE) -C anime_uml_detailed_aop_generated clean

.PHONY: rename
.PHONY: clean