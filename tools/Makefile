CXXFLAGS += -O3 -std=c++20
OBJDIR = build

OBJ = build/dataConverter.o

all: data_converter

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

data_converter: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $ $(LINKFLAGS)

clean:
	rm -rf ./build ./data_converter
