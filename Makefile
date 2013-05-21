CXXFLAGS=-O3 -I/usr/local/include -I/Users/hide/Downloads/marisa-0.2.4/lib -DHSDS_USE_SSE3 -mssse3 -msse3
OBJEXT=o

benchmark_OBJECTS = benchmark.$(OBJEXT) \
			   bit-vector.$(OBJEXT)

test_OBJECTS = test.$(OBJEXT) \
								bit-vector.$(OBJEXT)

TARGETS=test benchmark

benchmark: $(benchmark_OBJECTS)
	$(CXX) -o $@ $(benchmark_OBJECTS) -lmarisa -lux

test: $(test_OBJECTS)
	$(CXX) -o $@ $(test_OBJECTS) -lmarisa

.cpp.o:
	$(CXX) -c $< $(CXXFLAGS)

clean:
	rm *.$(OBJEXT) $(TARGETS)

.PHONY: clean
