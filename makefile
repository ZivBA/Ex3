
myString: MyStringO
	ar rcs libmyString.a MyString.o

MyStringO:
	gcc -c -std=c99 -D NDEBUG -Wextra -Wvla -Wall MyString.c -o MyString.o

main: myString
	gcc  -std=c99 -Wextra -Wvla -Wall MyStringMain.c MyString.a -o MyStringMain

tests:
	gcc -std=c99 -Wextra -Wvla -Wall MyString.c -o MyString
	./MyString

clean:
	rm *.o