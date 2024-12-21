# Compiling the java file to get JNI header file
javac -d out -h src/c -sourcepath src src/App.java

# Compiling the C code to dll
gcc -shared -o src/c/App.dll -I"C:\Program Files\Eclipse Adoptium\jdk-21.0.4.7-hotspot\include" -I"C:\Program Files\Eclipse Adoptium\jdk-21.0.4.7-hotspot\include\win32" src/c/App.c