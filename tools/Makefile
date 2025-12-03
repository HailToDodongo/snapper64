CXXFLAGS += -O3 -std=c++23
OBJDIR = build

all: data_converter data_bundler

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

data_converter: build/dataConverter.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $ $(LINKFLAGS)

data_bundler: build/dataBundler.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $ $(LINKFLAGS)

clean:
	rm -rf ./build ./data_converter
