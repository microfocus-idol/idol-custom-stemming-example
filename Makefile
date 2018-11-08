SO_NAME=libexamplestemmer.so

lib:
	g++ -g -std=c++11 -fPIC -shared -Icontrib/OleanderStemmingLibrary -o ${SO_NAME} examplestemmer.cpp

clean:
	rm -f ${SO_NAME}
