# calc
A calculator written in C and GTk+ 3 and Glade UI

Is this best calculater written in C :- NOOO
is this memeory safe : Definitely NOT
is this fast : NO

will this get me a pass mark for Day One Assignment of computer science class : probably yes (if your professor is not Linus Torvalds )
  
Installation
------------

Clone the repository:

```sh
git clone  git clone git@github.com:lahirunirmalx/calc.git calc
cd calc
```

and install the dependencies with composer:

```sh
 
sudo apt install libgtk-3-dev
gcc $(pkg-config --cflags gtk+-3.0) -o test calc.c $(pkg-config --libs gtk+-3.0) -lm
./test 
```
 
