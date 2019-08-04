LIBS=-lstdc++
OUTPUT=-o calc
FILE=calc.cpp
GPP=g++

make: $(FILE)
	$(GPP) $(FILE) $(LIBS) $(OUTPUT)
