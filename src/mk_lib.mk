$(shell if [ ! -e $(PATH_BUILD) ];then mkdir -p $(PATH_BUILD); fi)
$(shell for dir in $(SUB_DIR); do if [ ! -e $(PATH_BUILD)/$$dir ];then mkdir -p $(PATH_BUILD)/$$dir; fi; done)

SOURCE_FILE := $(foreach d, $(SUB_DIR), $(wildcard $(d)/*.cpp $(d)/*.cc))
SOURCE_FILE += $(wildcard ./*.cpp ./*.cc)

OBJ_FILE := $(addprefix $(PATH_BUILD)/, $(addsuffix .o, $(basename $(SOURCE_FILE))))
DEP_FILE := $(patsubst %.o, %.d, $(OBJ_FILE))

all: $(PATH_BUILD)/$(TARGET)
debug: $(PATH_BUILD)/$(TARGET)

$(PATH_BUILD)/$(TARGET): $(OBJ_FILE)
	$(AR) -o $@ $(OBJ_FILE)
	@echo Build: $@

$(PATH_BUILD)/%.o: %.cpp
	$(CXX) -c $(CXX_FLAGS) $(DEFINE) -MMD $(INC_ALL) $< -o $@
	@echo compiled: $<

$(PATH_BUILD)/%.o: %.cc
	$(CXX) -c $(CXX_FLAGS) $(DEFINE) -MMD $(INC_ALL) $< -o $@
	@echo compiled: $<

ifneq ($(MAKECMDGOALS), clean)
-include $(DEP_FILE)
endif

clean:
	@rm -rf $(PATH_BUILD)
	@echo clean all....