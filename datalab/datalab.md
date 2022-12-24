# Datalab

## The answers of all the questions

All in [bits.c](bits.c), also with my comments there

## How to launch

### Using a virtual machine

Like Vmware, launch a 32-bit OS to compile and run the code

### Using docker(recommended)

Source from [Yansongsongsong](https://github.com/Yansongsongsong/CSAPP-Experiments)

Firstly using a docker:

`docker run -d -p 9912:22 --name datalab yansongsongsong/csapp:datalab`

Then using vscode plugin **remote ssh**

`ssh root@127.0.0.1 -p 9912`

password: `THEPASSWORDYOUCREATED`

## How to test

To show your correctness:

`make btest && ./btest`

To make sure you are not cheating:

`./dlc bits.c`
