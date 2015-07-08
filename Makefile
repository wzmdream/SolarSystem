test : main.cpp
	g++   main.cpp  -D_STDCALL_SUPPORTED -D_M_IX86 -L"lib" -lwinmm  glut32.lib -lopengl32 -lglu32 -o test
clean:	
	rm  test.exe